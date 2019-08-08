#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include "account.h"
#include "bill.h"
#include "billeditdialog.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    QString  AccountFileAddress, OBillFileAddress, SBillFileAddress;
    Account  *current_account;
    Bill     *current_bill;
    int      BillStartNumber;
    OneBill  *bill_start, *bill_end;

    QMenuBar *pMenuBar;
    QMenu    *pMenu1_File, *pMenu1_Account, *pMenu1_Help;
    QAction  *pOpen,       *pAccount,       *pVersion;

    QWidget *CenterWindow;
    QPushButton   b1,  b2, b3, b4, b5;
    QGridLayout *ly1;
    QLabel        l1,  l2;
    QTableWidget *t1, *t2;

    BillEditDialog *dlg1;

    MainWindow(QWidget *parent = 0);

    void accountExec();
    void readBasicInfo(QString);
    void clearTable(QTableWidget*);
    void updateBillTable();
    void updateAccountTable();
    void newBillExec();
    void deleteBillExec();
    void changeBillExec();
    void calculateTotal(OneBill*, OneBill*);
    void billStartEnd();

    ~MainWindow();

};

#endif // MAINWINDOW_H
