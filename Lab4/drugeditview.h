#ifndef DRUGEDITVIEW_H
#define DRUGEDITVIEW_H

#include <QWidget>

namespace Ui {
class DrugEditView;
}

class DrugEditView : public QWidget
{
    Q_OBJECT

public:
    explicit DrugEditView(QWidget *parent = nullptr, int row=0);
    ~DrugEditView();

private slots:
    void on_btSave_clicked();
    void on_btCancel_clicked();

private:
    Ui::DrugEditView *ui;
    int currentRow;
    bool isNewRecord;

signals:
    void goPreviousView();
};

#endif // DRUGEDITVIEW_H
