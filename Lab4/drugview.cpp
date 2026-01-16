#include "drugview.h"
#include "ui_drugview.h"
#include "idatabase.h"
#include <QSqlTableModel>
#include <QItemSelectionModel>
#include <QDebug>

DrugView::DrugView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DrugView)
{
    ui->setupUi(this);

    // 设置表格模型
    ui->tableView->setModel(IDatabase::getInstance().drugTabModel);

    // 设置选择模式为单行选择
    ui->tableView->setSelectionMode(QTableView::SingleSelection);
    ui->tableView->setSelectionBehavior(QTableView::SelectRows);

    // 设置选择模型
    ui->tableView->setSelectionModel(IDatabase::getInstance().theDrugSelection);

    // 连接选择模型的点击信号
    connect(ui->tableView, &QTableView::clicked,
            this, &DrugView::on_tableView_clicked);

    // 连接双击信号
    connect(ui->tableView, &QTableView::doubleClicked,
            this, &DrugView::on_tableView_doubleClicked);
}

DrugView::~DrugView()
{
    delete ui;
}

void DrugView::on_btAdd_clicked()
{
    int index = IDatabase::getInstance().addNewDrug();
    emit goDrugEditView(index);
}

void DrugView::on_btDelete_clicked()
{
    IDatabase::getInstance().deleteCurrentDrug();
}

void DrugView::on_btSearch_clicked()
{
    QString filter = QString("name like '%%1%'").arg(ui->dbEditSearch->text());
    IDatabase::getInstance().searchDrug(filter);
}

void DrugView::on_btEdit_clicked()
{
    // 获取当前选中的行
    QModelIndex curIndex = IDatabase::getInstance().theDrugSelection->currentIndex();
    if (curIndex.isValid()) {
        emit goDrugEditView(curIndex.row());
    }
}

void DrugView::on_tableView_clicked(const QModelIndex &index)
{
    // 单击选择记录
    Q_UNUSED(index);
}

void DrugView::on_tableView_doubleClicked(const QModelIndex &index)
{
    // 双击进入编辑页面
    emit goDrugEditView(index.row());
}
