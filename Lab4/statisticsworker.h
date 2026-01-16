#ifndef STATISTICSWORKER_H
#define STATISTICSWORKER_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QDateTime>
#include <QVariantMap>
#include <QJsonObject>
#include <QJsonDocument>

class StatisticsWorker : public QObject
{
    Q_OBJECT
public:
    explicit StatisticsWorker(QObject *parent = nullptr);
    ~StatisticsWorker();

public slots:
    void startWork();                    // 开始后台工作
    void stopWork();                     // 停止后台工作
    void generateDailyReport();          // 生成日报表
    void generateMonthlyReport();        // 生成月报表
    void syncData();                     // 同步数据

signals:
    void reportGenerated(const QJsonObject &report);    // 报表生成完成
    void statisticsUpdated(const QVariantMap &stats);   // 统计更新
    void syncCompleted(bool success, const QString &message);  // 同步完成
    void errorOccurred(const QString &error);           // 发生错误
    void progressUpdated(int progress, const QString &status); // 进度更新

private:
    QThread workerThread;
    QTimer *statisticsTimer;             // 统计定时器
    QTimer *syncTimer;                   // 同步定时器
    bool isRunning;

    QVariantMap calculateDailyStatistics();  // 添加无参数版本声明
    void calculateDailyStatistics(QVariantMap &stats);
    void calculateMonthlyStatistics(QVariantMap &stats);
    QJsonObject createDailyReport();
    QJsonObject createMonthlyReport();

private slots:
    void onStatisticsTimeout();
    void onSyncTimeout();
};

#endif // STATISTICSWORKER_H
