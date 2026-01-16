#include "recordview.h"
#include "ui_recordview.h"
#include "idatabase.h"
#include <QSqlTableModel>
#include <QItemSelectionModel>
#include <QStyledItemDelegate>
#include <QComboBox>
#include <QDebug>

// 自定义委托，用于显示状态文本
class StatusDelegate : public QStyledItemDelegate
{
public:
    explicit StatusDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    QString displayText(const QVariant &value, const QLocale &locale) const override
    {
        Q_UNUSED(locale);
        int status = value.toInt();
        QStringList statusTexts = {"进行中", "已完成", "已取消"};
        if (status >= 0 && status < statusTexts.size()) {
            return statusTexts[status];
        }
        return QString::number(status);
    }
};

RecordView::RecordView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecordView)
{
    ui->setupUi(this);

    initDbView();

    // 连接选择模型的点击信号
    connect(ui->tableView, &QTableView::clicked,
            this, &RecordView::on_tableView_clicked);

    // 连接双击信号
    connect(ui->tableView, &QTableView::doubleClicked,
            this, &RecordView::on_tableView_doubleClicked);
}

RecordView::~RecordView()
{
    delete ui;
}

void RecordView::initDbView()
{
    // 设置表格模型
    ui->tableView->setModel(IDatabase::getInstance().recordTabModel);

    // 设置选择模式为单行选择
    ui->tableView->setSelectionMode(QTableView::SingleSelection);
    ui->tableView->setSelectionBehavior(QTableView::SelectRows);

    // 为STATUS列设置自定义委托，显示文本而非数字
    QSqlTableModel *model = IDatabase::getInstance().recordTabModel;
    int statusCol = model->fieldIndex("STATUS");
    qDebug() << "STATUS column index:" << statusCol;
    if (statusCol >= 0) {
        ui->tableView->setItemDelegateForColumn(statusCol, new StatusDelegate(this));
        qDebug() << "StatusDelegate applied to column" << statusCol;
    } else {
        qDebug() << "STATUS column not found!";
    }

    // 隐藏ID列（不显示）
    // 注意：实际列索引可能需要根据表结构调整
}

void RecordView::on_btAdd_clicked()
{
    int index = IDatabase::getInstance().addNewRecord();
    emit goRecordEditView(index);
}

void RecordView::on_btDelete_clicked()
{
    IDatabase::getInstance().deleteCurrentRecord();
}

void RecordView::on_btSearch_clicked()
{
    QString filter = ui->dbEditSearch->text();
    IDatabase::getInstance().searchRecord(filter);
}

void RecordView::on_tableView_clicked(const QModelIndex &index)
{
    // 单击选择记录
}

void RecordView::on_tableView_doubleClicked(const QModelIndex &index)
{
    // 双击进入编辑页面
    emit goRecordEditView(index.row());
}
//1
