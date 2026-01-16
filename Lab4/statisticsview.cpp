#include "statisticsview.h"
#include "ui_statisticsview.h"
#include "statisticsworker.h"
#include "networksync.h"
#include "idatabase.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QTextEdit>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QDebug>

StatisticsView::StatisticsView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StatisticsView)
{
    ui->setupUi(this);

    // 初始化统计显示
    updateStatisticsDisplay();

    // 连接按钮信号
    connect(ui->btDailyReport, &QPushButton::clicked, this, &StatisticsView::onGenerateDailyReport);
    connect(ui->btMonthlyReport, &QPushButton::clicked, this, &StatisticsView::onGenerateMonthlyReport);
    connect(ui->btSyncDrugs, &QPushButton::clicked, this, &StatisticsView::onSyncDrugs);
    connect(ui->btSyncDiagnoses, &QPushButton::clicked, this, &StatisticsView::onSyncDiagnoses);
    connect(ui->btSyncAll, &QPushButton::clicked, this, &StatisticsView::onSyncAll);
}

StatisticsView::~StatisticsView()
{
    delete ui;
}

void StatisticsView::onGenerateDailyReport()
{
    appendLog("正在生成日报表...");

    // 这里调用实际的报表生成逻辑
    QVariantMap dailyStats = IDatabase::getInstance().getDailyStatistics();

    // 显示统计结果
    QString report = QString("日报表统计：\n"
                             "今日就诊人数：%1\n"
                             "今日预约数：%2\n"
                             "今日处方数：%3\n"
                             "今日收入：¥%4")
                         .arg(dailyStats["todayPatients"].toInt())
                         .arg(dailyStats["todayAppointments"].toInt())
                         .arg(dailyStats["todayPrescriptions"].toInt())
                         .arg(dailyStats["todayRevenue"].toDouble(), 0, 'f', 2);

    appendLog(report);
}

void StatisticsView::onGenerateMonthlyReport()
{
    appendLog("正在生成月报表...");

    QVariantMap monthStats = IDatabase::getInstance().getMonthlyStatistics();

    QString report = QString("月报表统计：\n"
                             "本月就诊人数：%1\n"
                             "本月预约数：%2\n"
                             "本月处方数：%3\n"
                             "本月收入：¥%4")
                         .arg(monthStats["monthPatients"].toInt())
                         .arg(monthStats["monthAppointments"].toInt())
                         .arg(monthStats["monthPrescriptions"].toInt())
                         .arg(monthStats["monthRevenue"].toDouble(), 0, 'f', 2);

    appendLog(report);
}

void StatisticsView::onSyncDrugs()
{
    appendLog("开始同步药品信息...");
    // 实际同步逻辑在NetSync类中实现
    appendLog("药品同步功能需要配置服务器地址后使用");
}

void StatisticsView::onSyncDiagnoses()
{
    appendLog("开始同步诊断标准...");
    appendLog("诊断标准同步功能需要配置服务器地址后使用");
}

void StatisticsView::onSyncAll()
{
    appendLog("开始同步所有数据...");
    appendLog("同步功能需要配置服务器地址后使用");
}

void StatisticsView::onReportGenerated(const QJsonObject &report)
{
    appendLog("报表生成成功！");
    appendLog(QString("报表类型：%1").arg(report["reportType"].toString()));
}

void StatisticsView::onStatisticsUpdated(const QVariantMap &stats)
{
    Q_UNUSED(stats);
    updateStatisticsDisplay();
}

void StatisticsView::onSyncProgress(int progress, const QString &status)
{
    ui->progressBar->setValue(progress);
    appendLog(status);
}

void StatisticsView::onDrugSyncCompleted(bool success, const QString &message, int count)
{
    if (success) {
        appendLog(QString("药品同步成功：%1条记录").arg(count));
    } else {
        appendLog(QString("药品同步失败：%1").arg(message));
    }
    ui->progressBar->setValue(100);
}

void StatisticsView::onDiagnosisSyncCompleted(bool success, const QString &message, int count)
{
    if (success) {
        appendLog(QString("诊断标准同步成功：%1条记录").arg(count));
    } else {
        appendLog(QString("诊断标准同步失败：%1").arg(message));
    }
    ui->progressBar->setValue(100);
}

void StatisticsView::onErrorOccurred(const QString &error)
{
    appendLog(QString("错误：%1").arg(error));
    QMessageBox::warning(this, "错误", error);
}

void StatisticsView::updateStatisticsDisplay()
{
    // 获取今日统计
    QVariantMap dailyStats = IDatabase::getInstance().getDailyStatistics();
    ui->lbTodayPatients->setText(QString::number(dailyStats["todayPatients"].toInt()));
    ui->lbTodayAppointments->setText(QString::number(dailyStats["todayAppointments"].toInt()));
    ui->lbTodayPrescriptions->setText(QString::number(dailyStats["todayPrescriptions"].toInt()));
    ui->lbTodayRevenue->setText(QString("¥%1").arg(dailyStats["todayRevenue"].toDouble(), 0, 'f', 2));

    // 获取本月统计
    QVariantMap monthStats = IDatabase::getInstance().getMonthlyStatistics();
    ui->lbMonthPatients->setText(QString::number(monthStats["monthPatients"].toInt()));
    ui->lbMonthAppointments->setText(QString::number(monthStats["monthAppointments"].toInt()));
    ui->lbMonthPrescriptions->setText(QString::number(monthStats["monthPrescriptions"].toInt()));
    ui->lbMonthRevenue->setText(QString("¥%1").arg(monthStats["monthRevenue"].toDouble(), 0, 'f', 2));
}

void StatisticsView::appendLog(const QString &message)
{
    QString timestamp = QTime::currentTime().toString("hh:mm:ss");
    QString logEntry = QString("[%1] %2").arg(timestamp).arg(message);
    ui->teLog->append(logEntry);
}
