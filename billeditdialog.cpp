#include "billeditdialog.h"
#include <QDebug>

BillEditDialog::BillEditDialog(OneBill *input_bill, Account *input_account, Bill *input_bill_book)
{
    object_bill  = input_bill;
    account      = input_account;
    current_bill = input_bill_book;

    head1             = nullptr;
    tail1             = nullptr;
    head2             = nullptr;
    tail2             = nullptr;

    createItems();
    adjustToBillData();
    updateTable();

    connect(&e4, &QLineEdit::editingFinished, this, &BillEditDialog::slotMoneyEdit);
    connect(&b1, &QPushButton::clicked, this, &BillEditDialog::AcceptExec);
    connect(&b2, &QPushButton::clicked, this, &BillEditDialog::CancelExec);
    connect(&b3, &QPushButton::clicked, this, &BillEditDialog::paidDefault);
    connect(&b4, &QPushButton::clicked, this, &BillEditDialog::owedDefault);
    connect(&b5, &QPushButton::clicked, this, &BillEditDialog::addSubBill);
    connect(&b6, &QPushButton::clicked, this, &BillEditDialog::deleteSubBill);
    connect(&b7, &QPushButton::clicked, this, &BillEditDialog::changeSubBill);
}


BillEditDialog::~BillEditDialog()
{
    clearTable();
    delete t1;

    delete ly2;
    delete ly3;
    delete ly1;

    deleteCheckBox(head1, tail1);
    deleteCheckBox(head2, tail2);

}


void BillEditDialog::AcceptExec()
{
    Divider      *temp1, *temp4=nullptr;
    CheckBoxList *temp2;
    User         *temp3;

    QMessageBox  *dlg;

    if (!checkPaid()){
        dlg = new QMessageBox(QMessageBox::Warning, "Warning", "Paid money not matched", QMessageBox::Ok);
        dlg->exec();
        delete dlg;
    }
    else if (!checkOwed()){
        dlg = new QMessageBox(QMessageBox::Warning, "Warning", "Owed money not matched", QMessageBox::Ok);
        dlg->exec();
        delete dlg;
    }
    else{
        object_bill->Time  = e2.text().toInt();
        object_bill->Item  = e3.text();
        object_bill->Money = e4.text().toDouble();

        for (temp1=object_bill->PaidHeader; temp1!=nullptr; ){
            temp4 = temp1;
            temp1 = temp1->next;
            delete temp4;
        }
        object_bill->PaidHeader = nullptr;
        for (temp2=head1, temp3=account->head; temp2!=nullptr; temp3=temp3->next){
            if (temp3->isFreezed)       continue;

            if (temp2->box_state == true){
                temp1         = new Divider;
                temp1->People = temp3->Number;
                temp1->Money  = temp2->edit->text().toDouble();
                temp1->next   = nullptr;

                if (object_bill->PaidHeader == nullptr){
                    object_bill->PaidHeader = temp1;
                    temp4                   = temp1;
                }
                else{
                    temp4->next             = temp1;
                    temp4                   = temp1;
                }
            }

            temp2=temp2->next;
        }

        for (temp1=object_bill->OwedHeader; temp1!=nullptr; ){
            temp4 = temp1;
            temp1 = temp1->next;
            delete temp4;
        }
        object_bill->OwedHeader = nullptr;
        for (temp2=head2, temp3=account->head; temp2!=nullptr; temp3=temp3->next){
            if (temp3->isFreezed)       continue;

            if (temp2->box_state == true){
                temp1         = new Divider;
                temp1->People = temp3->Number;
                temp1->Money  = temp2->edit->text().toDouble();
                temp1->next   = nullptr;

                if (object_bill->OwedHeader == nullptr){
                    object_bill->OwedHeader = temp1;
                    temp4                   = temp1;
                }
                else{
                    temp4->next             = temp1;
                    temp4                   = temp1;
                }
            }

            temp2=temp2->next;
        }

        done(QDialog::Accepted);
    }

}


void BillEditDialog::CancelExec()
{
    done(QDialog::Rejected);
}


void BillEditDialog::newCheckBox(CheckBoxList* &head, CheckBoxList* &tail, QGridLayout *ly)
{
    CheckBoxList *temp1;
    User         *temp2;
    int          m1;

    // create a check box and an editor for each account
    for (temp2=account->head, m1=0; temp2!=nullptr; temp2=temp2->next, m1++){
        if (temp2->isFreezed)       continue;

        temp1 = new CheckBoxList;
        if (head == nullptr) {
            temp1->next = nullptr;
            head        = temp1;
            tail        = temp1;
        }
        else{
            temp1->next = nullptr;
            tail->next  = temp1;
            tail        = temp1;
        }

        temp1->box       = new QCheckBox(temp2->Name);
        temp1->edit      = new QLineEdit;
        temp1->box_state = false;
        ly->addWidget(temp1->box,  m1, 0, 1, 1);
        ly->addWidget(temp1->edit, m1, 1, 1, 1);
    }
}


void BillEditDialog::deleteCheckBox(CheckBoxList* &head, CheckBoxList* &tail)
{
    CheckBoxList *temp1, *temp2;

    // disconnect the checkboxes with their slot function
    connectingCheckBox(false, head);

    for (temp1=head; temp1!=nullptr;){
        temp2 = temp1;
        delete temp2->box;
        delete temp2->edit;
        temp1 = temp1->next;
        delete temp2;
    }

    head = nullptr;
    tail = nullptr;
}


void BillEditDialog::slotCheckBox(int)
{
    updateCheckBox(head1);
    updateCheckBox(head2);

}


void BillEditDialog::updateCheckBox(CheckBoxList *head)
{
    CheckBoxList *temp1;

    for (temp1=head; temp1!=nullptr; temp1=temp1->next){
        if (temp1->box->checkState() == Qt::Checked){
            temp1->edit->setEnabled(true);
            temp1->box_state = true;
        }
        else{
            if (temp1->box_state == true){
                temp1->edit->setText(QString::number(0));
            }
            temp1->edit->setEnabled(false);
            temp1->box_state = false;
        }
    }
/*
    for (temp1=head; temp1!=NULL; temp1=temp1->next){
        qDebug() << temp1->box_state;
    }
*/
}


void BillEditDialog::createItems()
{
    QStringList t1Header;

    ly1 = new QGridLayout;
    ly2 = new QGridLayout;
    ly3 = new QGridLayout;
    t1  = new QTableWidget;

    newCheckBox(head1, tail1, ly2);
    newCheckBox(head2, tail2, ly3);

    l1.setText("NO.");
    l2.setText("Time");
    l3.setText("Items");
    l4.setText("Money $");
    l5.setText("Paid By");
    l6.setText("Owed to");
    l7.setText("Sub Bill");

    b1.setText("Accept");
    b2.setText("Cancel");
    b3.setText("Default");
    b4.setText("Default");
    b5.setText("Add");
    b6.setText("Delete");
    b7.setText("Change");

    e1.setEnabled(false);
    e6.setEnabled(false);

    t1->setColumnCount(3);
    t1->horizontalHeader()->setVisible(true);
    t1->verticalHeader()->setVisible(false);
    t1Header << "Sub-bill NO." << "Items" << "Money $";
    t1->setHorizontalHeaderLabels(t1Header);
    t1->setSelectionBehavior(QAbstractItemView::SelectRows);
    t1->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t1->setSelectionMode(QAbstractItemView::SingleSelection);

    ly1->addWidget(&l1, 0, 0, 1, 1);
    ly1->addWidget(&e1, 0, 1, 1, 1);
    ly1->addWidget(&l2, 1, 0, 1, 1);
    ly1->addWidget(&e2, 1, 1, 1, 1);
    ly1->addWidget(&l3, 2, 0, 1, 1);
    ly1->addWidget(&e3, 2, 1, 1, 1);
    ly1->addWidget(&l4, 3, 0, 1, 1);
    ly1->addWidget(&e4, 3, 1, 1, 1);

    if (object_bill->Independent == true){
        l8.setText("Remained $");

        ly1->addWidget(&l5, 4, 0, 1, 1);
        ly1->addLayout(ly2, 4, 1, 3, 2);
        ly1->addWidget(&b3, 6, 3, 1, 1);

        ly1->addWidget(&l7, 7, 0, 1, 1);
        ly1->addWidget(t1,  7, 1, 3, 2);
        ly1->addWidget(&b5, 7, 3, 1, 1);
        ly1->addWidget(&b6, 8, 3, 1, 1);
        ly1->addWidget(&b7, 9, 3, 1, 1);

        ly1->addWidget(&l8, 10, 0, 1, 1);
        ly1->addWidget(&e6, 10, 1, 1, 1);

        ly1->addWidget(&l6, 11, 0, 1, 1);
        ly1->addLayout(ly3, 11, 1, 3, 2);
        ly1->addWidget(&b4, 13, 3, 1, 1);

        ly1->addWidget(&b1, 14, 0, 1, 1);
        ly1->addWidget(&b2, 14, 2, 1, 1);

    }
    else{
        l8.setText("Belongs to ");
        ly1->addWidget(&l8, 4, 0, 1, 1);
        ly1->addWidget(&e6, 4, 1, 1, 1);

        ly1->addWidget(&l6, 5, 0, 1, 1);
        ly1->addLayout(ly3, 5, 1, 3, 2);
        ly1->addWidget(&b4, 7, 3, 1, 1);

        ly1->addWidget(&b1, 8, 0, 1, 1);
        ly1->addWidget(&b2, 8, 2, 1, 1);
    }



    this->setLayout(ly1);

}


void BillEditDialog::adjustToBillData()
{
    Divider         *temp1;
    CheckBoxList    *temp2;
    User            *temp3;
    OneBill         *temp5;
    int             count, m1;
    double          remained;

    QTableWidgetItem *it;

    e1.setText(QString::number(object_bill->Number));
    e2.setText(QString::number(object_bill->Time));
    e3.setText(object_bill->Item);
    e4.setText(QString::number(object_bill->Money));

    // record the paid info to the CheckBoxList
    for (temp1=object_bill->PaidHeader; temp1!=nullptr; temp1=temp1->next){
        for (temp2=head1, temp3=account->head; temp3!=nullptr; temp3=temp3->next){
            if (temp3->isFreezed)       continue;

            if (temp3->Number == temp1->People){
                temp2->box_state = true;
                temp2->edit->setText(QString::number(temp1->Money));
                break;
            }

            temp2 = temp2->next;
        }
    }
    // update the CheckBox enability by its info
    for (temp2=head1; temp2!=nullptr; temp2=temp2->next){
        if (temp2->box_state == false){
            temp2->box->setCheckState(Qt::Unchecked);
            temp2->edit->setEnabled(false);
        }
        else{
            temp2->box->setCheckState(Qt::Checked);
            temp2->edit->setEnabled(true);
        }
    }
    connectingCheckBox(true, head1);

    // record the owed info to the CheckBoxList
    for (temp1=object_bill->OwedHeader; temp1!=nullptr; temp1=temp1->next){
        for (temp2=head2, temp3=account->head; temp3!=nullptr; temp3=temp3->next){
            if (temp3->isFreezed)       continue;

            if (temp3->Number == temp1->People){
                temp2->box_state = true;
                temp2->edit->setText(QString::number(temp1->Money));
                break;
            }

            temp2=temp2->next;
        }
    }
    // update the CheckBox enability by its info
    for (temp2=head2; temp2!=nullptr; temp2=temp2->next){
        if (temp2->box_state == false){
            temp2->box->setCheckState(Qt::Unchecked);
            temp2->edit->setEnabled(false);
        }
        else{
            temp2->box->setCheckState(Qt::Checked);
            temp2->edit->setEnabled(true);
        }
    }
    connectingCheckBox(true, head2);

    // update the sub-bills info
    if (object_bill->Independent == true){

        // get the total number of the sub-bills
        for (count=0, temp5=object_bill->pSubBill; temp5!=nullptr; count++, temp5=temp5->pSubBill);
        t1->setRowCount(count);

        for (m1=0, temp5=object_bill->pSubBill; temp5!=nullptr; m1++, temp5=temp5->next){
            // update the bill number
            it = new QTableWidgetItem;
            it->setText(QString::number(temp5->Number));
            t1->setItem(m1, 0, it);
            it->setTextColor(QColor(0,0,0));
            it->setBackgroundColor(QColor(255,255,255));

            // update the bill item
            it = new QTableWidgetItem;
            it->setText(temp5->Item);
            t1->setItem(m1, 1, it);
            it->setTextColor(QColor(0,0,0));
            it->setBackgroundColor(QColor(255,255,255));

            //update the bill money
            it = new QTableWidgetItem;
            it->setText(QString::number(temp5->Money));
            t1->setItem(m1, 2, it);
            it->setTextColor(QColor(0,0,0));
            it->setBackgroundColor(QColor(255,255,255));
        }

        t1->resizeRowsToContents();
        t1->resizeColumnsToContents();

        remained = calculateRemained();
        e6.setText(QString::number(remained));
    }
    else{
        // set the host bill number
        e6.setText(QString::number(int(object_bill->Number/100)));
    }
}


void BillEditDialog::connectingCheckBox(bool command, CheckBoxList *head)
{
    CheckBoxList *temp1;

    if (command == true){
        for (temp1=head; temp1!=nullptr; temp1=temp1->next)
            connect(temp1->box, &QCheckBox::stateChanged, this, &BillEditDialog::slotCheckBox);
    }
    if (command == false){
        for (temp1=head; temp1!=nullptr; temp1=temp1->next)
            disconnect(temp1->box, &QCheckBox::stateChanged, this, &BillEditDialog::slotCheckBox);
    }
}


void BillEditDialog::clearTable()
{
    int m1, m2;

    for (m1=0; m1<t1->rowCount(); m1++)
        for (m2=0; m2<t1->columnCount(); m2++){
            if (t1->item(m1, m2) != nullptr)   delete t1->item(m1, m2);
        }
    t1->setRowCount(0);
}


double BillEditDialog::calculateRemained()
{
    double   temp1;
    OneBill *temp2;

    for (temp1=e4.text().toDouble(), temp2=object_bill->pSubBill; temp2!=nullptr; temp2=temp2->pSubBill)
        temp1 = temp1 - temp2->Money;

    return temp1;
}


void BillEditDialog::slotMoneyEdit()
{

    if (object_bill->Independent == true)
        e6.setText(QString::number(calculateRemained()));
}


void BillEditDialog::addSubBill()
{
    OneBill         *temp1;
    BillEditDialog  *temp2;
    OneBill         *tail;

    // find the tail of the sub- bills
    for (tail=object_bill; tail->pSubBill!=nullptr; tail=tail->pSubBill);

    // make a new and blank sub-bill
    temp1                = new OneBill;

    // set the sub-bill number
    if (tail->pObjBill == nullptr)     temp1->Number = tail->Number*100 + 1;
    else                               temp1->Number = tail->Number     + 1;
    // set the rest basic info
    temp1->Time          = e2.text().toInt();
    temp1->Item          = "";
    temp1->Money         = 0.0;
    temp1->Independent   = false;
    temp1->enable        = object_bill->enable;
    // initialize the pointers
    temp1->PaidHeader    = nullptr;
    temp1->OwedHeader    = nullptr;
    temp1->pSubBill      = nullptr;
    temp1->pObjBill      = nullptr;
    temp1->prev          = nullptr;
    temp1->next          = nullptr;

    // create a new dialog for modifying the new sub-bill
    temp2 = new BillEditDialog(temp1, account, current_bill);
    if(temp2->exec() == QDialog::Accepted){
        tail->pSubBill  = temp1;
        temp1->pObjBill = tail;
    }
    else{
        current_bill->deleteSubBill(2, temp1);
    }
    updateTable();
    slotMoneyEdit();

    delete temp2;
}


void BillEditDialog::updateTable()
{
    int              m1;
    OneBill          *temp1;
    QTableWidgetItem *it;


    clearTable();
    for (m1=0, temp1=object_bill->pSubBill; temp1!=nullptr; m1++, temp1=temp1->pSubBill);
    t1->setRowCount(m1);
    for (m1=0, temp1=object_bill->pSubBill; temp1!=nullptr; m1++, temp1=temp1->pSubBill){
//        qDebug() << current_bill->FindByNumber(temp1->BillNO);
        it = new QTableWidgetItem;
        t1->setItem(m1, 0, it);
        it->setText(QString::number(temp1->Number));
        it->setTextColor(QColor(0,0,0));
        it->setBackgroundColor(QColor(255,255,255));

//        qDebug() << "World";
        it = new QTableWidgetItem;
        t1->setItem(m1, 1, it);
        it->setText(temp1->Item);
        it->setTextColor(QColor(0,0,0));
        it->setBackgroundColor(QColor(255,255,255));

        it = new QTableWidgetItem;
        t1->setItem(m1, 2, it);
        it->setText(QString::number(temp1->Money));
        it->setTextColor(QColor(0,0,0));
        it->setBackgroundColor(QColor(255,255,255));

    }


}


bool BillEditDialog::checkPaid()
{
    float        total, sum;
    CheckBoxList *temp1;

    if (object_bill->Independent == false){
        return true;
    }
    else{
        total =  e4.text().toFloat();;

        for (sum = 0.0, temp1=head1; temp1!=nullptr; temp1=temp1->next){
            if (temp1->box_state == true){
                sum += temp1->edit->text().toFloat();
            }
        }
        if (int((sum-total)*100) == 0)          return true;
        else                                    return false;
    }

}


bool BillEditDialog::checkOwed()
{
    double        total, sum;
    CheckBoxList *temp1;

    if (object_bill->Independent == true){
        total = calculateRemained();
    }
    else{
        total = e4.text().toDouble();
    }


    for (sum = 0.0, temp1=head2; temp1!=nullptr; temp1=temp1->next){
//        qDebug() << temp1->money;
        if (temp1->box_state == true){
            sum += temp1->edit->text().toDouble();
        }
    }
    if (int((sum-total)*100) == 0)          return true;
    else                                    return false;
}


void BillEditDialog::changeSubBill()
{
    OneBill         *temp1, *original;
    BillEditDialog  *temp2;
    int              m1, row;

    if (object_bill->pSubBill != nullptr){
        // find the currently selected sub- bills
        row = t1->currentIndex().row();
        for (m1=0, original=object_bill->pSubBill; m1<row; m1++, original=original->pSubBill);

        // copy the original one to a new identical one
        temp1 = current_bill->copySubBill(original);

        // create a new dialog for modifying the new sub-bill
        temp2 = new BillEditDialog(temp1, account, current_bill);
        // if accepted, replace the original one with the new modified one, and then delete the old one, if not, just delete the new one
        if(temp2->exec() == QDialog::Accepted){
            current_bill->replaceSubBill(original, temp1);
            current_bill->deleteSubBill(2, original);
        }
        else{
            current_bill->deleteSubBill(2, temp1);
        }
        updateTable();
        slotMoneyEdit();

        delete temp2;
    }

}


void BillEditDialog::deleteSubBill()
{
    OneBill         *original;
    int              m1, row;

    if (object_bill->pSubBill != nullptr){
        // find the currently selected sub- bills
        row = t1->currentIndex().row();
        for (m1=0, original=object_bill->pSubBill; m1<row; m1++, original=original->pSubBill);

        // delete the selected sub-bill
        current_bill->deleteSubBill(1, original);

        updateTable();
        slotMoneyEdit();
    }
}


void BillEditDialog::defaultCalculate(double total, CheckBoxList *targetStart) {

    int remainedPeopleNumber=0, m1;
    double option1, option2;
    CheckBoxList *temp1;

    for (temp1=targetStart; temp1!=nullptr; temp1=temp1->next)
        if (temp1->box->isChecked()) {
            if (int(temp1->edit->text().toDouble()*100)==0)     remainedPeopleNumber += 1;
            else    total -= temp1->edit->text().toDouble();
        }
    option1 = double(int(total/remainedPeopleNumber*100))/100;
    option2 = total - option1 * (remainedPeopleNumber-1);

    for (m1=0, temp1=targetStart; temp1!=nullptr; temp1=temp1->next)
        if ((temp1->box->isChecked())&&(int(temp1->edit->text().toDouble()*100)==0)) {
            if (m1==(remainedPeopleNumber-1))       temp1->edit->setText(QString::number(option2));
            else                                    temp1->edit->setText(QString::number(option1));
            m1++;
        }
}


void BillEditDialog::paidDefault() {
    defaultCalculate(e4.text().toDouble(), head1);
}


void BillEditDialog::owedDefault() {
    if (object_bill->Independent == false)  defaultCalculate(e4.text().toDouble(), head2);
    else                                    defaultCalculate(e6.text().toDouble(), head2);
}












