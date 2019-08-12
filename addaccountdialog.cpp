#include "addaccountdialog.h"
#include <QDebug>


AddAccountDialog::AddAccountDialog(User *src)
{
    this->setWindowTitle("Account Input");

    tempUser = new User;
    tempUser->Number = src->Number;
    tempUser->Name = src->Name;
    tempUser->isFreezed = src->isFreezed;
    tempUser->Balance = src->Balance;

    l1.setText("Account #");
    l2.setText("Name");

    b1.setText("Accept");
    b2.setText("Cancel");

    e1.setText(QString::number(tempUser->Number));
    e1.setEnabled(false);
    e2.setText(tempUser->Name);

    r1.setText("Freezed");
    r2.setText("Unreezed");
    g1.addButton(&r1,0);
    g1.addButton(&r2,1);
    if (tempUser->isFreezed == true)    r1.setChecked(true);
    else                                r2.setChecked(true);

    ly1 = new QGridLayout(this);
    ly1->addWidget(&l1, 0, 0, 1, 1);
    ly1->addWidget(&l2, 1, 0, 1, 1);
    ly1->addWidget(&e1, 0, 1, 1, 2);
    ly1->addWidget(&e2, 1, 1, 1, 2);
    ly1->addWidget(&r1, 2, 0, 1, 1);
    ly1->addWidget(&r2, 2, 1, 1, 1);
    ly1->addWidget(&b1, 3, 0, 1, 1);
    ly1->addWidget(&b2, 3, 2, 1, 1);

    connect(&b1, &QPushButton::clicked, this, &AddAccountDialog::AcceptExec);
    connect(&b2, &QPushButton::clicked, this, &AddAccountDialog::CancelExec);
    connect(&r1, &QRadioButton::toggled, this, &AddAccountDialog::statusChanged);

}

AddAccountDialog::~AddAccountDialog()
{
    delete tempUser;
    delete ly1;
}


void AddAccountDialog::AcceptExec()
{
    tempUser->Name = e2.text();
    if (r1.isChecked())     tempUser->isFreezed = true;
    else                    tempUser->isFreezed = false;

    done(Accepted);
}

void AddAccountDialog::CancelExec()
{
    done(Rejected);
}

void AddAccountDialog::statusChanged() {
    if (g1.button(0)->isChecked()&&(int(tempUser->Balance*100))!=0) {
        QMessageBox::question(this, "Message", "The seleted user has non-zero balance, cannot freeze this user.", QMessageBox::Ok);
        g1.button(1)->setChecked(true);
    }
}











