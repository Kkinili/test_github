#include "doctoreditview.h"
#include "ui_doctoreditview.h"
#include "idatabase.h"
#include <QSqlTableModel>
#include <QSqlQuery>

DoctorEditView::DoctorEditView(QWidget *parent, int index)
    : QWidget(parent)
    , ui(new Ui::DoctorEditView)
{
    ui->setupUi(this);

    // 初始化数据映射器
    dataMapper = new QDataWidgetMapper();
    QSqlTableModel *tabModel = IDatabase::getInstance().doctorTabModel;
    dataMapper->setModel(IDatabase::getInstance().doctorTabModel);
    dataMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

    // 绑定界面控件与数据字段（根据实际数据库结构Doctor表：ID, EMPLOYEENO, NAME, DEPARTMENT_ID）
    dataMapper->addMapping(ui->dbEditID, tabModel->fieldIndex("ID"));
    dataMapper->addMapping(ui->dbEditName, tabModel->fieldIndex("NAME"));
    dataMapper->addMapping(ui->dbEditEmployeeNo, tabModel->fieldIndex("EMPLOYEENO"));
    // DEPARTMENT_ID 需要特殊处理，因为使用下拉框

    // 加载科室列表到下拉框
    QSqlTableModel *deptModel = IDatabase::getInstance().departmentTabModel;
    if (deptModel) {
        ui->dbComboDepartment->setModel(deptModel);
        ui->dbComboDepartment->setModelColumn(deptModel->fieldIndex("NAME"));
        ui->dbComboDepartment->setInsertPolicy(QComboBox::NoInsert);
    }

    // 加载完成后设置当前索引
    dataMapper->setCurrentIndex(index);

    // 手动设置科室下拉框的当前值
    if (index >= 0 && index < tabModel->rowCount()) {
        int deptId = tabModel->data(tabModel->index(index, tabModel->fieldIndex("DEPARTMENT_ID"))).toInt();
        // 在departmentModel中查找对应的科室
        if (deptModel) {
            for (int i = 0; i < deptModel->rowCount(); ++i) {
                if (deptModel->data(deptModel->index(i, deptModel->fieldIndex("ID"))).toInt() == deptId) {
                    ui->dbComboDepartment->setCurrentIndex(i);
                    break;
                }
            }
        }
    }
}

DoctorEditView::~DoctorEditView()
{
    delete ui;
}

void DoctorEditView::on_btSave_clicked()
{
    // 获取选中的科室ID（从模型中取ID列的值）
    int deptIndex = ui->dbComboDepartment->currentIndex();
    int departmentId = -1;
    if (deptIndex >= 0) {
        QSqlTableModel *deptModel = IDatabase::getInstance().departmentTabModel;
        departmentId = deptModel->data(deptModel->index(deptIndex, deptModel->fieldIndex("ID"))).toInt();
    }

    // 设置DEPARTMENT_ID到模型
    QSqlTableModel *tabModel = IDatabase::getInstance().doctorTabModel;
    int currentIndex = dataMapper->currentIndex();
    if (currentIndex >= 0 && currentIndex < tabModel->rowCount()) {
        tabModel->setData(tabModel->index(currentIndex, tabModel->fieldIndex("DEPARTMENT_ID")), departmentId);
    }

    IDatabase::getInstance().submitDoctorEdit();
    emit goPreviousView();
}

void DoctorEditView::on_btCancel_clicked()
{
    IDatabase::getInstance().revertDoctorEdit();
    emit goPreviousView();
}
