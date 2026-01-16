#include "networksync.h"
#include "idatabase.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>
#include <QCoreApplication>
#include <QEventLoop>
#include <QNetworkAccessManager>

NetSync::NetSync(QObject *parent)
    : QObject(parent)
    , networkManager(new QNetworkAccessManager(this))
    , serverPort(8080)
{
    // 默认服务器地址（可配置）
    serverAddress = "localhost";
    apiKey = "";

    // 连接网络管理器信号
    connect(networkManager, &QNetworkAccessManager::finished,
            this, &NetSync::onDrugReplyFinished);

    // 从本地数据库加载数据
    loadFromDatabase();

    qDebug() << "网络同步模块已初始化";
}

NetSync::~NetSync()
{
}

void NetSync::setServerAddress(const QString &address, int port)
{
    serverAddress = address;
    serverPort = port;
    qDebug() << "服务器地址已更新：" << serverAddress << ":" << serverPort;
}

void NetSync::setApiKey(const QString &key)
{
    apiKey = key;
}

void NetSync::syncDrugInfo()
{
    qDebug() << "开始同步药品信息...";
    emit syncProgress(0, "正在连接服务器...");

    // 构建请求URL
    QString url = QString("http://%1:%2/api/drugs").arg(serverAddress).arg(serverPort);
    if (!apiKey.isEmpty()) {
        url += QString("?api_key=%1").arg(apiKey);
    }

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // 发送GET请求
    QNetworkReply *reply = networkManager->get(request);

    // 临时连接完成信号（只使用一次）
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onDrugReplyFinished(reply);
    });

    // 连接错误信号
    connect(reply, &QNetworkReply::errorOccurred, this, [this](QNetworkReply::NetworkError error) {
        onError(error);
    });
}

void NetSync::syncDiagnosisStandards()
{
    qDebug() << "开始同步诊断标准...";
    emit syncProgress(0, "正在连接服务器...");

    // 构建请求URL
    QString url = QString("http://%1:%2/api/diagnoses").arg(serverAddress).arg(serverPort);
    if (!apiKey.isEmpty()) {
        url += QString("?api_key=%1").arg(apiKey);
    }

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // 发送GET请求
    QNetworkReply *reply = networkManager->get(request);

    // 临时连接完成信号
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onDiagnosisReplyFinished(reply);
    });

    // 连接错误信号
    connect(reply, &QNetworkReply::errorOccurred, this, [this](QNetworkReply::NetworkError error) {
        onError(error);
    });
}

void NetSync::syncAll()
{
    qDebug() << "开始同步所有数据...";
    emit syncProgress(0, "正在同步药品信息...");

    // 先同步药品信息
    syncDrugInfo();
}

void NetSync::onSyncDrugInfoFinished()
{
    // 槽函数实现（目前为空，可根据需要扩展）
    qDebug() << "药品同步完成信号已接收";
}

void NetSync::onSyncDiagnosisFinished()
{
    // 槽函数实现（目前为空，可根据需要扩展）
    qDebug() << "诊断标准同步完成信号已接收";
}

void NetSync::onDrugReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg = QString("药品同步失败：%1").arg(reply->errorString());
        qDebug() << errorMsg;
        emit drugSyncCompleted(false, errorMsg, 0);
        reply->deleteLater();
        return;
    }

    emit syncProgress(50, "正在解析药品数据...");

    // 获取响应数据
    QByteArray responseData = reply->readAll();
    reply->deleteLater();

    // 解析数据（支持XML和JSON格式）
    QList<DrugInfo> drugs;

    // 尝试解析JSON
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(responseData, &jsonError);

    if (jsonError.error == QJsonParseError::NoError && doc.isArray()) {
        // JSON格式
        QJsonArray drugArray = doc.array();
        for (int i = 0; i < drugArray.size(); ++i) {
            QJsonObject drugObj = drugArray[i].toObject();
            DrugInfo drug;
            drug.id = drugObj["id"].toInt();
            drug.name = drugObj["name"].toString();
            drug.category = drugObj["category"].toString();
            drug.manufacturer = drugObj["manufacturer"].toString();
            drug.unitPrice = drugObj["price"].toDouble();
            drug.description = drugObj["description"].toString();
            drugs.append(drug);
        }
    } else {
        // 尝试解析XML
        drugs = parseDrugXml(responseData);
    }

    emit syncProgress(80, "正在保存药品数据...");

    // 保存到本地数据库
    saveDrugsToDatabase(drugs);

    // 更新本地缓存
    localDrugs = drugs;

    emit syncProgress(100, "药品同步完成");
    emit drugSyncCompleted(true, QString("成功同步%1条药品信息").arg(drugs.size()), drugs.size());
    emit dataUpdated();

    qDebug() << "药品同步完成，共" << drugs.size() << "条记录";
}

void NetSync::onDiagnosisReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg = QString("诊断标准同步失败：%1").arg(reply->errorString());
        qDebug() << errorMsg;
        emit diagnosisSyncCompleted(false, errorMsg, 0);
        reply->deleteLater();
        return;
    }

    emit syncProgress(50, "正在解析诊断标准数据...");

    // 获取响应数据
    QByteArray responseData = reply->readAll();
    reply->deleteLater();

    // 解析JSON数据
    QList<DiagnosisStandard> diagnoses = parseDiagnosisJson(responseData);

    emit syncProgress(80, "正在保存诊断标准数据...");

    // 保存到本地数据库
    saveDiagnosesToDatabase(diagnoses);

    // 更新本地缓存
    localDiagnoses = diagnoses;

    emit syncProgress(100, "诊断标准同步完成");
    emit diagnosisSyncCompleted(true, QString("成功同步%1条诊断标准").arg(diagnoses.size()), diagnoses.size());
    emit dataUpdated();

    qDebug() << "诊断标准同步完成，共" << diagnoses.size() << "条记录";
}

void NetSync::onError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (reply) {
        QString errorMsg = QString("网络错误：%1").arg(reply->errorString());
        qDebug() << errorMsg;
        emit errorOccurred(errorMsg);
    }
}

QList<DrugInfo> NetSync::parseDrugXml(const QByteArray &xmlData)
{
    QList<DrugInfo> drugs;
    QXmlStreamReader xml(xmlData);

    DrugInfo currentDrug;

    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNext();

        if (xml.isStartElement()) {
            if (xml.name() == "drug") {
                currentDrug = DrugInfo();
            } else if (xml.name() == "id") {
                currentDrug.id = xml.readElementText().toInt();
            } else if (xml.name() == "name") {
                currentDrug.name = xml.readElementText();
            } else if (xml.name() == "category") {
                currentDrug.category = xml.readElementText();
            } else if (xml.name() == "manufacturer") {
                currentDrug.manufacturer = xml.readElementText();
            } else if (xml.name() == "price") {
                currentDrug.unitPrice = xml.readElementText().toDouble();
            } else if (xml.name() == "description") {
                currentDrug.description = xml.readElementText();
            }
        } else if (xml.isEndElement() && xml.name() == "drug") {
            drugs.append(currentDrug);
        }
    }

    return drugs;
}

QList<DiagnosisStandard> NetSync::parseDiagnosisJson(const QByteArray &jsonData)
{
    QList<DiagnosisStandard> diagnoses;

    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &jsonError);

    if (jsonError.error != QJsonParseError::NoError) {
        qDebug() << "JSON解析错误：" << jsonError.errorString();
        return diagnoses;
    }

    if (!doc.isArray()) {
        qDebug() << "JSON格式错误，期望数组格式";
        return diagnoses;
    }

    QJsonArray diagArray = doc.array();
    for (int i = 0; i < diagArray.size(); ++i) {
        QJsonObject diagObj = diagArray[i].toObject();
        DiagnosisStandard diag;
        diag.id = diagObj["id"].toInt();
        diag.code = diagObj["code"].toString();
        diag.name = diagObj["name"].toString();
        diag.category = diagObj["category"].toString();
        diag.description = diagObj["description"].toString();
        diag.treatmentPlan = diagObj["treatment_plan"].toString();
        diagnoses.append(diag);
    }

    return diagnoses;
}

void NetSync::saveDrugsToDatabase(const QList<DrugInfo> &drugs)
{
    // 创建药品同步表（用于存储同步的数据）
    QSqlQuery query;

    // 创建同步药品表（如果不存在）
    query.exec("CREATE TABLE IF NOT EXISTS sync_drugs ("
               "id INTEGER PRIMARY KEY, "
               "name VARCHAR(50), "
               "category VARCHAR(20), "
               "manufacturer VARCHAR(50), "
               "unit_price DECIMAL(10,2), "
               "description TEXT, "
               "synced_at DATETIME DEFAULT CURRENT_TIMESTAMP)");

    // 批量插入或更新
    for (const DrugInfo &drug : drugs) {
        query.prepare("INSERT OR REPLACE INTO sync_drugs "
                      "(id, name, category, manufacturer, unit_price, description) "
                      "VALUES (:id, :name, :category, :manufacturer, :price, :desc)");
        query.bindValue(":id", drug.id);
        query.bindValue(":name", drug.name);
        query.bindValue(":category", drug.category);
        query.bindValue(":manufacturer", drug.manufacturer);
        query.bindValue(":price", drug.unitPrice);
        query.bindValue(":desc", drug.description);
        query.exec();
    }

    qDebug() << "药品数据已保存到同步表";
}

void NetSync::saveDiagnosesToDatabase(const QList<DiagnosisStandard> &diagnoses)
{
    QSqlQuery query;

    // 创建诊断标准同步表
    query.exec("CREATE TABLE IF NOT EXISTS sync_diagnoses ("
               "id INTEGER PRIMARY KEY, "
               "code VARCHAR(20), "
               "name VARCHAR(100), "
               "category VARCHAR(50), "
               "description TEXT, "
               "treatment_plan TEXT, "
               "synced_at DATETIME DEFAULT CURRENT_TIMESTAMP)");

    // 批量插入或更新
    for (const DiagnosisStandard &diag : diagnoses) {
        query.prepare("INSERT OR REPLACE INTO sync_diagnoses "
                      "(id, code, name, category, description, treatment_plan) "
                      "VALUES (:id, :code, :name, :category, :desc, :plan)");
        query.bindValue(":id", diag.id);
        query.bindValue(":code", diag.code);
        query.bindValue(":name", diag.name);
        query.bindValue(":category", diag.category);
        query.bindValue(":desc", diag.description);
        query.bindValue(":plan", diag.treatmentPlan);
        query.exec();
    }

    qDebug() << "诊断标准数据已保存到同步表";
}

void NetSync::loadFromDatabase()
{
    // 从数据库加载同步的药品数据
    QSqlQuery query;

    query.exec("SELECT id, name, category, manufacturer, unit_price, description FROM sync_drugs");
    while (query.next()) {
        DrugInfo drug;
        drug.id = query.value(0).toInt();
        drug.name = query.value(1).toString();
        drug.category = query.value(2).toString();
        drug.manufacturer = query.value(3).toString();
        drug.unitPrice = query.value(4).toDouble();
        drug.description = query.value(5).toString();
        localDrugs.append(drug);
    }

    // 从数据库加载诊断标准数据
    query.exec("SELECT id, code, name, category, description, treatment_plan FROM sync_diagnoses");
    while (query.next()) {
        DiagnosisStandard diag;
        diag.id = query.value(0).toInt();
        diag.code = query.value(1).toString();
        diag.name = query.value(2).toString();
        diag.category = query.value(3).toString();
        diag.description = query.value(4).toString();
        diag.treatmentPlan = query.value(5).toString();
        localDiagnoses.append(diag);
    }

    qDebug() << "已从数据库加载" << localDrugs.size() << "条药品数据";
    qDebug() << "已从数据库加载" << localDiagnoses.size() << "条诊断标准";
}

QList<DrugInfo> NetSync::getLocalDrugs() const
{
    return localDrugs;
}

QList<DiagnosisStandard> NetSync::getLocalDiagnoses() const
{
    return localDiagnoses;
}
