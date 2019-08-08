#ifndef ACCOUNTDIALOG_H
#define ACCOUNTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include <QGridLayout>
#include "account.h"
#include "addaccountdialog.h"

class AccountDialog : public QDialog
{
    Q_OBJECT

public:
    QLabel       l1,   l2;
    QTableWidget *t1;
    QPushButton  b1,   b2,   b3,   b4,   b5;
    QGridLayout  *ly1;
    QString      receiveTemp1;

    Account          *current_account;
    AddAccountDialog *dlg;

    void SaveExec();
    void CancelExec();
    void AddExec();
    void DeleteExec();
    void ChangeExec();
    void UpdateTable();
    void receiveFcn1(QString);

    void (AddAccountDialog::*addSignal) (QString);

    AccountDialog(QString);
    ~AccountDialog();

};

#endif // ACCOUNTDIALOG_H
