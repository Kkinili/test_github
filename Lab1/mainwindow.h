#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QLabel>
#include<QStack>
#include<QKeyEvent>
#include<QMap>
#include<QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString operand;
    QString opcode;
    QStack<QString> operands;
    QStack<QString>opcodes;
    QMap<int, QPushButton *> digitBTNs;

    QString calculation(bool *ok=NULL);

private slots:

    void btnNumClicked();

    void binBinaryOperatorClicked();

    void btnUnaryOperatorClicked();

    void on_btnPeriod_clicked();

    void on_btnDel_clicked();

    void on_btnClear_clicked();

    void on_btnEqual_clicked();

    virtual void keyPressEvent(QKeyEvent *event);

    void btnSignClicked();

    void on_btnClearAll_clicked();

private:
    Ui::MainWindow *ui;

    QLabel *statusMsgLabel;
    QMap<int, QPushButton*> symbolBTNs;
};
#endif // MAINWINDOW_H
