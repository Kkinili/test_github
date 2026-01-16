#ifndef MASTERVIEW_H
#define MASTERVIEW_H

#include <QWidget>
#include "loginview.h"
#include "doctorview.h"
#include "doctoreditview.h"
#include "departmentview.h"
#include "departmenteditview.h"
#include "patienteditview.h"
#include "patientview.h"
#include "welcomeview.h"
#include "recordview.h"
#include "recordeditview.h"
#include "drugview.h"
#include "drugeditview.h"
#include "appointmentview.h"
#include "appointmenteditview.h"
#include "statisticsview.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MasterView;
}
QT_END_NAMESPACE

class MasterView : public QWidget
{
    Q_OBJECT

public:
    MasterView(QWidget *parent = nullptr);
    ~MasterView();

public slots:
    void goLoginView();
    void goWelcomeView();
    void goDoctorView();
    void goDepartmentView();
    void goPatientEditView(int rowNo);
    void goDoctorEditView(int rowNo);
    void goDepartmentEditView(int rowNo);
    void goPatientView();
    void goRecordView();
    void goRecordEditView(int rowNo);
    void goDrugView();
    void goDrugEditView(int rowNo);
    void goAppointmentView();
    void goAppointmentEditView(int rowNo);
    void goStatisticsView();
    void goPreviousView();

private slots:
    void on_btBack_clicked();

    void on_stackedWidget_currentChanged(int arg1);

    void on_btLogout_clicked();

private:
    void pushWidgetToStackView(QWidget *widget);

    Ui::MasterView *ui;

    WelcomeView *welcomeView;
    DoctorView *doctorView;
    DoctorEditView *doctorEditView;
    PatientView *patientView;
    DepartmentView *departmentView;
    DepartmentEditView *departmentEditView;
    LoginView *loginView;
    PatientEditView *patientEditView;
    RecordView *recordView;
    RecordEditView *recordEditView;
    DrugView *drugView;
    DrugEditView *drugEditView;
    AppointmentView *appointmentView;
    AppointmentEditView *appointmentEditView;
    StatisticsView *statisticsView;

};
#endif // MASTERVIEW_H
