#ifndef STATISTICSVIEW_H
#define STATISTICSVIEW_H

#include <QWidget>
#include <QJsonObject>
#include <QVariantMap>

namespace Ui {
class StatisticsView;
}

class StatisticsView : public QWidget
{
    Q_OBJECT

public:
    explicit StatisticsView(QWidget *parent = nullptr);
    ~StatisticsView();

private slots:
    void onGenerateDailyReport();
    void onGenerateMonthlyReport();
    void onSyncDrugs();
    void onSyncDiagnoses();
    void onSyncAll();

    void onReportGenerated(const QJsonObject &report);
    void onStatisticsUpdated(const QVariantMap &stats);
    void onSyncProgress(int progress, const QString &status);
    void onDrugSyncCompleted(bool success, const QString &message, int count);
    void onDiagnosisSyncCompleted(bool success, const QString &message, int count);
    void onErrorOccurred(const QString &error);

private:
    Ui::StatisticsView *ui;
    void updateStatisticsDisplay();
    void appendLog(const QString &message);
};

#endif // STATISTICSVIEW_H
