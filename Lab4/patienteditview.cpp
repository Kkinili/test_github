#include "patienteditview.h"
#include "ui_patienteditview.h"
#include "idatabase.h"
#include <QSqlTableModel>
#include <QDate>

PatientEditView::PatientEditView(QWidget *parent, int index)
    : QWidget(parent)
    , ui(new Ui::PatientEditView)
{
    ui->setupUi(this);

    dataMapper = new QDataWidgetMapper();
    QSqlTableModel *tabModel = IDatabase::getInstance().patientTabModel;
    dataMapper->setModel(IDatabase::getInstance().patientTabModel);
    dataMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

    dataMapper->addMapping(ui->dbEditID, tabModel->fieldIndex("ID"));
    dataMapper->addMapping(ui->dbEditName, tabModel->fieldIndex("NAME"));
    dataMapper->addMapping(ui->dbEditIDCard, tabModel->fieldIndex("ID_CARD"));
    dataMapper->addMapping(ui->dbSpinHeight, tabModel->fieldIndex("HEIGHT"));
    dataMapper->addMapping(ui->dbSpinWeight, tabModel->fieldIndex("WEIGHT"));
    dataMapper->addMapping(ui->dbEditMobile, tabModel->fieldIndex("MOBILEPHONE"));
    dataMapper->addMapping(ui->dbDateEditDOB, tabModel->fieldIndex("DOB"));
    dataMapper->addMapping(ui->dbComboSex, tabModel->fieldIndex("SEX"));
    dataMapper->addMapping(ui->dbCreatedTimeStamp, tabModel->fieldIndex("CREATEDTIMESTAMP"));

    dataMapper->setCurrentIndex(index);

    ui->dbEditID->setEnabled(false);
    ui->dbCreatedTimeStamp->setEnabled(false);
}

PatientEditView::~PatientEditView()
{
    delete ui;
}

void PatientEditView::on_pushButton_clicked()
{
    // 先提交mapper中的数据，确保DOB等字段正确同步到模型
    dataMapper->submit();

    // 保存前根据DOB自动计算AGE
    QDate dob = ui->dbDateEditDOB->date();
    if (dob.isValid()) {
        int age = QDate::currentDate().year() - dob.year();
        // 如果今年还没过生日，年龄减1
        if (QDate::currentDate().month() < dob.month() ||
            (QDate::currentDate().month() == dob.month() && QDate::currentDate().day() < dob.day())) {
            age--;
        }
        // 直接设置模型中的AGE字段
        int ageFieldIndex = IDatabase::getInstance().patientTabModel->fieldIndex("AGE");
        IDatabase::getInstance().patientTabModel->setData(
            IDatabase::getInstance().patientTabModel->index(dataMapper->currentIndex(), ageFieldIndex), age);
    }

    IDatabase::getInstance().submitPatientEdit();
    emit goPreviousView();
}

void PatientEditView::on_pushButton_2_clicked()
{
    IDatabase::getInstance().revertPatientEdit();
    emit goPreviousView();
}
