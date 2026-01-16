#include "statisticsworker.h"
#include "idatabase.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QCoreApplication>

StatisticsWorker::StatisticsWorker(QObject *parent)
    : QObject(parent)
    , isRunning(false)
{
    // 创建定时器
    statisticsTimer = new QTimer(this);
    syncTimer = new QTimer(this);

    // 连接定时器信号
    connect(statisticsTimer, &QTimer::timeout, this, &StatisticsWorker::onStatisticsTimeout);
    connect(syncTimer, &QTimer::timeout, this, &StatisticsWorker::onSyncTimeout);

    // 将工作移动到独立线程
    moveToThread(&workerThread);

    // 连接线程启动信号
    connect(&workerThread, &QThread::started, this, &StatisticsWorker::startWork);
    connect(&workerThread, &QThread::finished, this, [this]() {
        isRunning = false;
        qDebug() << "统计线程已停止";
    });
}

StatisticsWorker::~StatisticsWorker()
{
    stopWork();
    workerThread.quit();
    workerThread.wait();
}

void StatisticsWorker::startWork()
{
    if (isRunning) return;

    isRunning = true;
    qDebug() << "统计线程已启动";

    // 每小时执行一次统计（3600000毫秒）
    statisticsTimer->start(3600000);

    // 每6小时执行一次同步（21600000毫秒）
    syncTimer->start(21600000);

    // 立即执行一次统计和同步
    generateDailyReport();
    syncData();

    emit statisticsUpdated(calculateDailyStatistics());
}

void StatisticsWorker::stopWork()
{
    if (!isRunning) return;

    statisticsTimer->stop();
    syncTimer->stop();
    isRunning = false;

    qDebug() << "统计线程正在停止...";
}

void StatisticsWorker::generateDailyReport()
{
    qDebug() << "正在生成日报表...";
    emit progressUpdated(10, "正在收集今日数据...");

    QJsonObject report = createDailyReport();

    emit progressUpdated(90, "正在保存报表...");

    // 保存报表到文件
    QString reportsDir = QCoreApplication::applicationDirPath() + "/reports";
    QDir dir;
    if (!dir.exists(reportsDir)) {
        dir.mkpath(reportsDir);
    }

    QString fileName = QString("%1/daily_report_%2.json")
                           .arg(reportsDir)
                           .arg(QDate::currentDate().toString("yyyy-MM-dd"));

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(report);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        qDebug() << "日报表已保存：" << fileName;
    }

    emit progressUpdated(100, "日报表生成完成");
    emit reportGenerated(report);
}

void StatisticsWorker::generateMonthlyReport()
{
    qDebug() << "正在生成月报表...";
    emit progressUpdated(10, "正在收集本月数据...");

    QJsonObject report = createMonthlyReport();

    emit progressUpdated(90, "正在保存报表...");

    // 保存报表到文件
    QString reportsDir = QCoreApplication::applicationDirPath() + "/reports";
    QDir dir;
    if (!dir.exists(reportsDir)) {
        dir.mkpath(reportsDir);
    }

    QString fileName = QString("%1/monthly_report_%2.json")
                           .arg(reportsDir)
                           .arg(QDate::currentDate().toString("yyyy-MM"));

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(report);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        qDebug() << "月报表已保存：" << fileName;
    }

    emit progressUpdated(100, "月报表生成完成");
    emit reportGenerated(report);
}

void StatisticsWorker::syncData()
{
    qDebug() << "开始同步数据...";
    emit progressUpdated(0, "正在连接服务器...");

    // 这里会发出信号，由主线程的网络模块处理实际的网络请求
    // 实际同步在NetSync类中实现
    emit syncCompleted(true, "数据同步完成（本地模式）");
}

QVariantMap StatisticsWorker::calculateDailyStatistics()
{
    QVariantMap stats;

    // 今日就诊人数
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM MedicalRecord WHERE DATE(VISIT_DATE) = DATE('now', 'localtime')");
    if (query.exec() && query.next()) {
        stats["todayPatients"] = query.value(0).toInt();
    }

    // 今日处方数量
    query.prepare("SELECT COUNT(DISTINCT record_id) FROM prescriptions "
                  "WHERE DATE(prescribed_at) = DATE('now', 'localtime')");
    if (query.exec() && query.next()) {
        stats["todayPrescriptions"] = query.value(0).toInt();
    }

    // 药品使用数量
    query.prepare("SELECT SUM(quantity) FROM prescriptions "
                  "WHERE DATE(prescribed_at) = DATE('now', 'localtime')");
    if (query.exec() && query.next()) {
        stats["todayDrugUsage"] = query.value(0).toInt();
    }

    // 预约统计
    query.prepare("SELECT COUNT(*) FROM appointments "
                  "WHERE DATE(appoint_date) = DATE('now', 'localtime')");
    if (query.exec() && query.next()) {
        stats["todayAppointments"] = query.value(0).toInt();
    }

    return stats;
}

void StatisticsWorker::calculateDailyStatistics(QVariantMap &stats)
{
    stats = calculateDailyStatistics();
}

void StatisticsWorker::calculateMonthlyStatistics(QVariantMap &stats)
{
    // 月度就诊人数
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM MedicalRecord WHERE strftime('%Y-%m', VISIT_DATE) = strftime('%Y-%m', 'now', 'localtime')");
    if (query.exec() && query.next()) {
        stats["monthPatients"] = query.value(0).toInt();
    }

    // 月度处方数量
    query.prepare("SELECT COUNT(DISTINCT record_id) FROM prescriptions "
                  "WHERE strftime('%Y-%m', prescribed_at) = strftime('%Y-%m', 'now', 'localtime')");
    if (query.exec() && query.next()) {
        stats["monthPrescriptions"] = query.value(0).toInt();
    }

    // 药品使用数量
    query.prepare("SELECT SUM(quantity) FROM prescriptions "
                  "WHERE strftime('%Y-%m', prescribed_at) = strftime('%Y-%m', 'now', 'localtime')");
    if (query.exec() && query.next()) {
        stats["monthDrugUsage"] = query.value(0).toInt();
    }

    // 预约统计
    query.prepare("SELECT COUNT(*) FROM appointments "
                  "WHERE strftime('%Y-%m', appoint_date) = strftime('%Y-%m', 'now', 'localtime')");
    if (query.exec() && query.next()) {
        stats["monthAppointments"] = query.value(0).toInt();
    }

    // 今日数据（用于对比）
    stats["todayPatients"] = calculateDailyStatistics()["todayPatients"];
}

QJsonObject StatisticsWorker::createDailyReport()
{
    QJsonObject report;

    // 基本信息
    report["reportType"] = "daily";
    report["reportDate"] = QDate::currentDate().toString("yyyy-MM-dd");
    report["generateTime"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    // 统计数据
    QVariantMap dailyStats = calculateDailyStatistics();
    report["todayPatients"] = dailyStats["todayPatients"].toInt();
    report["todayPrescriptions"] = dailyStats["todayPrescriptions"].toInt();
    report["todayDrugUsage"] = dailyStats["todayDrugUsage"].toInt();
    report["todayAppointments"] = dailyStats["todayAppointments"].toInt();

    // 药品使用排行
    QJsonArray drugUsageArray;
    QSqlQuery query;
    query.prepare("SELECT drug_name, SUM(quantity) as total_qty "
                  "FROM prescriptions "
                  "WHERE DATE(prescribed_at) = DATE('now', 'localtime') "
                  "GROUP BY drug_name "
                  "ORDER BY total_qty DESC LIMIT 10");
    if (query.exec()) {
        while (query.next()) {
            QJsonObject drugObj;
            drugObj["name"] = query.value("drug_name").toString();
            drugObj["quantity"] = query.value("total_qty").toInt();
            drugUsageArray.append(drugObj);
        }
    }
    report["drugUsageRanking"] = drugUsageArray;

    return report;
}

QJsonObject StatisticsWorker::createMonthlyReport()
{
    QJsonObject report;

    // 基本信息
    report["reportType"] = "monthly";
    report["reportMonth"] = QDate::currentDate().toString("yyyy-MM");
    report["generateTime"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    // 统计数据
    QVariantMap monthStats;
    calculateMonthlyStatistics(monthStats);
    report["monthPatients"] = monthStats["monthPatients"].toInt();
    report["monthPrescriptions"] = monthStats["monthPrescriptions"].toInt();
    report["monthDrugUsage"] = monthStats["monthDrugUsage"].toInt();
    report["monthAppointments"] = monthStats["monthAppointments"].toInt();

    // 每日就诊趋势
    QJsonArray dailyTrendArray;
    QSqlQuery query;
    query.prepare("SELECT DATE(VISIT_DATE) as visit_date, COUNT(*) as patient_count "
                  "FROM MedicalRecord "
                  "WHERE strftime('%Y-%m', VISIT_DATE) = strftime('%Y-%m', 'now', 'localtime') "
                  "GROUP BY DATE(VISIT_DATE) "
                  "ORDER BY DATE(VISIT_DATE)");
    if (query.exec()) {
        while (query.next()) {
            QJsonObject dayObj;
            dayObj["date"] = query.value("visit_date").toString();
            dayObj["count"] = query.value("patient_count").toInt();
            dailyTrendArray.append(dayObj);
        }
    }
    report["dailyTrend"] = dailyTrendArray;

    return report;
}

void StatisticsWorker::onStatisticsTimeout()
{
    qDebug() << "定时统计执行...";
    QVariantMap stats = calculateDailyStatistics();
    emit statisticsUpdated(stats);
}

void StatisticsWorker::onSyncTimeout()
{
    qDebug() << "定时同步执行...";
    syncData();
}
