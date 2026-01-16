#include "appointmentview.h"
#include "ui_appointmentview.h"
#include "idatabase.h"
#include <QSqlTableModel>
#include <QItemSelectionModel>
#include <QDebug>

AppointmentView::AppointmentView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppointmentView)
{
    ui->setupUi(this);

    // 设置表格模型
    ui->tableView->setModel(IDatabase::getInstance().appointmentTabModel);

    // 设置选择模式为单行选择
    ui->tableView->setSelectionMode(QTableView::SingleSelection);
    ui->tableView->setSelectionBehavior(QTableView::SelectRows);

    // 设置选择模型
    ui->tableView->setSelectionModel(IDatabase::getInstance().theAppointmentSelection);

    // 连接选择模型的点击信号
    connect(ui->tableView, &QTableView::clicked,
            this, &AppointmentView::on_tableView_clicked);

    // 连接双击信号
    connect(ui->tableView, &QTableView::doubleClicked,
            this, &AppointmentView::on_tableView_doubleClicked);
}

AppointmentView::~AppointmentView()
{
    delete ui;
}

void AppointmentView::on_btAdd_clicked()
{
    int index = IDatabase::getInstance().addNewAppointment();
    emit goAppointmentEditView(index);
}

void AppointmentView::on_btDelete_clicked()
{
    IDatabase::getInstance().deleteCurrentAppointment();
}

void AppointmentView::on_btSearch_clicked()
{
    QString filter = QString("patient_id like '%%1%'").arg(ui->dbEditSearch->text());
    IDatabase::getInstance().searchAppointment(filter);
}

void AppointmentView::on_btEdit_clicked()
{
    // 获取当前选中的行
    QModelIndex curIndex = IDatabase::getInstance().theAppointmentSelection->currentIndex();
    if (curIndex.isValid()) {
        emit goAppointmentEditView(curIndex.row());
    }
}

void AppointmentView::on_tableView_clicked(const QModelIndex &index)
{
    // 单击选择记录
    Q_UNUSED(index);
}

void AppointmentView::on_tableView_doubleClicked(const QModelIndex &index)
{
    // 双击进入编辑页面
    emit goAppointmentEditView(index.row());
}
