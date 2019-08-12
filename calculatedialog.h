#ifndef CALCULATEDIALOG_H
#define CALCULATEDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QGridLayout>
#include <QStringList>
#include <QHeaderView>
#include <QMessageBox>
#include <QDateTime>
#include "bill.h"

struct CartList{
    OneBill *billptr;
    QCheckBox *box;
    CartList *next;
};

struct MoneyStream{
    User *sender;
    User *receiver;
    double money;
    MoneyStream *next;
};

class CalculateDialog : public QDialog
{
public:
    QGridLayout *ly1;
    QLabel l1;
    QPushButton b1, b2, b3;
    QTableWidget *t1;
    QCheckBox c1;

    CartList *cartHead;
    Bill* currentBill;
    Account* currentAccount, *tempAccount;
    int totalBill;
    MoneyStream *msHead;

    CalculateDialog(int, Account*, Bill*);
    void initialTable();
    void clearTable();
    void finishExec();
    void calculate();
    void calculateExec();
    void clearExec();
    void examineSeleteAll();
    void seleteAllExec();
    ~CalculateDialog();
};

#endif // CALCULATEDIALOG_H
