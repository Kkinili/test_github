#ifndef APPOINTMENTEDITVIEW_H
#define APPOINTMENTEDITVIEW_H

#include <QWidget>

namespace Ui {
class AppointmentEditView;
}

class AppointmentEditView : public QWidget
{
    Q_OBJECT

public:
    explicit AppointmentEditView(QWidget *parent = nullptr, int row=0);
    ~AppointmentEditView();

private slots:
    void on_btSave_clicked();
    void on_btCancel_clicked();

private:
    Ui::AppointmentEditView *ui;
    int currentRow;
    bool isNewRecord;

signals:
    void goPreviousView();
};

#endif // APPOINTMENTEDITVIEW_H
