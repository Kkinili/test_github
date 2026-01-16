#include "recordeditview.h"
#include "ui_recordeditview.h"
#include "idatabase.h"
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QDebug>
#include <QTableWidgetItem>
#include <QMessageBox>

RecordEditView::RecordEditView(QWidget *parent, int row)
    : QWidget(parent)
    , ui(new Ui::RecordEditView)
    , currentRow(row)
    , isNewRecord(false)
{
    ui->setupUi(this);

    QSqlTableModel *tabModel = IDatabase::getInstance().recordTabModel;

    // 安全检查
    if (!tabModel) {
        qDebug() << "错误：recordTabModel 为空！";
        return;
    }

    qDebug() << "RecordEditView 初始化：currentRow =" << currentRow << "rowCount =" << tabModel->rowCount();

    // 设置状态combobox的显示文本
    ui->dbComboStatus->addItem("进行中", 0);
    ui->dbComboStatus->addItem("已完成", 1);
    ui->dbComboStatus->addItem("已取消", 2);

    // 设置药品数量SpinBox的范围
    ui->dbSpinQuantity->setRange(1, 9999);
    ui->dbSpinQuantity->setValue(1);

    // 初始化药品处方列表
    ui->dbTablePrescription->horizontalHeader()->setStretchLastSection(true);
    prescriptionItems.clear();

    // 加载患者列表到combobox
    QSqlTableModel *patientModel = IDatabase::getInstance().patientTabModel;
    if (patientModel) {
        ui->dbComboPatient->setModel(patientModel);
        ui->dbComboPatient->setModelColumn(patientModel->fieldIndex("NAME"));
        ui->dbComboPatient->setInsertPolicy(QComboBox::NoInsert);
    }

    // 加载医生列表到combobox
    QSqlTableModel *doctorModel = IDatabase::getInstance().doctorTabModel;
    if (doctorModel) {
        ui->dbComboDoctor->setModel(doctorModel);
        ui->dbComboDoctor->setModelColumn(doctorModel->fieldIndex("NAME"));
        ui->dbComboDoctor->setInsertPolicy(QComboBox::NoInsert);
    }

    // 加载药品列表到combobox
    QSqlTableModel *drugModel = IDatabase::getInstance().drugTabModel;
    if (drugModel) {
        ui->dbComboDrug->setModel(drugModel);
        ui->dbComboDrug->setModelColumn(drugModel->fieldIndex("NAME"));
        ui->dbComboDrug->setInsertPolicy(QComboBox::NoInsert);
    }

    // 判断是新增记录还是编辑现有记录
    isNewRecord = (currentRow < 0 || currentRow >= tabModel->rowCount());

    qDebug() << "isNewRecord =" << isNewRecord;

    if (isNewRecord) {
        // 新增记录：初始化为空值
        ui->dbEditID->clear();
        ui->dbComboPatient->setCurrentIndex(-1);
        ui->dbComboDoctor->setCurrentIndex(-1);
        ui->dbDateTimeEditVisit->setDateTime(QDateTime::currentDateTime());
        ui->dbTextEditComplaint->clear();
        ui->dbTextEditDiagnosis->clear();
        ui->dbComboStatus->setCurrentIndex(0);  // 默认"进行中"

        qDebug() << "新增记录模式初始化完成";
    } else {
        // 编辑现有记录：加载数据
        ui->dbEditID->setText(tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("ID"))).toString());

        // 设置患者
        int patientId = tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("PATIENT_ID"))).toInt();
        if (patientModel) {
            for (int i = 0; i < patientModel->rowCount(); ++i) {
                if (patientModel->data(patientModel->index(i, patientModel->fieldIndex("ID"))).toInt() == patientId) {
                    ui->dbComboPatient->setCurrentIndex(i);
                    break;
                }
            }
        }

        // 设置医生
        int doctorId = tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("DOCTOR_ID"))).toInt();
        if (doctorModel) {
            for (int i = 0; i < doctorModel->rowCount(); ++i) {
                if (doctorModel->data(doctorModel->index(i, doctorModel->fieldIndex("ID"))).toInt() == doctorId) {
                    ui->dbComboDoctor->setCurrentIndex(i);
                    break;
                }
            }
        }

        ui->dbDateTimeEditVisit->setDateTime(tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("VISIT_DATE"))).toDateTime());

        // 加载纯文本内容
        ui->dbTextEditComplaint->setText(tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("CHIEF_COMPLAINT"))).toString());
        ui->dbTextEditDiagnosis->setText(tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("DIAGNOSIS"))).toString());

        // 设置STATUS
        int status = tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("STATUS"))).toInt();
        int statusIndex = ui->dbComboStatus->findData(status);
        if (statusIndex >= 0) {
            ui->dbComboStatus->setCurrentIndex(statusIndex);
        }

        // 加载处方信息（如果有）
        loadPrescriptionFromRecord(currentRow);

        qDebug() << "编辑现有记录模式初始化完成，记录ID：" << ui->dbEditID->text();
    }
}

RecordEditView::~RecordEditView()
{
    delete ui;
}

void RecordEditView::loadPrescriptionFromRecord(int recordRow)
{
    // 从数据库加载该就诊记录的处方信息
    QSqlTableModel *tabModel = IDatabase::getInstance().recordTabModel;
    QString recordId = tabModel->data(tabModel->index(recordRow, tabModel->fieldIndex("ID"))).toString();

    QSqlQuery query;
    query.prepare("SELECT drug_name, quantity, unit_price FROM prescriptions WHERE record_id = :recordId");
    query.bindValue(":recordId", recordId);

    if (query.exec()) {
        while (query.next()) {
            QString drugName = query.value("drug_name").toString();
            int quantity = query.value("quantity").toInt();
            double unitPrice = query.value("unit_price").toDouble();

            // 添加到处方列表
            addPrescriptionItem(drugName, quantity, unitPrice);
        }
    }
}

void RecordEditView::addPrescriptionItem(QString drugName, int quantity, double unitPrice)
{
    // 检查是否已存在该药品
    for (int i = 0; i < prescriptionItems.count(); ++i) {
        if (prescriptionItems[i].drugName == drugName) {
            // 已存在，增加数量
            prescriptionItems[i].quantity += quantity;
            updatePrescriptionTable();
            return;
        }
    }

    // 添加新药品
    PrescriptionItem item;
    item.drugName = drugName;
    item.quantity = quantity;
    item.unitPrice = unitPrice;
    prescriptionItems.append(item);

    updatePrescriptionTable();
}

void RecordEditView::updatePrescriptionTable()
{
    ui->dbTablePrescription->setRowCount(prescriptionItems.count());
    double total = 0.0;

    for (int i = 0; i < prescriptionItems.count(); ++i) {
        // 药品名称
        ui->dbTablePrescription->setItem(i, 0, new QTableWidgetItem(prescriptionItems[i].drugName));
        // 数量
        ui->dbTablePrescription->setItem(i, 1, new QTableWidgetItem(QString::number(prescriptionItems[i].quantity)));
        // 单价
        ui->dbTablePrescription->setItem(i, 2, new QTableWidgetItem(QString::number(prescriptionItems[i].unitPrice, 'f', 2)));
        // 金额小计
        double subtotal = prescriptionItems[i].quantity * prescriptionItems[i].unitPrice;
        total += subtotal;

        // 操作按钮
        QWidget *widget = new QWidget();
        QPushButton *btnRemove = new QPushButton("删除");
        connect(btnRemove, &QPushButton::clicked, this, [this, i]() {
            prescriptionItems.removeAt(i);
            updatePrescriptionTable();
        });

        QHBoxLayout *layout = new QHBoxLayout(widget);
        layout->addWidget(btnRemove);
        layout->setContentsMargins(0, 0, 0, 0);
        widget->setLayout(layout);
        ui->dbTablePrescription->setCellWidget(i, 3, widget);
    }

    // 更新总金额显示
    ui->labelTotal->setText(QString("总金额: ¥%1").arg(total, 0, 'f', 2));
}

void RecordEditView::on_btAddDrug_clicked()
{
    int drugIndex = ui->dbComboDrug->currentIndex();
    if (drugIndex < 0) {
        QMessageBox::warning(this, "警告", "请先选择一个药品！");
        return;
    }

    int quantity = ui->dbSpinQuantity->value();
    if (quantity <= 0) {
        QMessageBox::warning(this, "警告", "请输入有效的药品数量！");
        return;
    }

    // 获取药品信息
    QSqlTableModel *drugModel = IDatabase::getInstance().drugTabModel;
    QString drugName = drugModel->data(drugModel->index(drugIndex, drugModel->fieldIndex("NAME"))).toString();
    double unitPrice = drugModel->data(drugModel->index(drugIndex, drugModel->fieldIndex("UNIT_PRICE"))).toDouble();
    int currentStock = drugModel->data(drugModel->index(drugIndex, drugModel->fieldIndex("STOCK_QTY"))).toInt();

    // 检查库存
    // 先检查处方中是否已添加过该药品
    int existingQty = 0;
    for (const auto &item : prescriptionItems) {
        if (item.drugName == drugName) {
            existingQty = item.quantity;
            break;
        }
    }

    if (existingQty + quantity > currentStock) {
        QMessageBox::warning(this, "库存不足",
                             QString("药品【%1】当前库存: %2\n处方中已添加: %3\n本次添加: %4")
                                 .arg(drugName).arg(currentStock).arg(existingQty).arg(quantity));
        return;
    }

    // 添加到处方
    addPrescriptionItem(drugName, quantity, unitPrice);

    // 重置数量选择
    ui->dbSpinQuantity->setValue(1);
}

void RecordEditView::on_btSave_clicked()
{
    QSqlTableModel *tabModel = IDatabase::getInstance().recordTabModel;

    // 安全检查
    if (!tabModel) {
        qDebug() << "错误：recordTabModel 为空！";
        return;
    }

    // 验证必填字段
    if (ui->dbComboPatient->currentIndex() < 0) {
        QMessageBox::warning(this, "警告", "请选择患者！");
        return;
    }
    if (ui->dbComboDoctor->currentIndex() < 0) {
        QMessageBox::warning(this, "警告", "请选择医生！");
        return;
    }

    // 如果是新增记录，先确保有有效的行
    int targetRow = currentRow;
    if (isNewRecord) {
        if (currentRow < 0 || currentRow >= tabModel->rowCount()) {
            targetRow = tabModel->rowCount() - 1;
        } else {
            targetRow = currentRow;
        }

        if (targetRow < 0 || targetRow >= tabModel->rowCount()) {
            qDebug() << "错误：无法确定要保存的行！";
            return;
        }
    }

    qDebug() << "保存记录：targetRow =" << targetRow << "isNewRecord =" << isNewRecord;

    // 保存所有字段到正确的行
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("ID")),
                      ui->dbEditID->text());

    // 获取选中的患者ID（从模型中取ID列的值）
    int patientIndex = ui->dbComboPatient->currentIndex();
    int patientId = -1;
    if (patientIndex >= 0) {
        QSqlTableModel *patientModel = IDatabase::getInstance().patientTabModel;
        patientId = patientModel->data(patientModel->index(patientIndex, patientModel->fieldIndex("ID"))).toInt();
    }
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("PATIENT_ID")), patientId);

    // 获取选中的医生ID（从模型中取ID列的值）
    int doctorIndex = ui->dbComboDoctor->currentIndex();
    int doctorId = -1;
    if (doctorIndex >= 0) {
        QSqlTableModel *doctorModel = IDatabase::getInstance().doctorTabModel;
        doctorId = doctorModel->data(doctorModel->index(doctorIndex, doctorModel->fieldIndex("ID"))).toInt();
    }
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("DOCTOR_ID")), doctorId);

    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("VISIT_DATE")),
                      ui->dbDateTimeEditVisit->dateTime());
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("CHIEF_COMPLAINT")),
                      ui->dbTextEditComplaint->toPlainText());
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("DIAGNOSIS")),
                      ui->dbTextEditDiagnosis->toPlainText());

    // 生成处方文本
    QString prescriptionText;
    for (const auto &item : prescriptionItems) {
        prescriptionText += QString("%1 x %2\n").arg(item.drugName).arg(item.quantity);
    }
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("PRESCRIPTION")), prescriptionText);

    // 保存STATUS字段
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("STATUS")),
                      ui->dbComboStatus->currentData().toInt());

    qDebug() << "开始提交数据...";

    // 先提交就诊记录
    bool success = tabModel->submitAll();

    if (success) {
        qDebug() << "记录保存成功！";

        // 保存处方到数据库并扣减库存
        QString recordId = ui->dbEditID->text();
        if (recordId.isEmpty()) {
            // 如果是新增记录，获取自动生成的ID
            recordId = tabModel->data(tabModel->index(targetRow, tabModel->fieldIndex("ID"))).toString();
        }

        // 删除旧的处方记录
        QSqlQuery query;
        query.prepare("DELETE FROM prescriptions WHERE record_id = :recordId");
        query.bindValue(":recordId", recordId);
        query.exec();

        // 保存新的处方并扣减库存
        QSqlTableModel *drugModel = IDatabase::getInstance().drugTabModel;
        for (const auto &item : prescriptionItems) {
            // 保存处方详情
            query.prepare("INSERT INTO prescriptions (record_id, drug_name, quantity, unit_price) "
                          "VALUES (:recordId, :drugName, :quantity, :unitPrice)");
            query.bindValue(":recordId", recordId);
            query.bindValue(":drugName", item.drugName);
            query.bindValue(":quantity", item.quantity);
            query.bindValue(":unitPrice", item.unitPrice);
            query.exec();

            // 扣减库存
            query.prepare("UPDATE drug_inventory SET stock_qty = stock_qty - :quantity "
                          "WHERE name = :drugName AND stock_qty >= :quantity");
            query.bindValue(":quantity", item.quantity);
            query.bindValue(":drugName", item.drugName);
            query.exec();

            // 更新药品模型
            drugModel->select();
        }

        qDebug() << "处方保存成功，库存已扣减";
    } else {
        qDebug() << "记录保存失败！错误：" << tabModel->lastError().text();
    }

    emit goPreviousView();
}

void RecordEditView::on_btCancel_clicked()
{
    if (isNewRecord) {
        // 新增记录的取消：删除新插入的行
        QSqlTableModel *tabModel = IDatabase::getInstance().recordTabModel;
        if (tabModel && currentRow >= 0 && currentRow < tabModel->rowCount()) {
            tabModel->removeRow(currentRow);
            tabModel->submitAll();
            qDebug() << "已删除新插入的记录";
        }
    } else {
        // 编辑现有记录的取消：回滚更改
        IDatabase::getInstance().revertRecordEdit();
    }
    emit goPreviousView();
}
