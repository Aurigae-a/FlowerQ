#include "accountdialog.h"
#include <QHeaderView>
#include <QDebug>
#include <QMessageBox>


AccountDialog::AccountDialog(QString path) {
    initializeWindow();
    current_account = new Account(path);
    UpdateTable();
}


AccountDialog::AccountDialog(Account* src) {
    initializeWindow();
    current_account = new Account();
    current_account->copy(src);
    UpdateTable();
}


AccountDialog::~AccountDialog()
{
    int m1;

    if (t1->rowCount()!=0) {
        for (m1=0; m1<t1->rowCount(); m1++) {
            delete t1->item(m1,0);
            delete t1->item(m1,1);
            delete t1->item(m1,2);
        }
    }
    delete t1;
    delete current_account;
}


void AccountDialog::SaveExec()
{
    done(Accepted);
}


void AccountDialog::CancelExec()
{
    done(Rejected);
}


void AccountDialog::AddExec()
{
    User *temp, *temp2;

    /* 如果是增加新的用户，那么就生成一个名字为空字符串，编号为最高编号+1，状态为非冻结态的新User结构体，把这个结构体传入到对话框中。*/

    temp2 = new User;
    temp2->Name = "";
    temp2->Number = current_account->highestNumber + 1;
    temp2->Balance = 0.0;
    temp2->isFreezed = false;

    dlg = new AddAccountDialog(temp2);
    if (dlg->exec() == QDialog::Accepted) {
        temp = new User;
        temp->Paid                  = 0.0;
        temp->Owed                  = 0.0;
        temp->Balance               = 0.0;
        temp->Name                  = dlg->tempUser->Name;
        temp->Number                = dlg->tempUser->Number;
        temp->isFreezed             = dlg->tempUser->isFreezed;
        temp->next                  = nullptr;

        // 如果tail是null的话，说明当前用户册是空的，因此这个新增用户是第一个，如果不是空的，则将新增用户加到链表的结尾处
        if (current_account->tail == nullptr) {
            current_account->head   = temp;
            current_account->tail   = temp;
            temp->prev              = nullptr;
        }
        else {
            current_account->tail->next = temp;
            temp->prev                  = current_account->tail;
            current_account->tail       = temp;
        }

        // 用户总数+1，当前最高编号+1
        current_account->total_num += 1;
        current_account->highestNumber += 1;

        UpdateTable();
    }
    delete dlg;
    delete temp2;
}


void AccountDialog::DeleteExec()
{
    int  current_row, m1;
    User *temp;

    /* 如果当前用户册已经为空，那么再点击Delete键将不会有任何作用；如果当前用户册只剩下最后一个用户，那么删除完这个用户后，还要把head和tail都归为nullptr；如果不是最后一个
        那就进行正常的删除。删除头部和尾部的时候，应当进行head和tail的响应移动，删除中间的时候，可以不用管head和tail。
       同时，还要进行判断，如果这个用户他当前的Paid与Owed不为0的话，就要弹出消息对话框，提示该用户无法删除。并取消删除操作。*/
    if (current_account->total_num == 0){}
    else if (current_account->total_num == 1) {
        if ((int(current_account->head->Paid*100)!=0)||(int(current_account->head->Owed*100)!=0)) {
            QMessageBox::question(this, "Warning", "The selected user still has some balance, so cannot delete this user.", QMessageBox::Ok);
        }
        else {
            delete current_account->head;
            current_account->head = current_account->tail = nullptr;
            current_account->total_num -= 1;
        }
    }
    else
    {
        current_row = t1->currentIndex().row();
        for (m1=0, temp=current_account->head; m1<current_row; m1++, temp=temp->next);

        if ((int(current_account->head->Paid*100)!=0)||(int(current_account->head->Owed*100)!=0)) {
            QMessageBox::question(this, "Warning", "The selected user still has some balance, so cannot delete this user.", QMessageBox::Ok);
        }
        else {
            if (current_row == 0) {
                current_account->head       = current_account->head->next;
                current_account->head->prev = nullptr;
            }
            else {
                if (current_row == current_account->total_num-1)
                {
                    current_account->tail       = current_account->tail->prev;
                    current_account->tail->next = nullptr;
                }
                else
                {
                    temp->prev->next = temp->next;
                    temp->next->prev = temp->prev;
                }

            }
            delete temp;
            current_account->total_num -= 1;
        }
    }

    UpdateTable();

}


void AccountDialog::ChangeExec()
{
    int current_row, m1;
    User *temp;

    /* 修改按钮的相关操作：
        如果当前用户册的个数是0的话，那么点击Change按钮是没有反应的
        如果当前用户册的个数不是0，那么就新建一个新增用户窗口，新增用户窗口的配置设置为2，即修改型。生成一个临时的User结构体，在结构体中复制要修改用户的变量，并把这个临时结
            构体传入到对话框中。从对话框中可以得到哪些修改的选项，这样在用户册窗口进行更新。*/
    if (current_account->total_num == 0) {}
    else {
        current_row = t1->currentIndex().row();
        for (m1=0, temp=current_account->head; m1<current_row; m1++, temp=temp->next);

        dlg = new AddAccountDialog(temp);
        if (dlg->exec() == QDialog::Accepted)
        {
            temp->Name = dlg->tempUser->Name;
            temp->isFreezed = dlg->tempUser->isFreezed;
            UpdateTable();
        }
        delete dlg;
    }
}


void AccountDialog::UpdateTable()
{
    int m1;
    User *temp;
    QTableWidgetItem *it;

    if (t1->rowCount() != 0) {
        for (m1=0; m1<t1->rowCount(); m1++) {
            delete t1->item(m1,0);
            delete t1->item(m1,1);
            delete t1->item(m1,2);
        }
    }

    t1->setRowCount(current_account->total_num);
    t1->setColumnCount(3);

    for (m1=0, temp=current_account->head; temp!=nullptr; m1++, temp=temp->next)
    {
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

        it = new QTableWidgetItem;
        t1->setItem(m1,2,it);
        if (temp->isFreezed)    it->setText("Freezed");
        else                    it->setText("Unfreezed");
        it->setTextColor(QColor(0,0,0));
        it->setBackgroundColor(QColor(255,255,255));
    }

}


void AccountDialog::initializeWindow() {

    QStringList t1Header;

    /* 进行用户册编辑窗口的配置 */
    this->setWindowTitle("Account Manager");
    this->resize(500,300);

    ly1 = new QGridLayout(this);

    t1  = new QTableWidget(0, 3);
    t1->horizontalHeader()->setVisible(true);
    t1->verticalHeader()->setVisible(false);
    t1->setSelectionBehavior(QAbstractItemView::SelectRows);
    t1->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t1->setSelectionMode(QAbstractItemView::SingleSelection);
    t1Header << "NO" << "Name" << "Status";
    t1->setHorizontalHeaderLabels(t1Header);
    t1->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    b1.setText("Save");
    b2.setText("Cancel");
    b3.setText("Add");
    b4.setText("Delete");
    b5.setText("Change");

    ly1->addWidget( t1, 0, 0, 5, 3);
    ly1->addWidget(&b3, 1, 3, 1, 1);
    ly1->addWidget(&b4, 2, 3, 1, 1);
    ly1->addWidget(&b5, 3, 3, 1, 1);
    ly1->addWidget(&b1, 5, 0, 1, 1);
    ly1->addWidget(&b2, 5, 2, 1, 1);

    connect(&b1, &QPushButton::clicked, this, &AccountDialog::SaveExec);
    connect(&b2, &QPushButton::clicked, this, &AccountDialog::CancelExec);
    connect(&b3, &QPushButton::clicked, this, &AccountDialog::AddExec);
    connect(&b4, &QPushButton::clicked, this, &AccountDialog::DeleteExec);
    connect(&b5, &QPushButton::clicked, this, &AccountDialog::ChangeExec);

}
















