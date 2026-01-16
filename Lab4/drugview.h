#ifndef DRUGVIEW_H
#define DRUGVIEW_H

#include <QWidget>

namespace Ui {
class DrugView;
}

class DrugView : public QWidget
{
    Q_OBJECT

public:
    explicit DrugView(QWidget *parent = nullptr);
    ~DrugView();

private slots:
    void on_btAdd_clicked();
    void on_btSearch_clicked();
    void on_btDelete_clicked();
    void on_btEdit_clicked();
    void on_tableView_clicked(const QModelIndex &index);
    void on_tableView_doubleClicked(const QModelIndex &index);

signals:
    void goDrugEditView(int idx);

private:
    Ui::DrugView *ui;
};

#endif // DRUGVIEW_H
