#include "masterview.h"
#include "ui_masterview.h"
#include <QDebug>
#include <QApplication>
#include "idatabase.h"

MasterView::MasterView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MasterView)
{
    ui->setupUi(this);

    this->setWindowFlag(Qt::FramelessWindowHint);

    goLoginView();

    IDatabase::getInstance();
}

MasterView::~MasterView()
{
    delete ui;
}

void MasterView::goLoginView()
{
    qDebug()<<"goLoginView";
    loginView = new LoginView(this);
    pushWidgetToStackView(loginView);

    connect(loginView, SIGNAL(loginSuccess()), this, SLOT(goWelcomeView()));
}
void MasterView::goWelcomeView()
{
    qDebug()<<"goWelcomeView";
    welcomeView = new WelcomeView(this);
    pushWidgetToStackView(welcomeView);

    connect(welcomeView, SIGNAL(goDoctorView()), this, SLOT(goDoctorView()));
    connect(welcomeView, SIGNAL(goPatientView()), this, SLOT(goPatientView()));
    connect(welcomeView, SIGNAL(goDepartmentView()), this, SLOT(goDepartmentView()));
    connect(welcomeView, SIGNAL(goRecordView()), this, SLOT(goRecordView()));
    connect(welcomeView, SIGNAL(goDrugView()), this, SLOT(goDrugView()));
    connect(welcomeView, SIGNAL(goAppointmentView()), this, SLOT(goAppointmentView()));
}

void MasterView::goDoctorView()
{
    qDebug()<<"goDoctorView";
    doctorView = new DoctorView(this);
    pushWidgetToStackView(doctorView);

    connect(doctorView, SIGNAL(goDoctorEditView(int)), this, SLOT(goDoctorEditView(int)));
}

void MasterView::goDepartmentView()
{
    qDebug()<<"goDepartmentView";
    departmentView = new DepartmentView(this);
    pushWidgetToStackView(departmentView);

    connect(departmentView, SIGNAL(goDepartmentEditView(int)), this, SLOT(goDepartmentEditView(int)));
}

void MasterView::goPatientEditView(int rowNo)
{
    qDebug()<<"goPatientEditView";
    patientEditView = new PatientEditView(this, rowNo);
    pushWidgetToStackView(patientEditView);

    connect(patientEditView, SIGNAL(goPreviousView()), this, SLOT(goPreviousView()));
}

void MasterView::goDoctorEditView(int rowNo)
{
    qDebug()<<"goDoctorEditView";
    doctorEditView = new DoctorEditView(this, rowNo);
    pushWidgetToStackView(doctorEditView);

    connect(doctorEditView, SIGNAL(goPreviousView()), this, SLOT(goPreviousView()));
}

void MasterView::goDepartmentEditView(int rowNo)
{
    qDebug()<<"goDepartmentEditView";
    departmentEditView = new DepartmentEditView(this, rowNo);
    pushWidgetToStackView(departmentEditView);

    connect(departmentEditView, SIGNAL(goPreviousView()), this, SLOT(goPreviousView()));
}

void MasterView::goPatientView()
{
    qDebug()<<"goPatientView";
    patientView = new PatientView(this);
    pushWidgetToStackView(patientView);

    connect(patientView, SIGNAL(goPatientEditView(int)), this, SLOT(goPatientEditView(int)));
}

void MasterView::goRecordView()
{
    qDebug()<<"goRecordView";
    recordView = new RecordView(this);
    pushWidgetToStackView(recordView);

    connect(recordView, SIGNAL(goRecordEditView(int)), this, SLOT(goRecordEditView(int)));
}

void MasterView::goRecordEditView(int rowNo)
{
    qDebug()<<"goRecordEditView";
    recordEditView = new RecordEditView(this, rowNo);
    pushWidgetToStackView(recordEditView);

    connect(recordEditView, SIGNAL(goPreviousView()), this, SLOT(goPreviousView()));
}

void MasterView::goDrugView()
{
    qDebug()<<"goDrugView";
    drugView = new DrugView(this);
    pushWidgetToStackView(drugView);

    connect(drugView, SIGNAL(goDrugEditView(int)), this, SLOT(goDrugEditView(int)));
}

void MasterView::goDrugEditView(int rowNo)
{
    qDebug()<<"goDrugEditView";
    drugEditView = new DrugEditView(this, rowNo);
    pushWidgetToStackView(drugEditView);

    connect(drugEditView, SIGNAL(goPreviousView()), this, SLOT(goPreviousView()));
}

void MasterView::goAppointmentView()
{
    qDebug()<<"goAppointmentView";
    appointmentView = new AppointmentView(this);
    pushWidgetToStackView(appointmentView);

    connect(appointmentView, SIGNAL(goAppointmentEditView(int)), this, SLOT(goAppointmentEditView(int)));
}

void MasterView::goAppointmentEditView(int rowNo)
{
    qDebug()<<"goAppointmentEditView";
    appointmentEditView = new AppointmentEditView(this, rowNo);
    pushWidgetToStackView(appointmentEditView);

    connect(appointmentEditView, SIGNAL(goPreviousView()), this, SLOT(goPreviousView()));
}

void MasterView::goPreviousView()
{
    qDebug() << "goPreviousView 被调用";
    int count = ui->stackedWidget->count();
    qDebug() << "当前堆栈中的widget数量:" << count;

    if(count > 1){
        qDebug() << "切换到上一个界面";

        // 先切换到上一个界面
        ui->stackedWidget->setCurrentIndex(count - 2);
        ui->labelTitle->setText(ui->stackedWidget->currentWidget()->windowTitle());

        // 删除当前的编辑界面
        QWidget *widget = ui->stackedWidget->widget(count - 1);
        ui->stackedWidget->removeWidget(widget);
        widget->deleteLater();

        // 强制刷新整个界面
        this->repaint();
        this->update();

        // 强制处理待处理的事件
        QApplication::processEvents();

        qDebug() << "界面切换完成";
    } else {
        qDebug() << "堆栈中只有一个widget，无法返回";
    }
}

void MasterView::pushWidgetToStackView(QWidget *widget)
{
    ui->stackedWidget->addWidget(widget);
    int count = ui->stackedWidget->count();
    ui->stackedWidget->setCurrentIndex(count - 1);//总是显示最新加入的View
    ui->labelTitle->setText(widget->windowTitle());
}

void MasterView::on_btBack_clicked()
{
    goPreviousView();
}


void MasterView::on_stackedWidget_currentChanged(int arg1)
{
    int count = ui->stackedWidget->count();
    if (count > 1)
        ui->btBack->setEnabled(true);
    else
        ui->btBack->setEnabled(false);

    QString title = ui->stackedWidget->currentWidget()->windowTitle();

    if(title == "欢迎"){
        ui->btLogout->setEnabled(true);
        ui->btBack->setEnabled(false);
    } else
        ui->btLogout->setEnabled(false);

    if(title == "登录")
        ui->btBack->setEnabled(false);
}


void MasterView::on_btLogout_clicked()
{
    goPreviousView();
}
