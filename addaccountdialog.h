#ifndef ADDACCOUNTDIALOG_H
#define ADDACCOUNTDIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QRadioButton>
#include <QMessageBox>
#include "account.h"


class AddAccountDialog : public QDialog
{
    Q_OBJECT

public:
    QLineEdit     e1,  e2;
    QLabel        l1,  l2;
    QGridLayout *ly1;
    QPushButton   b1,  b2;
    QButtonGroup  g1;
    QRadioButton  r1, r2;

    User *tempUser;

    void AcceptExec();
    void CancelExec();
    void statusChanged();

    AddAccountDialog(User*);
    ~AddAccountDialog();
};

#endif // ADDACCOUNTDIALOG_H
