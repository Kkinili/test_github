#ifndef RECORDEDITVIEW_H
#define RECORDEDITVIEW_H

#include <QWidget>
#include <QComboBox>
#include <QSpinBox>

namespace Ui {
class RecordEditView;
}

struct PrescriptionItem {
    QString drugName;
    int quantity;
    double unitPrice;
};

class RecordEditView : public QWidget
{
    Q_OBJECT

public:
    explicit RecordEditView(QWidget *parent = nullptr, int row = -1);
    ~RecordEditView();

private slots:
    void on_btSave_clicked();
    void on_btCancel_clicked();
    void on_btAddDrug_clicked();

private:
    Ui::RecordEditView *ui;
    int currentRow;
    bool isNewRecord;
    QList<PrescriptionItem> prescriptionItems;

    void loadPrescriptionFromRecord(int recordRow);
    void addPrescriptionItem(QString drugName, int quantity, double unitPrice);
    void updatePrescriptionTable();

signals:
    void goPreviousView();
};

#endif // RECORDEDITVIEW_H
