#include "recordeditview.h"
#include "ui_recordeditview.h"
#include "idatabase.h"
#include <QSqlTableModel>
#include <QDebug>

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

    // 判断是新增记录还是编辑现有记录
    isNewRecord = (currentRow < 0 || currentRow >= tabModel->rowCount());

    qDebug() << "isNewRecord =" << isNewRecord;

    if (isNewRecord) {
        // 新增记录：初始化为空值
        ui->dbEditID->clear();
        ui->dbEditPatientId->clear();
        ui->dbEditDoctorId->clear();
        ui->dbDateTimeEditVisit->setDateTime(QDateTime::currentDateTime());
        ui->dbTextEditComplaint->clear();
        ui->dbTextEditDiagnosis->clear();
        ui->dbTextEditPrescription->clear();
        ui->dbComboStatus->setCurrentIndex(0);  // 默认"进行中"

        qDebug() << "新增记录模式初始化完成";
    } else {
        // 编辑现有记录：加载数据
        ui->dbEditID->setText(tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("ID"))).toString());
        ui->dbEditPatientId->setText(tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("PATIENT_ID"))).toString());
        ui->dbEditDoctorId->setText(tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("DOCTOR_ID"))).toString());
        ui->dbDateTimeEditVisit->setDateTime(tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("VISIT_DATE"))).toDateTime());

        // 加载纯文本内容（使用setText替代setHtml）
        ui->dbTextEditComplaint->setText(tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("CHIEF_COMPLAINT"))).toString());
        ui->dbTextEditDiagnosis->setText(tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("DIAGNOSIS"))).toString());
        ui->dbTextEditPrescription->setText(tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("PRESCRIPTION"))).toString());

        // 设置STATUS
        int status = tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("STATUS"))).toInt();
        int statusIndex = ui->dbComboStatus->findData(status);
        if (statusIndex >= 0) {
            ui->dbComboStatus->setCurrentIndex(statusIndex);
        }

        qDebug() << "编辑现有记录模式初始化完成，记录ID：" << ui->dbEditID->text();
    }
}

RecordEditView::~RecordEditView()
{
    delete ui;
}

void RecordEditView::on_btSave_clicked()
{
    QSqlTableModel *tabModel = IDatabase::getInstance().recordTabModel;

    // 安全检查
    if (!tabModel) {
        qDebug() << "错误：recordTabModel 为空！";
        return;
    }

    // 如果是新增记录，先确保有有效的行
    int targetRow = currentRow;
    if (isNewRecord) {
        // 新增记录时，currentRow 应该指向新插入的行
        // 检查当前模型是否已经有新行
        if (currentRow < 0 || currentRow >= tabModel->rowCount()) {
            // 尝试使用最后一行
            targetRow = tabModel->rowCount() - 1;
        } else {
            targetRow = currentRow;
        }

        // 如果仍然无效，无法保存
        if (targetRow < 0 || targetRow >= tabModel->rowCount()) {
            qDebug() << "错误：无法确定要保存的行！";
            return;
        }
    }

    qDebug() << "保存记录：targetRow =" << targetRow << "isNewRecord =" << isNewRecord;

    // 保存所有字段到正确的行（使用toPlainText保存纯文本）
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("ID")),
                      ui->dbEditID->text());
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("PATIENT_ID")),
                      ui->dbEditPatientId->text());
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("DOCTOR_ID")),
                      ui->dbEditDoctorId->text());
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("VISIT_DATE")),
                      ui->dbDateTimeEditVisit->dateTime());
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("CHIEF_COMPLAINT")),
                      ui->dbTextEditComplaint->toPlainText());
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("DIAGNOSIS")),
                      ui->dbTextEditDiagnosis->toPlainText());
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("PRESCRIPTION")),
                      ui->dbTextEditPrescription->toPlainText());
    // 保存STATUS字段
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("STATUS")),
                      ui->dbComboStatus->currentData().toInt());

    qDebug() << "开始提交数据...";

    // 直接调用submitAll()而不是通过IDatabase
    bool success = tabModel->submitAll();

    if (success) {
        qDebug() << "记录保存成功！";
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
