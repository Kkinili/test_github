#include "drugeditview.h"
#include "ui_drugeditview.h"
#include "idatabase.h"
#include <QSqlTableModel>
#include <QDebug>

DrugEditView::DrugEditView(QWidget *parent, int row)
    : QWidget(parent)
    , ui(new Ui::DrugEditView)
    , currentRow(row)
{
    ui->setupUi(this);

    QSqlTableModel *tabModel = IDatabase::getInstance().drugTabModel;

    // 安全检查
    if (!tabModel) {
        qDebug() << "错误：drugTabModel 为空！";
        return;
    }

    // 判断是新增记录还是编辑现有记录
    isNewRecord = (currentRow < 0 || currentRow >= tabModel->rowCount());

    if (isNewRecord) {
        // 新增记录：初始化为空值
        ui->dbEditName->clear();
        ui->dbEditCategory->clear();
        ui->dbSpinStock->setValue(0);
        ui->dbSpinPrice->setValue(0.0);
        ui->dbEditManufacturer->clear();
        ui->dbDateEditExpiry->setDate(QDate::currentDate());
    } else {
        // 编辑现有记录：加载数据
        ui->dbEditName->setText(tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("NAME"))).toString());
        ui->dbEditCategory->setText(tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("CATEGORY"))).toString());
        ui->dbSpinStock->setValue(tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("STOCK_QTY"))).toInt());
        ui->dbSpinPrice->setValue(tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("UNIT_PRICE"))).toDouble());
        ui->dbEditManufacturer->setText(tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("MANUFACTURER"))).toString());

        QDate expiryDate = tabModel->data(tabModel->index(currentRow, tabModel->fieldIndex("EXPIRY_DATE"))).toDate();
        if (expiryDate.isValid()) {
            ui->dbDateEditExpiry->setDate(expiryDate);
        }
    }
}

DrugEditView::~DrugEditView()
{
    delete ui;
}

void DrugEditView::on_btSave_clicked()
{
    QSqlTableModel *tabModel = IDatabase::getInstance().drugTabModel;

    // 安全检查
    if (!tabModel) {
        qDebug() << "错误：drugTabModel 为空！";
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
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("NAME")),
                      ui->dbEditName->text());
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("CATEGORY")),
                      ui->dbEditCategory->text());
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("STOCK_QTY")),
                      ui->dbSpinStock->value());
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("UNIT_PRICE")),
                      ui->dbSpinPrice->value());
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("MANUFACTURER")),
                      ui->dbEditManufacturer->text());
    tabModel->setData(tabModel->index(targetRow, tabModel->fieldIndex("EXPIRY_DATE")),
                      ui->dbDateEditExpiry->date());

    if (IDatabase::getInstance().submitDrugEdit()) {
        qDebug() << "药品保存成功！";
    } else {
        qDebug() << "药品保存失败！";
    }

    emit goPreviousView();
}

void DrugEditView::on_btCancel_clicked()
{
    if (isNewRecord) {
        // 新增记录的取消：删除新插入的行
        QSqlTableModel *tabModel = IDatabase::getInstance().drugTabModel;
        if (tabModel && currentRow >= 0 && currentRow < tabModel->rowCount()) {
            tabModel->removeRow(currentRow);
            tabModel->submitAll();
            qDebug() << "已删除新插入的药品记录";
        }
    } else {
        // 编辑现有记录的取消：回滚更改
        IDatabase::getInstance().revertDrugEdit();
    }
    emit goPreviousView();
}
