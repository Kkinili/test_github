#ifndef APPOINTMENTVIEW_H
#define APPOINTMENTVIEW_H

#include <QWidget>

namespace Ui {
class AppointmentView;
}

class AppointmentView : public QWidget
{
    Q_OBJECT

public:
    explicit AppointmentView(QWidget *parent = nullptr);
    ~AppointmentView();

private slots:
    void on_btAdd_clicked();
    void on_btSearch_clicked();
    void on_btDelete_clicked();
    void on_btEdit_clicked();
    void on_tableView_clicked(const QModelIndex &index);
    void on_tableView_doubleClicked(const QModelIndex &index);

signals:
    void goAppointmentEditView(int idx);

private:
    Ui::AppointmentView *ui;
};

#endif // APPOINTMENTVIEW_H
