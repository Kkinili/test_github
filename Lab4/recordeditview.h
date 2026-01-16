#ifndef RECORDEDITVIEW_H
#define RECORDEDITVIEW_H

#include <QWidget>

namespace Ui {
class RecordEditView;
}

class RecordEditView : public QWidget
{
    Q_OBJECT

public:
    explicit RecordEditView(QWidget *parent = nullptr, int row=0);
    ~RecordEditView();

private slots:
    void on_btSave_clicked();
    void on_btCancel_clicked();

private:
    Ui::RecordEditView *ui;
    int currentRow;
    bool isNewRecord;

signals:
    void goPreviousView();
};

#endif // RECORDEDITVIEW_H
