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
#include <QCloseEvent>
#include "account.h"
#include "bill.h"
#include "billeditdialog.h"
#include "databasepathdialog.h"
#include "savedialog.h"
#include "calculatedialog.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    Account  *current_account;
    Bill     *current_bill;
    QString billSavingPath, obName, sbName, accountSavingPath, acName;

    QMenuBar *pMenuBar;
    QMenu    *pMenu1_File, *pMenu1_Account, *pMenu1_Help;
    QAction  *pAccount, *pVersion, *pNewAccount, *pNewBill, *pSaveAccount, *pSaveAsAccount, *pSaveBill, *pSaveAsBill;

    QWidget *CenterWindow;
    QPushButton   b1,  b2, b3, b4, b5;
    QGridLayout *ly1;
    QLabel        l1,  l2;
    QTableWidget *t1, *t2;

    BillEditDialog *dlg1;

    bool accountChanged, billChanged;

    MainWindow(QWidget *parent = nullptr);

    void accountExec();
    void clearTable(QTableWidget*);
    void updateBillTable();
    void updateAccountTable();
    void newBillExec();
    void deleteBillExec();
    void changeBillExec();
    void calculateTotal();
    void closeEvent(QCloseEvent*);
    int accountSaveAs();
    int accountSave();
    int billSaveAs();
    int billSave();
    void newOpenAccount();
    void newOpenBill();
    void calculateExec();
    void clearExec();

    ~MainWindow();

};

#endif // MAINWINDOW_H
