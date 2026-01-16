#ifndef IDATABASE_H
#define IDATABASE_H

#include <QObject>
#include <QtSql>
#include <QSqlDatabase>
#include <QDataWidgetMapper>

class IDatabase : public QObject
{
    Q_OBJECT
public:

    static IDatabase& getInstance()
    {
        static IDatabase instance;
        return instance;
    }

    QString userLogin(QString userName, QString password);

private:
    explicit IDatabase(QObject *parent = nullptr);
    IDatabase(IDatabase const &)               = delete;
    void operator=(IDatabase const &) = delete;

    QSqlDatabase database;

    void ininDatabase();

signals:

public:
    // 患者管理
    bool initPatientModel();
    int addNewPatient();
    bool searchPatient(QString filter);
    bool deleteCurrentPatient();
    bool submitPatientEdit();
    void revertPatientEdit();

    QSqlTableModel  *patientTabModel;
    QItemSelectionModel *thePatientSelection;

    // 医生管理
    bool initDoctorModel();
    int addNewDoctor();
    bool searchDoctor(QString filter);
    bool deleteCurrentDoctor();
    bool submitDoctorEdit();
    void revertDoctorEdit();

    QSqlTableModel  *doctorTabModel;
    QItemSelectionModel *theDoctorSelection;

    // 科室管理
    bool initDepartmentModel();
    int addNewDepartment();
    bool searchDepartment(QString filter);
    bool deleteCurrentDepartment();
    bool submitDepartmentEdit();
    void revertDepartmentEdit();

    QSqlTableModel  *departmentTabModel;
    QItemSelectionModel *theDepartmentSelection;

    // 就诊记录管理
    bool initRecordModel();
    int addNewRecord();
    bool searchRecord(QString filter);
    bool deleteCurrentRecord();
    bool submitRecordEdit();
    void revertRecordEdit();

    QSqlTableModel  *recordTabModel;
    QItemSelectionModel *theRecordSelection;

    // 药品库存管理
    bool initDrugModel();
    int addNewDrug();
    bool searchDrug(QString filter);
    bool deleteCurrentDrug();
    bool submitDrugEdit();
    void revertDrugEdit();

    QSqlTableModel  *drugTabModel;
    QItemSelectionModel *theDrugSelection;

    // 预约排班管理
    bool initAppointmentModel();
    int addNewAppointment();
    bool searchAppointment(QString filter);
    bool deleteCurrentAppointment();
    bool submitAppointmentEdit();
    void revertAppointmentEdit();

    QSqlTableModel  *appointmentTabModel;
    QItemSelectionModel *theAppointmentSelection;

    // 获取患者名称（用于显示）
    QString getPatientNameById(int patientId);
    // 获取医生名称（用于显示）
    QString getDoctorNameById(int doctorId);

};

#endif // IDATABASE_H
