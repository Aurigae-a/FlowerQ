#ifndef ADDACCOUNTDIALOG_H
#define ADDACCOUNTDIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include "account.h"


class AddAccountDialog : public QDialog
{
    Q_OBJECT

public:
    QLineEdit     e1,  e2;
    QLabel        l1,  l2;
    QGridLayout *ly1;
    QPushButton   b1,  b2;

    int     command, Number;
    QString Name;

    void AcceptExec();
    void CancelExec();

    AddAccountDialog(int, int, QString);
    ~AddAccountDialog();

signals:
    void addInfo(QString);
};

#endif // ADDACCOUNTDIALOG_H
