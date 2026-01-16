#ifndef RECORDVIEW_H
#define RECORDVIEW_H

#include <QWidget>

namespace Ui {
class RecordView;
}

class RecordView : public QWidget
{
    Q_OBJECT

public:
    explicit RecordView(QWidget *parent = nullptr);
    ~RecordView();

private slots:
    void on_btAdd_clicked();
    void on_btDelete_clicked();
    void on_btSearch_clicked();
    void on_tableView_clicked(const QModelIndex &index);
    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    Ui::RecordView *ui;
    void initDbView();

signals:
    void goRecordEditView(int index);
    void goPreviousView();
};

#endif // RECORDVIEW_H
