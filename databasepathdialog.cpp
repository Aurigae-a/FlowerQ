#include "databasepathdialog.h"
#include <QDebug>

DatabasePathDialog::DatabasePathDialog(QString name)
{
    returnPath = "?";
    lMain = new QGridLayout();

    l1.setText(name);

    b1.setText("create a new account set");
    b2.setText("load from existed file");
    g1.addButton(&b1,0);
    g1.addButton(&b2,1);

    p1.setText("Search");
    p2.setText("Accept");
    p3.setText("Cancel");

    lMain->addWidget(&l1, 0, 0, 1, 1);
    lMain->addWidget(&b1, 1, 0, 1, 3);
    lMain->addWidget(&b2, 2, 0, 1, 3);
    lMain->addWidget(&e1, 3, 0, 1, 2);
    lMain->addWidget(&p1, 3, 2, 1, 1);
    lMain->addWidget(&p2, 4, 0, 1, 1);
    lMain->addWidget(&p3, 4, 2, 1, 1);

    void (QButtonGroup::*fcnPointer1) (int) = &QButtonGroup::buttonClicked;
    connect(&g1, fcnPointer1, this, &DatabasePathDialog::updateState);
    connect(&p2, &QPushButton::clicked, this, &DatabasePathDialog::acceptExec);
    connect(&p3, &QPushButton::clicked, this, &DatabasePathDialog::cancelExec);

    g1.button(0)->setChecked(true);
    e1.setEnabled(false);

    this->setLayout(lMain);
}

DatabasePathDialog::~DatabasePathDialog()
{

    delete lMain;
}

void DatabasePathDialog::updateState(int)
{


    if (g1.button(0)->isChecked()) {
        e1.setEnabled(false);
        disconnect(&p1, &QPushButton::clicked, this, &DatabasePathDialog::openPathDialog);
        e1.setText("");
    }
    if (g1.button(1)->isChecked()) {
        e1.setEnabled(true);
        connect(&p1, &QPushButton::clicked, this, &DatabasePathDialog::openPathDialog);
    }
}

void DatabasePathDialog::openPathDialog()
{
    QString path = QFileDialog::getOpenFileName(this, "Load path", "./");
    e1.setText(path);
}

void DatabasePathDialog::acceptExec()
{
    if (g1.button(0)->isChecked()) {
        returnPath = "!";
    }
    if (g1.button(1)->isChecked()) {
        returnPath = e1.text();
    }
    done(QDialog::Accepted);
}

void DatabasePathDialog::cancelExec()
{
    returnPath = "?";
    done(QDialog::Rejected);
}














