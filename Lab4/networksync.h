#ifndef NETWORKSYNC_H
#define NETWORKSYNC_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslConfiguration>
#include <QJsonObject>
#include <QJsonDocument>
#include <QXmlStreamReader>
#include <QTimer>

struct DrugInfo {
    int id;
    QString name;
    QString category;
    QString manufacturer;
    double unitPrice;
    QString description;
    bool operator==(const DrugInfo &other) const {
        return id == other.id && name == other.name;
    }
};

struct DiagnosisStandard {
    int id;
    QString code;
    QString name;
    QString category;
    QString description;
    QString treatmentPlan;
    bool operator==(const DiagnosisStandard &other) const {
        return id == other.id && code == other.code;
    }
};

class NetSync : public QObject
{
    Q_OBJECT
public:
    explicit NetSync(QObject *parent = nullptr);
    ~NetSync();

    // 配置同步服务器地址
    void setServerAddress(const QString &address, int port = 8080);
    void setApiKey(const QString &apiKey);

    // 同步功能
    void syncDrugInfo();          // 同步药品信息
    void syncDiagnosisStandards(); // 同步诊断标准
    void syncAll();               // 同步所有数据

    // 获取本地缓存的数据
    QList<DrugInfo> getLocalDrugs() const;
    QList<DiagnosisStandard> getLocalDiagnoses() const;

public slots:
    void onSyncDrugInfoFinished();
    void onSyncDiagnosisFinished();

signals:
    void drugSyncCompleted(bool success, const QString &message, int count);
    void diagnosisSyncCompleted(bool success, const QString &message, int count);
    void syncProgress(int progress, const QString &status);
    void errorOccurred(const QString &error);
    void dataUpdated(); // 数据更新信号

private slots:
    void onDrugReplyFinished(QNetworkReply *reply);
    void onDiagnosisReplyFinished(QNetworkReply *reply);
    void onError(QNetworkReply::NetworkError error);

private:
    QNetworkAccessManager *networkManager;
    QString serverAddress;
    int serverPort;
    QString apiKey;

    QList<DrugInfo> localDrugs;
    QList<DiagnosisStandard> localDiagnoses;

    // 解析药品XML数据
    QList<DrugInfo> parseDrugXml(const QByteArray &xmlData);

    // 解析诊断标准JSON数据
    QList<DiagnosisStandard> parseDiagnosisJson(const QByteArray &jsonData);

    // 保存到本地数据库
    void saveDrugsToDatabase(const QList<DrugInfo> &drugs);
    void saveDiagnosesToDatabase(const QList<DiagnosisStandard> &diagnoses);

    // 从数据库加载
    void loadFromDatabase();
};

#endif // NETWORKSYNC_H
