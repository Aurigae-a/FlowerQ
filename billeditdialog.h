#ifndef BILLEDITDIALOG_H
#define BILLEDITDIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <math.h>
#include "account.h"
#include "bill.h"


struct CheckBoxList{
    QCheckBox    *box;
    QLineEdit    *edit;
    bool         box_state;
    CheckBoxList *next;
};


class BillEditDialog : public QDialog
{
public:
    Account         *account;
    OneBill         *object_bill;
    Bill            *current_bill;

    QGridLayout  *ly1, *ly2, *ly3;
    QLabel       l1, l2, l3, l4, l5 , l6, l7, l8;
    QLineEdit    e1, e2, e3, e4, e5, e6;
    QPushButton  b1, b2, b3, b4, b5, b6, b7;
    QTableWidget *t1;

    CheckBoxList *head1, *tail1, *head2, *tail2;

    void AcceptExec();
    void CancelExec();
    void newCheckBox(CheckBoxList*&, CheckBoxList*&, QGridLayout*);
    void deleteCheckBox(CheckBoxList*&, CheckBoxList*&);
    void slotCheckBox(int);
    void slotMoneyEdit();
    void updateCheckBox(CheckBoxList*);
    void createItems();
    void adjustToBillData();
    void connectingCheckBox(bool, CheckBoxList*);
    void clearTable();
    void updateTable();
    void addSubBill();
    void deleteSubBill();
    void changeSubBill();
    void defaultCalculate(double, CheckBoxList*);
    void paidDefault();
    void owedDefault();
    double calculateRemained();
    bool checkPaid();
    bool checkOwed();

    BillEditDialog(OneBill*, Account*, Bill*);
    ~BillEditDialog();

};

#endif // BILLEDITDIALOG_H
