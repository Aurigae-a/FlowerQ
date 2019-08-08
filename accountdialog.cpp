#include "accountdialog.h"
#include <QHeaderView>
#include <QDebug>
#include <QMessageBox>


AccountDialog::AccountDialog(QString path)
{
//    qDebug() << path;
    receiveTemp1 = "";

    current_account = new Account(path);
//    qDebug() << "aha!";

    this->setWindowTitle("Account Manager");

    ly1 = new QGridLayout(this);

    t1  = new QTableWidget(0, 2);
    t1->horizontalHeader()->setVisible(false);
    t1->verticalHeader()->setVisible(false);
    t1->setSelectionBehavior(QAbstractItemView::SelectRows);
    t1->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t1->setSelectionMode(QAbstractItemView::SingleSelection);

    l1.setText("Account #");
    l2.setText("Name");

    b1.setText("Save");
    b2.setText("Cancel");
    b3.setText("Add");
    b4.setText("Delete");
    b5.setText("Change");

    ly1->addWidget(&l1, 0, 0, 1, 1);
    ly1->addWidget(&l2, 0, 1, 1, 1);
    ly1->addWidget( t1, 1, 0, 5, 2);
    ly1->addWidget(&b3, 2, 2, 1, 1);
    ly1->addWidget(&b4, 3, 2, 1, 1);
    ly1->addWidget(&b5, 4, 2, 1, 1);
    ly1->addWidget(&b1, 6, 0, 1, 1);
    ly1->addWidget(&b2, 6, 1, 1, 1);

    connect(&b1, &QPushButton::clicked, this, &AccountDialog::SaveExec);
    connect(&b2, &QPushButton::clicked, this, &AccountDialog::CancelExec);
    connect(&b3, &QPushButton::clicked, this, &AccountDialog::AddExec);
    connect(&b4, &QPushButton::clicked, this, &AccountDialog::DeleteExec);
    connect(&b5, &QPushButton::clicked, this, &AccountDialog::ChangeExec);

//    qDebug() << "aha!";

    addSignal = &AddAccountDialog::addInfo;
//    connect(dlg, addSignal, this, &AccountDialog::receiveFcn1);

    UpdateTable();

}


AccountDialog::~AccountDialog()
{
    int m1;

    if (t1->rowCount()!=0)
    {
        for (m1=0; m1<t1->rowCount(); m1++)
        {
            delete t1->item(m1,0);
            delete t1->item(m1,1);
        }
    }
    delete t1;
    delete current_account;
}


void AccountDialog::SaveExec()
{
    current_account->SaveToFile(current_account->SavingFilePath);
    done(Accepted);
}


void AccountDialog::CancelExec()
{
    done(Rejected);
}


void AccountDialog::AddExec()
{
    User *temp;

//    qDebug() << current_account->tail->Name;

    dlg = new AddAccountDialog(1, current_account->tail->Number, "");
    connect(dlg, addSignal, this, &AccountDialog::receiveFcn1);
    if (dlg->exec() == QDialog::Accepted)
    {
        disconnect(dlg, addSignal, this, &AccountDialog::receiveFcn1);
        temp = new User;
        temp->Paid                  = 0.0;
        temp->Owed                  = 0.0;
        temp->Balance               = 0.0;
        temp->Name                  = receiveTemp1;
        temp->Number                = current_account->tail->Number+1;
        current_account->tail->next = temp;
        temp->prev                  = current_account->tail;
        temp->next                  = NULL;
        current_account->tail       = temp;

        current_account->total_num += 1;

        UpdateTable();
    }
    delete dlg;
//    qDebug() << current_account->tail->Name;
}


void AccountDialog::DeleteExec()
{
    int  current_row, m1;
    User *temp;

    if (current_account->total_num <= 1)
    {
        QMessageBox msg(QMessageBox::Warning,"Deleting Error",
                        "Cannot delete this account, because this is the last account.",
                        QMessageBox::Ok);
        msg.exec();
    }
    else
    {
        current_row = t1->currentIndex().row();
//        qDebug() << t1->currentIndex().row();

        if (current_row == 0)
        {
//            qDebug() << "first";
            temp                        = current_account->head;
            current_account->head       = current_account->head->next;
            current_account->head->prev = NULL;
        }
        else
        {
            for (m1=0, temp=current_account->head;
                 m1<current_row;
                 m1++, temp=temp->next);

//            qDebug() << temp->Name;
            if (current_row == current_account->total_num-1)
            {
//                qDebug() << "last";
                current_account->tail       = current_account->tail->prev;
                current_account->tail->next = NULL;
            }
            else
            {
//                qDebug() << "middle";
                temp->prev->next = temp->next;
                temp->next->prev = temp->prev;
            }

        }
        delete temp;
        current_account->total_num -= 1;
    }

    UpdateTable();

}


void AccountDialog::ChangeExec()
{
    int current_row, m1;
    User *temp;

    current_row = t1->currentIndex().row();
    for (m1=0, temp=current_account->head;
         m1<current_row;
         m1++, temp=temp->next);

    dlg = new AddAccountDialog(2, temp->Number, temp->Name);
    connect(dlg, addSignal, this, &AccountDialog::receiveFcn1);
    if (dlg->exec() == QDialog::Accepted)
    {
        disconnect(dlg, addSignal, this, &AccountDialog::receiveFcn1);
        temp->Name = receiveTemp1;
        UpdateTable();
    }
    delete dlg;

}


void AccountDialog::UpdateTable()
{
    int m1;
    User *temp;
    QTableWidgetItem *it;

    if (t1->rowCount() != 0)
    {
        for (m1=0; m1<t1->rowCount(); m1++)
        {
            delete t1->item(m1,0);
            delete t1->item(m1,1);
        }
    }

    t1->setRowCount(current_account->total_num);
    t1->setColumnCount(2);


//    qDebug() << current_account->total_num;

    for (m1=0, temp=current_account->head; temp!=NULL; m1++, temp=temp->next)
    {
//        t1->item(m1,0)->setText(QString(temp->Number));
//        qDebug() << temp->Number;
        it = new QTableWidgetItem;
        t1->setItem(m1,0,it);
        it->setText(QString::number(temp->Number));
        it->setTextColor(QColor(0,0,0));
        it->setBackgroundColor(QColor(255,255,255));

        it = new QTableWidgetItem;
        t1->setItem(m1,1,it);
        it->setText(temp->Name);
        it->setTextColor(QColor(0,0,0));
        it->setBackgroundColor(QColor(255,255,255));
    }

}


void AccountDialog::receiveFcn1(QString Input){
    receiveTemp1 = Input;
}
















