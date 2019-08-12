#include "calculatedialog.h"
#include <QDebug>

CalculateDialog::CalculateDialog(int cmd, Account *inputAccount, Bill *inputBill) {
    QStringList head1;

    // cmd=0: 计算； cmd=1:清帐
    currentBill = inputBill;
    currentAccount = inputAccount;
    cartHead = nullptr;
    msHead = nullptr;

    this->resize(550, 650);
    if (cmd == 0) {
        l1.setText("Calculation Section\n\nUncleared bills");
        b2.setText("Finish");
    }
    if (cmd == 1) {
        l1.setText("Clear Section\n\nUncleared bills");
        b2.setText("Cancel");
    }
    b1.setText("Calculate");
    c1.setText("Pick aLl");

    t1 = new QTableWidget(0,5);
    head1 << "NO." << "Time" << "Contents" << "Money" << "Pick";
    t1->horizontalHeader()->setVisible(true);
    t1->setHorizontalHeaderLabels(head1);
    t1->verticalHeader()->setVisible(false);
    t1->setSelectionMode(QAbstractItemView::SingleSelection);
    t1->setSelectionBehavior(QAbstractItemView::SelectRows);
    t1->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t1->setColumnWidth(0,80);
    t1->setColumnWidth(1,80);
    t1->setColumnWidth(2,200);
    t1->setColumnWidth(3,60);
    t1->setColumnWidth(4,40);
    initialTable();

    ly1 = new QGridLayout();
    ly1->addWidget(&l1, 0, 0, 1, 1);
    ly1->addWidget(&c1, 0, 4, 1, 1);
    ly1->addWidget( t1, 1, 0, 4, 5);
    ly1->addWidget(&b1, 5, 0, 1, 1);
    ly1->addWidget(&b2, 5, 4, 1, 1);

    connect(&b1, &QPushButton::clicked, this, &CalculateDialog::calculateExec);
    connect(&b2, &QPushButton::clicked, this, &CalculateDialog::finishExec);
    connect(&c1, &QAbstractButton::clicked, this, &CalculateDialog::seleteAllExec);
    if (cmd == 1) {
        b3.setText("clear");
        ly1->addWidget(&b3, 5, 2, 1, 1);
        connect(&b3, &QPushButton::clicked, this, &CalculateDialog::clearExec);
    }

    this->setLayout(ly1);
}


CalculateDialog::~CalculateDialog() {
    CartList *temp1, *temp2;
    MoneyStream *temp3, *temp4;

    for (temp3=msHead; temp3!=nullptr;) {
        temp4 = temp3;
        temp3 = temp3->next;
        delete temp4;
    }
    msHead = nullptr;

    for (temp1=cartHead; temp1!=nullptr;) {
        temp2 = temp1;
        temp1 = temp1->next;
        disconnect(temp2->box, &QCheckBox::stateChanged, this, &CalculateDialog::examineSeleteAll);
        delete temp2->box;
        delete temp2;
    }
    delete tempAccount;

    clearTable();
    delete t1;
    delete ly1;
}


void CalculateDialog::clearTable() {
    int m1, m2, row, col;

    row = t1->rowCount();
    col = t1->columnCount();

    for (m1=0; m1<row; m1++)
        for (m2=0; m2<col-1; m2++) {
            delete t1->item(m1,m2);
        }
}


void CalculateDialog::initialTable() {
    OneBill *temp1;
    CartList *temp2, *tail=nullptr;
    QTableWidgetItem *it;
    int m1;

    // 将账簿中的有效单复制到CartList上面
    for (temp1=currentBill->head, totalBill=0; temp1!=nullptr; temp1=temp1->next)
        if (temp1->enable == true) {
            temp2 = new CartList;
            temp2->box = new QCheckBox();
            temp2->billptr = temp1;
            temp2->next = nullptr;
            if (cartHead == nullptr) {
                cartHead = tail = temp2;
            }
            else {
                tail->next = temp2;
                tail = temp2;
            }
            connect(temp2->box, &QCheckBox::stateChanged, this, &CalculateDialog::examineSeleteAll);
            totalBill += 1;
        }

    // 将未被冻结的用户复制到tempAccount上面
    tempAccount = new Account();
    tempAccount->copy(currentAccount);

    // 初始化表格
    t1->setRowCount(totalBill);
    for (m1=0, temp2=cartHead; m1<totalBill; m1++, temp2=temp2->next) {
        // 第一列：编号
        it = new QTableWidgetItem();
        t1->setItem(m1,0,it);
        it->setText(QString::number(temp2->billptr->Number));

        // 第2列：时间
        it = new QTableWidgetItem();
        t1->setItem(m1,1,it);
        it->setText(QString::number(temp2->billptr->Time));

        // 第3列：内容
        it = new QTableWidgetItem();
        t1->setItem(m1,2,it);
        it->setText(temp2->billptr->Item);

        // 第4列：金额
        it = new QTableWidgetItem();
        t1->setItem(m1,3,it);
        it->setText(QString::number(temp2->billptr->Money));

        // 第5列：复选框
        t1->setCellWidget(m1,4,temp2->box);
    }
}


void CalculateDialog::finishExec() {
    done(QDialog::Accepted);
}


void CalculateDialog::calculate() {
    User *temp1;
    CartList *temp5;
    OneBill *temp2, *temp3;
    Divider *temp4;
    int positiveNumber, negativeNumber, m1, m2;
    double cmp1, cmp2, minValue;
    User **positiveArray, **negativeArray;
    MoneyStream *temp6, *temp7=nullptr;

    // 统计各个账户上面的资金流入与支出
    for (temp1=tempAccount->head; temp1!=nullptr; temp1=temp1->next) {
        temp1->Owed = temp1->Paid = temp1->Balance = 0;
    }
    // go through each bill, add their values to their corresponding accounts
    for (temp5=cartHead; temp5!=nullptr; temp5=temp5->next){
        if (!temp5->box->isChecked())   continue;
        temp2 = temp5->billptr;
        for (temp3=temp2; temp3!=nullptr; temp3=temp3->pSubBill){
            // record the paid info
            for (temp4=temp3->PaidHeader; temp4!=nullptr; temp4=temp4->next){
                temp1 = tempAccount->FindByNumber(temp4->People);
                if (temp1==nullptr) {
                    QMessageBox::warning(this,"Calculation Error",
                                         "Cannot find the user from the current account, please check whether the account matches with the bill",
                                         QMessageBox::Ok);
                    return;
                }
                temp1->Paid += temp4->Money;
            }
            // record the owed info
            for (temp4=temp3->OwedHeader; temp4!=nullptr; temp4=temp4->next){
                temp1 = tempAccount->FindByNumber(temp4->People);
                if (temp1==nullptr) {
                    QMessageBox::warning(this,"Calculation Error",
                                         "Cannot find the user from the current account, please check whether the account matches with the bill",
                                         QMessageBox::Ok);
                    return;
                }
                temp1->Owed += temp4->Money;
            }
        }
    }
    // calculate the balance
    for (temp1=tempAccount->head; temp1!=nullptr; temp1=temp1->next)        temp1->Balance = temp1->Owed - temp1->Paid;

    // 将当前用户册中的用户按照Balance的正负分到两个数组中，并统计个数
    positiveArray = new User*[tempAccount->total_num];
    negativeArray = new User*[tempAccount->total_num];
    for (positiveNumber=0, negativeNumber=0, temp1=tempAccount->head; temp1!=nullptr; temp1=temp1->next) {
        if (int(temp1->Balance*100)>0) {     // 拥有正的Balance的用户
            *(positiveArray + positiveNumber) = temp1;
            positiveNumber += 1;
        }
        else if (int(temp1->Balance*100)<0){ // 拥有负的Balance的用户
            *(negativeArray + negativeNumber) = temp1;
            negativeNumber += 1;
        }
    }

    // 排序，正的单从小到大排列，负的单按绝对值从大到小排列
    for (m1=0; m1<(positiveNumber-1); m1++)
        for (m2=positiveNumber-1; m2>m1; m2--) {
            cmp1 = (*(positiveArray+m1))->Balance;
            cmp2 = (*(positiveArray+m2))->Balance;
            if (int(cmp2*100) < int(cmp1*100)) {
                temp1 = *(positiveArray+m1);
                *(positiveArray+m1) = *(positiveArray+m2);
                *(positiveArray+m2) = temp1;
            }
        }
    for (m1=0; m1<(negativeNumber-1); m1++)
        for (m2=negativeNumber-1; m2>m1; m2--) {
            cmp1 = (*(negativeArray+m1))->Balance;
            cmp2 = (*(negativeArray+m2))->Balance;
            if (int(cmp2*100) < int(cmp1*100)) {
                temp1 = *(negativeArray+m1);
                *(negativeArray+m1) = *(negativeArray+m2);
                *(negativeArray+m2) = temp1;
            }
        }

    // 开始计算资金流
    for (temp6=msHead; temp6!=nullptr;) { // 清空当前已存在的资金流链表
        temp7 = temp6;
        temp6 = temp6->next;
        delete temp7;
    }
    msHead = nullptr;

    m1 = m2 = 0;
    while ((m1<positiveNumber) && (m2<negativeNumber)) {
        // 新建一个MoneyStream结构体，然后连接好它的数据结构
        temp6 = new MoneyStream;
        temp6->next = nullptr;
        if (msHead == nullptr) {
            msHead = temp7 = temp6;
        }
        else {
            temp7->next = temp6;
            temp7 = temp6;
        }

        // 找到正Balance和负Balance中绝对值较小的那个，作为一笔资金流的金额，然后在两个账户的Balance中减去它
        cmp1 = (*(positiveArray + m1))->Balance;
        cmp2 = (*(negativeArray + m2))->Balance;
        if (int(cmp1*100)<qAbs(int(cmp2*100)))      minValue = cmp1;
        else                                        minValue = qAbs(cmp2);
        temp6->sender = *(positiveArray + m1);
        temp6->receiver = *(negativeArray + m2);
        temp6->money = minValue;
        (*(positiveArray + m1))->Balance -= minValue;
        (*(negativeArray + m2))->Balance += minValue;

        // 这笔计算完毕后，判断当前账户是否已清帐，如果已清帐，则往后移一位
        if (int((*(positiveArray + m1))->Balance*100)==0)   m1++;
        if (int((*(negativeArray + m2))->Balance*100)==0)   m2++;
    }

    delete positiveArray;
    delete negativeArray;
}


void CalculateDialog::calculateExec() {
    QString msString="";
    MoneyStream *temp1;
    int count;

    calculate();

    for (count=1, temp1=msHead; temp1!=nullptr; count++, temp1=temp1->next){
        msString.append("Money Stream NO.");
        msString.append(QString::number(count));
        msString.append("\n\tSender: ");
        msString.append(temp1->sender->Name);
        msString.append("\n\tReceiver: ");
        msString.append(temp1->receiver->Name);
        msString.append("\n\tMoney: ");
        msString.append(QString::number(temp1->money));
        msString.append("\n\n");
    }

    QMessageBox::information(this, "Money stream list", msString, QMessageBox::Ok);

}


void CalculateDialog::clearExec() {
    MoneyStream *temp1;
    OneBill *temp2;
    CartList *temp4;

    int ans = QMessageBox::warning(this, "Clear warning",
                                   "The clear section is irreversible. After clearing the seleted bills, they cannot be recoverd back any more. Do you still want to clear them?",
                                   QMessageBox::Yes | QMessageBox::Yes, QMessageBox::No);

    if (ans == QMessageBox::Yes) {
        calculateExec();

        if (currentBill->head == nullptr)   return;
        // 自动生成平账单
        for (temp1=msHead; temp1!=nullptr; temp1=temp1->next) {
            temp2 = new OneBill;
            temp2->Number = currentBill->tail->Number + 1;
            temp2->Time = ((QDateTime::currentDateTime()).toString("yyyyMMdd")).toInt();
            temp2->Item = "Money stream from "+temp1->sender->Name+" to "+temp1->receiver->Name;
            temp2->Money = temp1->money;
            temp2->Independent = true;
            temp2->enable = false;
            temp2->pSubBill = nullptr;
            temp2->pObjBill = nullptr;

            currentBill->tail->next = temp2;
            temp2->prev = currentBill->tail;
            temp2->next = nullptr;
            currentBill->tail = temp2;

            temp2->PaidHeader = new Divider;
            temp2->PaidHeader->People = temp1->sender->Number;
            temp2->PaidHeader->Money = temp1->money;
            temp2->PaidHeader->next = nullptr;
            temp2->OwedHeader = new Divider;
            temp2->OwedHeader->People = temp1->receiver->Number;
            temp2->OwedHeader->Money = temp1->money;
            temp2->OwedHeader->next = nullptr;
        }
        // 将选中要进行清帐的单据设置为enable=false
        for (temp4=cartHead; temp4!=nullptr; temp4=temp4->next)
            if (temp4->box->isChecked())    temp4->billptr->enable = false;

        QMessageBox::information(this, "Clear successfully", "The selected bills are cleared successfully!", QMessageBox::Ok);
        done(QDialog::Accepted);
    }

}


void CalculateDialog::examineSeleteAll() {
    CartList *temp1;
    int count;

    for (temp1=cartHead, count=0; temp1!=nullptr; temp1=temp1->next)
        if (temp1->box->isChecked())    count += 1;

    if (count == 0)                     c1.setCheckState(Qt::Unchecked);
    else if (count == totalBill)      c1.setCheckState(Qt::Checked);
    else                                c1.setCheckState(Qt::PartiallyChecked);
}


void CalculateDialog::seleteAllExec() {
    CartList *temp1;

    if (c1.checkState()==Qt::PartiallyChecked)      c1.setCheckState(Qt::Checked);
    if (c1.checkState()==Qt::Checked) {
        c1.setCheckState(Qt::Unchecked);
        for (temp1=cartHead; temp1!=nullptr; temp1=temp1->next)         temp1->box->setCheckState(Qt::Checked);
    }
    else{
        c1.setCheckState(Qt::Checked);
        for (temp1=cartHead; temp1!=nullptr; temp1=temp1->next)         temp1->box->setCheckState(Qt::Unchecked);
    }

}









