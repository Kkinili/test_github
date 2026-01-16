#include "appointmenteditview.h"
#include "ui_appointmenteditview.h"
#include "idatabase.h"
#include <QSqlTableModel>
#include <QDebug>
#include <QSqlQuery>

AppointmentEditView::AppointmentEditView(QWidget *parent, int row)
    : QWidget(parent)
    , ui(new Ui::AppointmentEditView)
    , currentRow(row)
{
    ui->setupUi(this);

    QSqlTableModel *tabModel = IDatabase::getInstance().appointmentTabModel;

    // 安全检查
    if (!tabModel) {
        qDebug() << "错误：appointmentTabModel 为空！";
        return;
    }

    // 加载患者列表到combobox
    QSqlTableModel *patientModel = IDatabase::getInstance().patientTabModel;
    if (patientModel) {
        ui->dbComboPatient->setModel(patientModel);
        // 设置显示名称列（NAME），值仍然是ID
        ui->dbComboPatient->setModelColumn(patientModel->fieldIndex("NAME"));
    }

    // 加载医生列表到combobox
    QSqlTableModel *doctorModel = IDatabase::getInstance().doctorTabModel;
    if (doctorModel) {
        ui->dbComboDoctor->setModel(doctorModel);
        // 设置显示名称列（NAME），值仍然是ID
        ui->dbComboDoctor->setModelColumn(doctorModel->fieldIndex("NAME"));
    }

    // 判断是新增记录还是编辑现有记录
    isNewRecord = (currentRow < 0 || currentRow >= tabModel->rowCount());

    if (isNewRecord) {
        // 新增记录：初始化为空值
        ui->dbComboPatient->setCurrentIndex(-1);
        ui->dbComboDoctor->setCurrentIndex(-1);
        ui->dbDateTimeEditAppointment->setDateTime(QDateTime::currentDateTime());
        ui->dbComboStatus->setCurrentIndex(0);  // 默认"待就诊"
        ui->dbTextEditNotes->clear();
    } else {
        // 编辑现有记录：加载数据
        // 设置患者
        int patientId = tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("PATIENT_ID"))).toInt();
        // 在patientModel中查找对应的行
        for (int i = 0; i < patientModel->rowCount(); ++i) {
            if (patientModel->data(patientModel->index(i, patientModel->fieldIndex("ID"))).toInt() == patientId) {
                ui->dbComboPatient->setCurrentIndex(i);
                break;
            }
        }

        // 设置医生
        int doctorId = tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("DOCTOR_ID"))).toInt();
        // 在doctorModel中查找对应的行
        for (int i = 0; i < doctorModel->rowCount(); ++i) {
            if (doctorModel->data(doctorModel->index(i, doctorModel->fieldIndex("ID"))).toInt() == doctorId) {
                ui->dbComboDoctor->setCurrentIndex(i);
                break;
            }
        }

        // 设置预约时间
        QDateTime appointTime = tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("APPOINT_DATE"))).toDateTime();
        if (appointTime.isValid()) {
            ui->dbDateTimeEditAppointment->setDateTime(appointTime);
        }

        // 设置状态
        QString status = tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("STATUS"))).toString();
        int statusIdx = ui->dbComboStatus->findText(status);
        if (statusIdx >= 0) {
            ui->dbComboStatus->setCurrentIndex(statusIdx);
        }

        // 设置备注
        ui->dbTextEditNotes->setText(tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("NOTES"))).toString());
    }
}

AppointmentEditView::~AppointmentEditView()
{
    delete ui;
}

void AppointmentEditView::on_btSave_clicked()
{
    QSqlTableModel *tabModel = IDatabase::getInstance().appointmentTabModel;

    // 安全检查
    if (!tabModel) {
        qDebug() << "错误：appointmentTabModel 为空！";
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

    // 保存所有字段
    // 获取选中的患者ID（从模型中取ID列的值）
    int patientIndex = ui->dbComboPatient->currentIndex();
    int patientId = -1;
    if (patientIndex >= 0) {
        QSqlTableModel *patientModel = IDatabase::getInstance().patientTabModel;
        patientId = patientModel->data(patientModel->index(patientIndex, patientModel->fieldIndex("ID"))).toInt();
    }

    // 获取选中的医生ID（从模型中取ID列的值）
    int doctorIndex = ui->dbComboDoctor->currentIndex();
    int doctorId = -1;
    if (doctorIndex >= 0) {
        QSqlTableModel *doctorModel = IDatabase::getInstance().doctorTabModel;
        doctorId = doctorModel->data(doctorModel->index(doctorIndex, doctorModel->fieldIndex("ID"))).toInt();
    }

    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("PATIENT_ID")), patientId);
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("DOCTOR_ID")), doctorId);
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("APPOINT_DATE")),
                      ui->dbDateTimeEditAppointment->dateTime());
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("STATUS")),
                      ui->dbComboStatus->currentText());
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("NOTES")),
                      ui->dbTextEditNotes->toPlainText());

    if (IDatabase::getInstance().submitAppointmentEdit()) {
        qDebug() << "预约保存成功！";
    } else {
        qDebug() << "预约保存失败！";
    }

    emit goPreviousView();
}

void AppointmentEditView::on_btCancel_clicked()
{
    if (isNewRecord) {
        // 新增记录的取消：删除新插入的行
        QSqlTableModel *tabModel = IDatabase::getInstance().appointmentTabModel;
        if (tabModel && currentRow >= 0 && currentRow < tabModel->rowCount()) {
            tabModel->removeRow(currentRow);
            tabModel->submitAll();
            qDebug() << "已删除新插入的预约记录";
        }
    } else {
        // 编辑现有记录的取消：回滚更改
        IDatabase::getInstance().revertAppointmentEdit();
    }
    emit goPreviousView();
}
