#include "departmenteditview.h"
#include "ui_departmenteditview.h"
#include "idatabase.h"
#include <QSqlTableModel>

DepartmentEditView::DepartmentEditView(QWidget *parent, int index)
    : QWidget(parent)
    , ui(new Ui::DepartmentEditView)
{
    ui->setupUi(this);

    // 初始化数据映射器
    dataMapper = new QDataWidgetMapper();
    QSqlTableModel *tabModel = IDatabase::getInstance().departmentTabModel;
    dataMapper->setModel(IDatabase::getInstance().departmentTabModel);
    dataMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

    // 绑定界面控件与数据字段（根据实际数据库结构Department表：ID, NAME）
    dataMapper->addMapping(ui->dbEditID, tabModel->fieldIndex("ID"));
    dataMapper->addMapping(ui->dbEditName, tabModel->fieldIndex("NAME"));

    dataMapper->setCurrentIndex(index);

    // ID字段可自由编辑
}

DepartmentEditView::~DepartmentEditView()
{
    delete ui;
}

void DepartmentEditView::on_btSave_clicked()
{
    IDatabase::getInstance().submitDepartmentEdit();
    emit goPreviousView();
}

void DepartmentEditView::on_btCancel_clicked()
{
    IDatabase::getInstance().revertDepartmentEdit();
    emit goPreviousView();
}
//123
