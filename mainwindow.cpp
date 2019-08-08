#include "mainwindow.h"
#include "accountdialog.h"
#include <QFile>
#include <QDebug>
#include <QDir>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QStringList t1Header, t2Header;

    // set basic main window
    this->setWindowTitle("BillPay Application");
    this->resize(1440, 800);

    // set main menu
    pMenuBar       = menuBar();
    pMenu1_File    = pMenuBar->addMenu("File");
    pMenu1_Account = pMenuBar->addMenu("Account");
    pMenu1_Help    = pMenuBar->addMenu("Help");
    pOpen          = pMenu1_File->addAction("File");
    pAccount        = pMenu1_Account->addAction("Account");
    pVersion       = pMenu1_Help->addAction("Version");



    // set the widgets on the main window
    CenterWindow = new QWidget;
    t1  = new QTableWidget(0,7);
    t1->horizontalHeader()->setVisible(true);
    t1->verticalHeader()->setVisible(false);
    t1Header << "NO." << "Time" << "Items" << "Money $" << "Paid By" << "Owed To" << "Indepedence";
    t1->setHorizontalHeaderLabels(t1Header);
    t1->setSelectionBehavior(QAbstractItemView::SelectRows);
    t1->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t1->setSelectionMode(QAbstractItemView::SingleSelection);

    t2  = new QTableWidget(1,5);
    t2->horizontalHeader()->setVisible(true);
    t2->verticalHeader()->setVisible(false);
    t2Header << "NO." << "Name" << "Money Out $" << "Money Owed $" << "Balance $";
    t2->setHorizontalHeaderLabels(t2Header);
    t2->setSelectionBehavior(QAbstractItemView::SelectRows);
    t2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t2->setSelectionMode(QAbstractItemView::SingleSelection);

    l1.setText("Bills:");
    l2.setText("Accounts:");

    b1.setText("New");
    b2.setText("Delete");
    b3.setText("Change");
    b4.setText("Calculate");
    b5.setText("Clear!");

    ly1 = new QGridLayout;
    ly1->addWidget(&l1,0,0,1,1);
    ly1->addWidget(&l2,0,7,1,1);
    ly1->addWidget( t1,1,0,5,7);
    ly1->addWidget( t2,1,7,5,5);
    ly1->addWidget(&b1,6,1,1,1);
    ly1->addWidget(&b2,6,3,1,1);
    ly1->addWidget(&b3,6,5,1,1);
    ly1->addWidget(&b4,6,8,1,1);
    ly1->addWidget(&b5,6,10,1,1);
    CenterWindow->setLayout(ly1);
    this->setCentralWidget(CenterWindow);

    connect(pAccount, &QAction::triggered, this, &MainWindow::accountExec);
    connect(&b1, &QPushButton::clicked, this, &MainWindow::newBillExec);
    connect(&b2, &QPushButton::clicked, this, &MainWindow::deleteBillExec);
    connect(&b3, &QPushButton::clicked, this, &MainWindow::changeBillExec);

    // load basic infomation (dataset saving path)
    readBasicInfo("../../../../data/BasicInfo");

    // load account dataset and bill dataset
    current_account = new Account(AccountFileAddress);
    current_bill    = new Bill(OBillFileAddress, SBillFileAddress);

    billStartEnd();
    calculateTotal(bill_start, bill_end);
    updateAccountTable();
    updateBillTable();  updateBillTable();

}

MainWindow::~MainWindow()
{
    current_bill->saveToFile(OBillFileAddress, SBillFileAddress);

    clearTable(t1);
    delete t1;
    delete t2;
    delete ly1;

    delete current_bill;
    delete current_account;

}

// Account
void MainWindow::accountExec()
{
    AccountDialog *dlg;

    dlg = new AccountDialog(MainWindow::AccountFileAddress);
    if (dlg->exec() == QDialog::Accepted){
        delete current_account;
        current_account = new Account(AccountFileAddress);

        // calculate the paid, owed, and balance, and then update the account table
        calculateTotal(bill_start, bill_end);
        updateAccountTable();
    }
    delete dlg;
}


void MainWindow::readBasicInfo(QString path)
{
    QFile f(path);
    QString temp;
    int LineLength, m1;
    char buffer[1024];

    f.open(QIODevice::ReadOnly);

    LineLength = f.readLine(buffer, 1024);
    for (m1=0, temp=""; m1<LineLength; m1++){
        if (buffer[m1]!='\n')   temp = temp + buffer[m1];
    }
    AccountFileAddress = temp;

    LineLength = f.readLine(buffer, 1024);
    for (m1=0, temp=""; m1<LineLength; m1++){
        if (buffer[m1]!='\n')   temp = temp + buffer[m1];
    }
    OBillFileAddress = temp;

    LineLength = f.readLine(buffer, 1024);
    for (m1=0, temp=""; m1<LineLength; m1++){
        if (buffer[m1]!='\n')   temp = temp + buffer[m1];
    }
    SBillFileAddress = temp;

    LineLength = f.readLine(buffer, 1024);
    for (m1=0, BillStartNumber=0; m1<LineLength; m1++){
        if (buffer[m1]!='\n')   BillStartNumber = BillStartNumber * 10 + (buffer[m1]-'0');
    }
}


void MainWindow::updateBillTable()
{
    OneBill          *temp1, *temp2, *host_bill;
    int              count, m1;
    QTableWidgetItem *it;
    QString          temp3;
    Divider          *temp4;


    clearTable(t1);
    if (bill_start == NULL);
    else{
        // read total number of the active bills
        for (count=0, temp1=bill_start; temp1!=NULL; temp1=temp1->next)
            for (temp2=temp1; temp2!=NULL; temp2=temp2->pSubBill, count++);

        t1->setRowCount(count);
        for (m1=0, temp1=bill_start; temp1!=NULL; temp1=temp1->next)
            for (temp2=temp1; temp2!=NULL; temp2=temp2->pSubBill, m1++){
                // set the number
                it = new QTableWidgetItem;
                t1->setItem(m1,0,it);
                it->setText(QString::number(temp2->Number));
                it->setTextColor(QColor(0,0,0));
                it->setBackgroundColor(QColor(255,255,255));

                // set the time
                it = new QTableWidgetItem;
                t1->setItem(m1,1,it);
                it->setText(QString::number(temp2->Time));
                it->setTextColor(QColor(0,0,0));
                it->setBackgroundColor(QColor(255,255,255));

                // find the number of the host bill for a sub-bill
                if (temp2->Independent == false){
                    for (host_bill=temp2; host_bill->pObjBill!=NULL; host_bill=host_bill->pObjBill);
                }

                // set the item
                it = new QTableWidgetItem;
                t1->setItem(m1,2,it);
                if (temp2->Independent == true)     it->setText(temp2->Item);
                else                                it->setText(QString::number(host_bill->Number) + ":\n" + temp2->Item);
                it->setTextColor(QColor(0,0,0));
                it->setBackgroundColor(QColor(255,255,255));

                // set the money
                it = new QTableWidgetItem;
                t1->setItem(m1,3,it);
                it->setText(QString::number(temp2->Money));
                it->setTextColor(QColor(0,0,0));
                it->setBackgroundColor(QColor(255,255,255));

                // set the paid info
                it = new QTableWidgetItem;
                t1->setItem(m1,4,it);
                it->setTextColor(QColor(0,0,0));
                it->setBackgroundColor(QColor(255,255,255));

                if (temp2->PaidHeader != NULL){
                    for(temp3="", temp4=temp2->PaidHeader; temp4!=NULL; temp4=temp4->next){
                        temp3 = temp3 + QString::number(temp4->People) + ":" + current_account->FindByNumber(temp4->People)->Name + "\t$"+QString::number(temp4->Money) + "\n";
                    }
                    it->setText(temp3);
                }

                // set the owed info
                it = new QTableWidgetItem;
                t1->setItem(m1,5,it);
                it->setTextColor(QColor(0,0,0));
                it->setBackgroundColor(QColor(255,255,255));

                if (temp2->OwedHeader != NULL){
                    for(temp3="", temp4=temp2->OwedHeader; temp4!=NULL; temp4=temp4->next){
                        temp3 = temp3 + QString::number(temp4->People) + ":" + current_account->FindByNumber(temp4->People)->Name + "\t$"+QString::number(temp4->Money) + "\n";
                    }
                    it->setText(temp3);
                }

                // set the independence info
                it = new QTableWidgetItem;
                t1->setItem(m1,6,it);
                if (temp2->Independent == true) it->setText("Yes");
                else                            it->setText("No");
                it->setTextColor(QColor(0,0,0));
                it->setBackgroundColor(QColor(255,255,255));

            }
            t1->resizeRowsToContents();
            t1->resizeColumnsToContents();
        }
}


void MainWindow::clearTable(QTableWidget* t)
{
    int row, col, m1, m2;

    row = t->rowCount();
    col = t->columnCount();

    for (m1=0; m1<row; m1++){
        for (m2=0; m2<col; m2++){
            delete t->item(m1, m2);
        }
    }

    t->setRowCount(0);
//    t->setColumnCount(0);
}


void MainWindow::newBillExec()
{
//    qDebug() << "Fuck!";
    OneBill *temp1;

    // create and initialize a new OneBill
    temp1                = new OneBill;
    if (current_bill->tail==NULL)   temp1->Number = 10000001;
    else                            temp1->Number = current_bill->tail->Number+1;
    temp1->Time          = 0;
    temp1->Item          = "";
    temp1->Money         = 0.0;
    temp1->PaidHeader    = NULL;
    temp1->OwedHeader    = NULL;
    temp1->pSubBill      = NULL;
    temp1->pObjBill      = NULL;
    temp1->prev          = NULL;
    temp1->next          = NULL;
    temp1->Independent   = true;

    // create a dialog and send this new OneBill to the dialog for modifying
    dlg1 = new BillEditDialog(temp1, current_account, current_bill);

    // if accept, then link this new OneBill to the tail of the current bill, if not, then delete this new OneBill
    if (dlg1->exec() == QDialog::Accepted){
        if (current_bill->head == NULL){
            current_bill->head       = temp1;
            current_bill->tail       = temp1;
        }
        else{
            current_bill->tail->next = temp1;
            current_bill->tail       = temp1;
        }
    }
    else{
        current_bill->deleteIndependentBill(2, temp1);
    }
    billStartEnd();
    calculateTotal(bill_start, bill_end);
    updateBillTable();
    updateAccountTable();

    delete dlg1;
}


void MainWindow::changeBillExec()
{
    int row, m1;
    OneBill *temp1, *temp2;

    // read the number of the selected row
    row   = t1->currentIndex().row();
    temp1 = current_bill->findByNumber(t1->item(row,0)->text().toInt());

    if (temp1->Independent == true){
        // make a copy of the selected one
        temp2 = current_bill->copyIndependentBill(temp1);

        // send the copyfile to the dialog for modifying
        dlg1 = new BillEditDialog(temp2, current_account, current_bill);

        // if accept, replace the old OneBill with the new one, and then delete the old one, if not, then just delete the new one
        if (dlg1->exec() == QDialog::Accepted){
            current_bill->replaceIndependentBill(temp1, temp2);
            current_bill->deleteIndependentBill(2, temp1);
        }
        else{
            current_bill->deleteIndependentBill(2, temp2);
        }
        delete dlg1;
        billStartEnd();
        calculateTotal(bill_start, bill_end);
        updateBillTable();
        updateAccountTable();
    }
    else{
        QMessageBox *dlg2 = new QMessageBox(QMessageBox::Warning, "Warning",
                                            "You cannot change the sub-bills directly, please change it from its host bill.",
                                            QMessageBox::Ok);
        dlg2->exec();
        delete dlg2;
    }
}


void MainWindow::deleteBillExec()
{
    int row;
    OneBill *temp1;

    // read the number of the selected row
    row   = t1->currentIndex().row();
    temp1 = current_bill->findByNumber(t1->item(row,0)->text().toInt());

    if (temp1->Independent == true){
        // delete the selected independent bill
        current_bill->deleteIndependentBill(1, temp1);
        billStartEnd();
        calculateTotal(bill_start, bill_end);
        updateBillTable();
        updateAccountTable();
    }
    else{
        QMessageBox *dlg2 = new QMessageBox(QMessageBox::Warning, "Warning",
                                            "You cannot delete the sub-bills directly, please delete it from its host bill.",
                                            QMessageBox::Ok);
        dlg2->exec();
        delete dlg2;
    }

}


void MainWindow::calculateTotal(OneBill *start_object, OneBill *end_object)
{
    User    *temp1;
    OneBill *temp2, *temp3;
    Divider *temp4;
    // clear all accounts
    for (temp1=current_account->head; temp1!=NULL; temp1=temp1->next){
        temp1->Owed    = 0.0;
        temp1->Paid    = 0.0;
        temp1->Balance = 0.0;
    }
    // go through each bill, add their values to their corresponding accounts
    for (temp2=start_object; (temp2!=NULL)&&(temp2!=end_object->next); temp2=temp2->next){
        for (temp3=temp2; temp3!=NULL; temp3=temp3->pSubBill){
            // record the paid info
            for (temp4=temp3->PaidHeader; temp4!=NULL; temp4=temp4->next){
                temp1 = current_account->FindByNumber(temp4->People);
                temp1->Paid += temp4->Money;
            }
            // record the owed info
            for (temp4=temp3->OwedHeader; temp4!=NULL; temp4=temp4->next){
                temp1 = current_account->FindByNumber(temp4->People);
                temp1->Owed += temp4->Money;
            }
        }
    }
    // calculate the balance
    for (temp1=current_account->head; temp1!=NULL; temp1=temp1->next)
        temp1->Balance = temp1->Owed - temp1->Paid;
}


void MainWindow::billStartEnd()
{
    OneBill *temp1;

    temp1 = current_bill->findByNumber(BillStartNumber);
    if (temp1 == NULL)      bill_start = current_bill->head;
    else                    bill_start = temp1->next;

    if (bill_start == NULL)      bill_end = NULL;
    else{
        for (bill_end=bill_start; bill_end->next!=NULL; bill_end=bill_end->next);
    }
}


void MainWindow::updateAccountTable()
{
    QTableWidgetItem *it;
    User             *temp1;
    int               m1,    count;

    // clear the account table
    clearTable(t2);
    // get the row number
    for (count=0, temp1=current_account->head; temp1!=NULL; temp1=temp1->next, count++);
    t2->setRowCount(count);
    // set the updated account table
    for (m1=0, temp1=current_account->head; temp1!=NULL; m1++, temp1=temp1->next){
        it = new QTableWidgetItem;
        t2->setItem(m1, 0, it);
        it->setText(QString::number(temp1->Number));
        it->setTextColor(QColor(0,0,0));
        it->setBackgroundColor(QColor(255,255,255));

        it = new QTableWidgetItem;
        t2->setItem(m1, 1, it);
        it->setText(temp1->Name);
        it->setTextColor(QColor(0,0,0));
        it->setBackgroundColor(QColor(255,255,255));

        it = new QTableWidgetItem;
        t2->setItem(m1, 2, it);
        it->setText(QString::number(temp1->Paid));
        it->setTextColor(QColor(0,0,0));
        it->setBackgroundColor(QColor(255,255,255));

        it = new QTableWidgetItem;
        t2->setItem(m1, 3, it);
        it->setText(QString::number(temp1->Owed));
        it->setTextColor(QColor(0,0,0));
        it->setBackgroundColor(QColor(255,255,255));

        it = new QTableWidgetItem;
        t2->setItem(m1, 4, it);
        it->setText(QString::number(temp1->Balance));
        it->setTextColor(QColor(0,0,0));
        it->setBackgroundColor(QColor(255,255,255));
    }
    t2->resizeRowsToContents();
    t2->resizeColumnsToContents();
}












