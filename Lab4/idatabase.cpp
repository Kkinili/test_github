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
    // 注意：数据库表已经存在，不需要重复创建
    // Doctor表和Department表结构如下：
    // Doctor: ID, EMPLOYEENO, NAME, DEPARTMENT_ID
    // Department: ID, NAME
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
