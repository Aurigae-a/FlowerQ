#include "addaccountdialog.h"


AddAccountDialog::AddAccountDialog(int cmd, int num, QString nam)
{
    command = cmd;
    Number  = num;
    Name    = nam;

    this->setWindowTitle("Account Input");

    l1.setText("Account #");
    l2.setText("Name");

    b1.setText("Accept");
    b2.setText("Cancel");

    if (command == 1)
    {
        e1.setEnabled(false);
        e1.setText(QString::number(Number+1));
    }
    if (command == 2)
    {
        e1.setEnabled(false);
        e1.setText(QString::number(Number));
        e2.setText(Name);
    }

    ly1 = new QGridLayout(this);
    ly1->addWidget(&l1, 0, 0, 1, 1);
    ly1->addWidget(&l2, 1, 0, 1, 1);
    ly1->addWidget(&e1, 0, 1, 1, 2);
    ly1->addWidget(&e2, 1, 1, 1, 2);
    ly1->addWidget(&b1, 2, 0, 1, 1);
    ly1->addWidget(&b2, 2, 2, 1, 1);

    connect(&b1, &QPushButton::clicked, this, &AddAccountDialog::AcceptExec);
    connect(&b2, &QPushButton::clicked, this, &AddAccountDialog::CancelExec);

}

AddAccountDialog::~AddAccountDialog()
{
    delete ly1;
}


void AddAccountDialog::AcceptExec()
{
    QString temp;

    temp = e2.text();
    emit addInfo(temp);
    done(Accepted);
}

void AddAccountDialog::CancelExec()
{
    done(Rejected);
}

