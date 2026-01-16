#include "idatabase.h"
#include <QUuid>

void IDatabase::ininDatabase()
{
    database = QSqlDatabase::addDatabase("QSQLITE");
    QString aFile = "D:/QtCode/lab4a.db";
    database.setDatabaseName(aFile);

    if(!database.open()){
        qDebug() << "failed to open database";
    }else
        qDebug() << "open database is ok";

    // 创建药品库存表
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS drug_inventory ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name VARCHAR(50) NOT NULL, "
               "category VARCHAR(20), "
               "stock_qty INTEGER DEFAULT 0, "
               "unit_price DECIMAL(10,2), "
               "manufacturer VARCHAR(50), "
               "expiry_date DATE)");

    // 创建预约表
    query.exec("CREATE TABLE IF NOT EXISTS appointments ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "patient_id INTEGER, "
               "doctor_id INTEGER, "
               "appoint_date DATETIME, "
               "status VARCHAR(20), "
               "notes TEXT)");

    // 创建处方明细表（用于存储就诊时的药品处方信息）
    query.exec("CREATE TABLE IF NOT EXISTS prescriptions ("
               "id INTEGER PRIMARY KEY, "
               "record_id VARCHAR(50), "
               "drug_name VARCHAR(50) NOT NULL, "
               "quantity INTEGER NOT NULL, "
               "unit_price DECIMAL(10,2), "
               "prescribed_at DATETIME DEFAULT CURRENT_TIMESTAMP)");

    // 初始化所有数据模型
    initPatientModel();
    initDoctorModel();
    initDepartmentModel();
    initRecordModel();
    initDrugModel();
    initAppointmentModel();
}


bool IDatabase::initPatientModel()
{
    patientTabModel = new QSqlTableModel(this, database);
    patientTabModel->setTable("Patient");
    patientTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    patientTabModel->setSort(patientTabModel->fieldIndex("NAME"), Qt::AscendingOrder);
    if(!(patientTabModel->select()))
        return false;

    thePatientSelection = new QItemSelectionModel(patientTabModel);
    return true;
}

int IDatabase::addNewPatient()
{
    patientTabModel->insertRow(patientTabModel->rowCount(),QModelIndex());//在末尾添加一个记录
    QModelIndex curIndex = patientTabModel->index(patientTabModel->rowCount() - 1,1);//创建最后一行的ModelIndex

    int curRecNo = curIndex.row();
    QSqlRecord curRec = patientTabModel->record(curRecNo);//获取当前记录
    curRec.setValue("CREATEDTIMESTAMP", QDateTime :: currentDateTime().toString("yyyy-MM-dd"));
    curRec.setValue("ID", QUuid :: createUuid().toString(QUuid :: WithoutBraces));
    curRec.setValue("AGE", 0);  // 默认年龄为0，编辑时会自动计算

    patientTabModel->setRecord(curRecNo, curRec);

    return curIndex.row();
}


bool IDatabase::searchPatient(QString filter)
{
    patientTabModel->setFilter(filter);
    return patientTabModel->select();
}

bool IDatabase::deleteCurrentPatient()
{
    // 检查选择模型和当前索引是否有效
    if (!thePatientSelection || !thePatientSelection->currentIndex().isValid()) {
        qDebug() << "未选中任何患者，无法删除";
        return false;
    }

    QModelIndex curIndex = thePatientSelection->currentIndex();
    // 尝试删除行
    if (patientTabModel->removeRow(curIndex.row())) {
        patientTabModel->submitAll(); // 提交更改到数据库
        patientTabModel->select();    // 重新查询以刷新表格
        qDebug() << "删除成功";
        return true;
    } else {
        qDebug() << "删除失败";
        return false;
    }
}


bool IDatabase::submitPatientEdit()
{
    return patientTabModel->submitAll();
}

void IDatabase::revertPatientEdit()
{
    patientTabModel->revertAll();
}

// ==================== 医生管理模块实现 ====================

bool IDatabase::initDoctorModel()
{
    doctorTabModel = new QSqlTableModel(this, database);
    doctorTabModel->setTable("Doctor");
    doctorTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    doctorTabModel->setSort(doctorTabModel->fieldIndex("NAME"), Qt::AscendingOrder);
    if(!(doctorTabModel->select()))
        return false;

    theDoctorSelection = new QItemSelectionModel(doctorTabModel);
    return true;
}

int IDatabase::addNewDoctor()
{
    doctorTabModel->insertRow(doctorTabModel->rowCount(), QModelIndex());
    QModelIndex curIndex = doctorTabModel->index(doctorTabModel->rowCount() - 1, 1);

    int curRecNo = curIndex.row();
    QSqlRecord curRec = doctorTabModel->record(curRecNo);
    // 不自动生成ID，让用户手动填写

    doctorTabModel->setRecord(curRecNo, curRec);

    return curIndex.row();
}

bool IDatabase::searchDoctor(QString filter)
{
    doctorTabModel->setFilter(filter);
    return doctorTabModel->select();
}

bool IDatabase::deleteCurrentDoctor()
{
    if (!theDoctorSelection || !theDoctorSelection->currentIndex().isValid()) {
        qDebug() << "未选中任何医生，无法删除";
        return false;
    }

    QModelIndex curIndex = theDoctorSelection->currentIndex();
    if (doctorTabModel->removeRow(curIndex.row())) {
        doctorTabModel->submitAll();
        doctorTabModel->select();
        qDebug() << "删除医生成功";
        return true;
    } else {
        qDebug() << "删除医生失败";
        return false;
    }
}

bool IDatabase::submitDoctorEdit()
{
    return doctorTabModel->submitAll();
}

void IDatabase::revertDoctorEdit()
{
    doctorTabModel->revertAll();
}

// ==================== 科室管理模块实现 ====================

bool IDatabase::initDepartmentModel()
{
    departmentTabModel = new QSqlTableModel(this, database);
    departmentTabModel->setTable("Department");
    departmentTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    departmentTabModel->setSort(departmentTabModel->fieldIndex("NAME"), Qt::AscendingOrder);
    if(!(departmentTabModel->select()))
        return false;

    theDepartmentSelection = new QItemSelectionModel(departmentTabModel);
    return true;
}

int IDatabase::addNewDepartment()
{
    departmentTabModel->insertRow(departmentTabModel->rowCount(), QModelIndex());
    QModelIndex curIndex = departmentTabModel->index(departmentTabModel->rowCount() - 1, 1);

    int curRecNo = curIndex.row();
    QSqlRecord curRec = departmentTabModel->record(curRecNo);
    // 不自动生成ID，让用户手动填写

    departmentTabModel->setRecord(curRecNo, curRec);

    return curIndex.row();
}

bool IDatabase::searchDepartment(QString filter)
{
    departmentTabModel->setFilter(filter);
    return departmentTabModel->select();
}

bool IDatabase::deleteCurrentDepartment()
{
    if (!theDepartmentSelection || !theDepartmentSelection->currentIndex().isValid()) {
        qDebug() << "未选中任何科室，无法删除";
        return false;
    }

    QModelIndex curIndex = theDepartmentSelection->currentIndex();
    if (departmentTabModel->removeRow(curIndex.row())) {
        departmentTabModel->submitAll();
        departmentTabModel->select();
        qDebug() << "删除科室成功";
        return true;
    } else {
        qDebug() << "删除科室失败";
        return false;
    }
}

bool IDatabase::submitDepartmentEdit()
{
    return departmentTabModel->submitAll();
}

void IDatabase::revertDepartmentEdit()
{
    departmentTabModel->revertAll();
}

// ==================== 就诊记录管理模块实现 ====================

bool IDatabase::initRecordModel()
{
    recordTabModel = new QSqlTableModel(this, database);
    recordTabModel->setTable("MedicalRecord");
    recordTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    recordTabModel->setSort(recordTabModel->fieldIndex("VISIT_DATE"), Qt::DescendingOrder);
    if(!(recordTabModel->select()))
        return false;

    theRecordSelection = new QItemSelectionModel(recordTabModel);
    return true;
}

int IDatabase::addNewRecord()
{
    // 计算要插入的行号（最后一行之后）
    int newRow = recordTabModel->rowCount();
    recordTabModel->insertRow(newRow, QModelIndex());

    QSqlRecord curRec = recordTabModel->record(newRow);
    // 设置默认就诊日期为当前时间
    curRec.setValue("VISIT_DATE", QDateTime::currentDateTime());
    curRec.setValue("STATUS", 0);  // 0: 进行中

    recordTabModel->setRecord(newRow, curRec);

    return newRow;
}

bool IDatabase::searchRecord(QString filter)
{
    recordTabModel->setFilter(filter);
    return recordTabModel->select();
}

bool IDatabase::deleteCurrentRecord()
{
    if (!theRecordSelection || !theRecordSelection->currentIndex().isValid()) {
        qDebug() << "未选中任何记录，无法删除";
        return false;
    }

    QModelIndex curIndex = theRecordSelection->currentIndex();
    if (recordTabModel->removeRow(curIndex.row())) {
        recordTabModel->submitAll();
        recordTabModel->select();
        qDebug() << "删除记录成功";
        return true;
    } else {
        qDebug() << "删除记录失败";
        return false;
    }
}

bool IDatabase::submitRecordEdit()
{
    return recordTabModel->submitAll();
}

void IDatabase::revertRecordEdit()
{
    recordTabModel->revertAll();
}

// ==================== 药品库存管理模块实现 ====================

bool IDatabase::initDrugModel()
{
    drugTabModel = new QSqlTableModel(this, database);
    drugTabModel->setTable("drug_inventory");
    drugTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    drugTabModel->setSort(drugTabModel->fieldIndex("NAME"), Qt::AscendingOrder);
    if(!(drugTabModel->select()))
        return false;

    theDrugSelection = new QItemSelectionModel(drugTabModel);
    return true;
}

int IDatabase::addNewDrug()
{
    drugTabModel->insertRow(drugTabModel->rowCount(), QModelIndex());
    QModelIndex curIndex = drugTabModel->index(drugTabModel->rowCount() - 1, 1);

    int curRecNo = curIndex.row();
    QSqlRecord curRec = drugTabModel->record(curRecNo);
    // 设置默认库存为0
    curRec.setValue("stock_qty", 0);

    drugTabModel->setRecord(curRecNo, curRec);

    return curIndex.row();
}

bool IDatabase::searchDrug(QString filter)
{
    drugTabModel->setFilter(filter);
    return drugTabModel->select();
}

bool IDatabase::deleteCurrentDrug()
{
    if (!theDrugSelection || !theDrugSelection->currentIndex().isValid()) {
        qDebug() << "未选中任何药品，无法删除";
        return false;
    }

    QModelIndex curIndex = theDrugSelection->currentIndex();
    if (drugTabModel->removeRow(curIndex.row())) {
        drugTabModel->submitAll();
        drugTabModel->select();
        qDebug() << "删除药品成功";
        return true;
    } else {
        qDebug() << "删除药品失败";
        return false;
    }
}

bool IDatabase::submitDrugEdit()
{
    return drugTabModel->submitAll();
}

void IDatabase::revertDrugEdit()
{
    drugTabModel->revertAll();
}

// ==================== 预约排班管理模块实现 ====================

bool IDatabase::initAppointmentModel()
{
    appointmentTabModel = new QSqlTableModel(this, database);
    appointmentTabModel->setTable("appointments");
    appointmentTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    appointmentTabModel->setSort(appointmentTabModel->fieldIndex("appoint_date"), Qt::DescendingOrder);
    if(!(appointmentTabModel->select()))
        return false;

    theAppointmentSelection = new QItemSelectionModel(appointmentTabModel);
    return true;
}

int IDatabase::addNewAppointment()
{
    appointmentTabModel->insertRow(appointmentTabModel->rowCount(), QModelIndex());
    QModelIndex curIndex = appointmentTabModel->index(appointmentTabModel->rowCount() - 1, 1);

    int curRecNo = curIndex.row();
    QSqlRecord curRec = appointmentTabModel->record(curRecNo);
    // 设置默认状态为待就诊
    curRec.setValue("status", "待就诊");

    appointmentTabModel->setRecord(curRecNo, curRec);

    return curIndex.row();
}

bool IDatabase::searchAppointment(QString filter)
{
    appointmentTabModel->setFilter(filter);
    return appointmentTabModel->select();
}

bool IDatabase::deleteCurrentAppointment()
{
    if (!theAppointmentSelection || !theAppointmentSelection->currentIndex().isValid()) {
        qDebug() << "未选中任何预约，无法删除";
        return false;
    }

    QModelIndex curIndex = theAppointmentSelection->currentIndex();
    if (appointmentTabModel->removeRow(curIndex.row())) {
        appointmentTabModel->submitAll();
        appointmentTabModel->select();
        qDebug() << "删除预约成功";
        return true;
    } else {
        qDebug() << "删除预约失败";
        return false;
    }
}

bool IDatabase::submitAppointmentEdit()
{
    return appointmentTabModel->submitAll();
}

void IDatabase::revertAppointmentEdit()
{
    appointmentTabModel->revertAll();
}

QString IDatabase::getPatientNameById(int patientId)
{
    QSqlQuery query;
    query.prepare("SELECT NAME FROM Patient WHERE ID = :ID");
    query.bindValue(":ID", patientId);
    query.exec();
    if (query.next()) {
        return query.value("NAME").toString();
    }
    return "";
}

QString IDatabase::getDoctorNameById(int doctorId)
{
    QSqlQuery query;
    query.prepare("SELECT NAME FROM Doctor WHERE ID = :ID");
    query.bindValue(":ID", doctorId);
    query.exec();
    if (query.next()) {
        return query.value("NAME").toString();
    }
    return "";
}

QString IDatabase::userLogin(QString userName, QString password)
{
    QSqlQuery query;
    query.prepare("SELECT USERNAME, PASSWORD FROM User WHERE USERNAME = :USER");
    query.bindValue(":USER", userName);
    query.exec();
    qDebug() << query.lastQuery() << query.first();

    if (query.first() && query.value("USERNAME").isValid()) {
        QString passwd = query.value("PASSWORD").toString();
        if (passwd == password) {
            qDebug() << "login ok";
            return "loginOk";
        } else {
            qDebug() << "wrong password";
            return "wrongPassword";
        }
    }else{
        qDebug() << "no such user";
        return "wrongUsername";
    }
}

IDatabase::IDatabase(QObject *parent)
    : QObject{parent}
{
    ininDatabase();
}
