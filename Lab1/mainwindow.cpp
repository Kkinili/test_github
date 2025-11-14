#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <math.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    digitBTNs = {{Qt::Key_0, ui->btnNum0},
               {Qt::Key_1, ui->btnNum1},
               {Qt::Key_2, ui->btnNum2},
               {Qt::Key_3, ui->btnNum3},
               {Qt::Key_4, ui->btnNum4},
               {Qt::Key_5, ui->btnNum5},
               {Qt::Key_6, ui->btnNum6},
               {Qt::Key_7, ui->btnNum7},
               {Qt::Key_8, ui->btnNum8},
               {Qt::Key_9, ui->btnNum9},
               };

    foreach(auto btn, digitBTNs )
        connect(btn, SIGNAL(clicked()),this,SLOT(btnNumClicked()));

    // 初始化符号按键映射
    symbolBTNs = {
        {Qt::Key_Plus, ui->btnPlus},         // 加号 +
        {Qt::Key_Minus, ui->btnMinus},       // 减号 -
        {Qt::Key_Asterisk, ui->btnMultiple}, // 乘号 *
        {Qt::Key_Slash, ui->btnDivide},      // 除号 /
        {Qt::Key_Percent, ui->btnPercentage},// 百分号 %
        {Qt::Key_Enter, ui->btnEqual},       // 回车键 =
        {Qt::Key_Return, ui->btnEqual},      // 小键盘回车键 =
        {Qt::Key_Backspace, ui->btnDel},     // 退格键 ←
        {Qt::Key_C, ui->btnClear},       // Delete键 C
        {Qt::Key_Delete, ui->btnPeriod}
    };

    connect(ui->btnPlus,SIGNAL(clicked()),this,SLOT(binBinaryOperatorClicked()));
    connect(ui->btnMinus,SIGNAL(clicked()),this,SLOT(binBinaryOperatorClicked()));
    connect(ui->btnMultiple,SIGNAL(clicked()),this,SLOT(binBinaryOperatorClicked()));
    connect(ui->btnDivide,SIGNAL(clicked()),this,SLOT(binBinaryOperatorClicked()));

    connect(ui->btnPercentage,SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnInverse,SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnSquare,SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnSqrt,SIGNAL(clicked()),this,SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnSign, SIGNAL(clicked()), this, SLOT(btnSignClicked()));
    connect(ui->btnClear, SIGNAL(clicked()), this, SLOT(on_btnClearAll_clicked()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::calculation(bool *ok)
{
    double result = 0;
    if(operands.size() == 2 && opcodes.size() > 0 ){
        //取操作数
        double operand1 = operands.front().toDouble();
        operands.pop_front();
        double operand2 = operands.front().toDouble();
        operands.pop_front();

        //取操作符
        QString op = opcodes.front();
        opcodes.pop_front();

        if(op == "+"){
            result = operand1 + operand2;
        } else if(op == "-"){
            result = operand1 - operand2;
        } else if(op == "×"){
            result = operand1 * operand2;
        } else if(op == "/"){
            result = operand1 / operand2;
        }

        operands.push_back(QString::number(result));

        ui->statusbar->showMessage(QString("calcuation is in progress: operands is %1, opcode is %2").arg(operands.size()).arg(opcodes.size()));
    }else
        ui->statusbar->showMessage(QString("operands is %1, opcode is %2").arg(operands.size()).arg(opcodes.size()));

    return QString::number(result);
}

void MainWindow::btnNumClicked()
{
    QString digit = qobject_cast<QPushButton*>(sender())->text();

    if(digit == "0" && operand == "0")
        digit = "";

    if(operand == "0" && digit !="0")
        operand = "";

    operand += digit;


    ui->display->setText(operand);

}

void MainWindow::on_btnPeriod_clicked()
{
    if(!operand.contains("."))
        operand += qobject_cast<QPushButton*>(sender())->text();
    ui->display->setText(operand);
}


void MainWindow::on_btnDel_clicked()
{
    operand = operand.left(operand.length()-1);
    ui->display->setText(operand);
}


void MainWindow::on_btnClear_clicked()
{
    operand.clear();
    ui->display->setText(operand);
}

void MainWindow::on_btnClearAll_clicked()
{
    operand.clear();         // 清空当前输入的操作数
    operands.clear();        // 清空操作数队列
    opcodes.clear();         // 清空运算符队列
    ui->display->setText("0"); // 显示框重置为0
    ui->statusbar->clearMessage(); // 清空状态栏信息
}

void MainWindow::binBinaryOperatorClicked()
{
    ui->statusbar->showMessage("last operand " + operand);
    QString opcode = qobject_cast<QPushButton*>(sender())->text();
    qDebug()<<opcode;

    if (operand != "")
    {
        operands.push_back(operand);
        operand = "";

        opcodes.push_back(opcode);

    QString result=calculation();

    ui->display->setText(result);
    }

}

void MainWindow::btnUnaryOperatorClicked()
{
    if (operand != ""){
        double result = operand.toDouble();
        operand = "";

        QString op = qobject_cast<QPushButton *>(sender())->text();

        if(op == "%")
            result /=100.0;
        else if (op == "1/x")
            result = 1/result;
        else if (op == "x^2")
            result *= result;
        else if (op == "√")
            result = sqrt(result);

        ui->display->setText(QString::number(result));
    }

}

void MainWindow::on_btnEqual_clicked()
{
    if (operand != ""){
        operands.push_back(operand);
        operand = "";
    }

    QString result=calculation();
    ui->display->setText(result);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    foreach (auto btnKey , digitBTNs.keys())
    {
        if(event->key() == btnKey)
            digitBTNs[btnKey]->animateClick();
    }

    foreach (auto btnKey , symbolBTNs.keys())
    {
        if(event->key() == btnKey)
            symbolBTNs[btnKey]->animateClick();
    }
}

void MainWindow::btnSignClicked()
{
    if (!operand.isEmpty()) {
        bool isNegative = operand.startsWith('-');
        if (isNegative) {
            operand.remove(0, 1); // 移除负号
        } else {
            operand.prepend('-'); // 添加负号
        }
        ui->display->setText(operand);
    }
}
