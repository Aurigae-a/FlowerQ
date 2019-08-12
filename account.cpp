#include "account.h"
#include <QFile>
#include <QDebug>

Account::Account(QString path)
{
    int f1;

    SavingFilePath = path;
    head = nullptr;
    tail = nullptr;
    highestNumber = 1000;

//    qDebug() << SavingFilePath;
    f1 = readFromFile(SavingFilePath);
}


Account::Account()
{
    SavingFilePath = "";
    head = nullptr;
    tail = nullptr;
    highestNumber = 1000;
}


Account::~Account()
{
    clearALl();
}


int Account::readFromFile(QString path)
{
    QFile rd(path);
    char buffer[1024];
    qint64 LineLength;
    int m1, temp3=0, state=0, ret, count=0;
    User *temp1=nullptr;
    QString temp4;

    // 如果当前用户册的头不是空的，说明当前用户册已经被使用过，再次加载其他用户册的时候，应当首先清空当前用户册
    clearALl();

    if(!rd.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ret = 0;
    }
    else
    {
        // 判断文件的第一行是不是指定的字符串，以判断是否是该程序能读取的数据文件
        rd.readLine(buffer, 1024);
        if(QString(buffer).compare("FlowerQAccount:Yes\n")!=0) {
            ret = 0;
            rd.close();
            return ret;
        }

        // 读取当前用户册的最大的编号
        rd.readLine(buffer, 1024);
        highestNumber = QString(buffer).toInt();

        count = 0;
        while(!rd.atEnd())
        {

            LineLength = rd.readLine(buffer, 1024);

            for (m1=0; m1<LineLength; m1++)
            {
                // 读到#的时候，表示遇到一个新的用户，要新建一个User结构体，并开始进入整数读取状态
                if (buffer[m1] == '#')
                {
                    temp1          = new User;
                    temp1->Paid    = 0.0;
                    temp1->Owed    = 0.0;
                    temp1->Balance = 0.0;
                    if (head==nullptr)
                    {
                        head        = temp1;
                        tail        = temp1;
                        temp1->next = nullptr;
                        temp1->prev = nullptr;
                    }
                    else
                    {
                        temp1->prev = tail;
                        temp1->next = nullptr;
                        tail->next  = temp1;
                        tail        = temp1;
                    }
                    state = 1;
                }

                // 读到%的时候，表示该用户的编号读取完毕，准备开始读取用户名，所以要将编号付给User的Number，并进入字符串读取状态
                else if (buffer[m1] == '%')
                {
                    temp1->Number = temp3;
                    temp3         = 0;
                    state         = 2;
                }

                /* 读到^的时候，表示该用户的用户名读取完毕，准备开始读取冻结状态，所以要将用户名付给User的Name，并直接再往后读一个数字，判断是1还是0
                    如果是1，则当前账户处于未冻结状态；如果是0，则当前用户处于冻结状态*/
                else if (buffer[m1] == '^')
                {
                    temp1->Name = temp4;
                    temp4       = "";
                    if((buffer[++m1]-'1')==0)       temp1->isFreezed = false;
                    else                            temp1->isFreezed = true;
                    state       = 0;
                }

                // 读到$的时候，表示该用户的全部信息读取完毕，当前用户册总数+1
                else if (buffer[m1] == '$')
                {
                    state       = 0;
                    count      += 1;
                }

                else
                {
                    if (state == 1)     temp3 = temp3 * 10 + (buffer[m1] - '0');
                    if (state == 2)     temp4 = temp4 + QString(buffer[m1]);
                }
            }
        }

        total_num = count;
        rd.close();
        ret = 1;
    }

    return ret;
}


int Account::saveToFile(QString path)
{
    int ret;
    QFile wt(path);
    User *temp1;

    if(!wt.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // 程序执行到这里，表示文件未成功以只写和文本形式打开
        ret = 0;
    }
    else {
        // 程序执行到这里，表示文件已成功以只写和文本形式打开
        QTextStream str(&wt);

        // 输出文件的头部，表示这个文件是一个格式正确的用户册文件
        str << "FlowerQAccount:Yes" << endl;

        // 输出当前用户册的最大编号
        str << highestNumber << endl;

        // 依次将该用户册中的各个用户的信息输出
        for (temp1=head; temp1!=nullptr; temp1=temp1->next) {
            str << "#" << temp1->Number << "%" << temp1->Name;
            if (temp1->isFreezed)   str << "^0";
            else                    str << "^1";
            str << "$" << endl;
        }
        ret = 1;

        wt.close();
    }

    return ret;
}


User* Account::FindByNumber(int required)
{
    User *temp;

    for (temp=head; temp!=nullptr; temp=temp->next){
        if (temp->Number==required)     break;
    }

    return temp;
}


void Account::clearALl() {
    User *temp1, *temp2;

    if (head!=nullptr){
        for (temp1=head; temp1!=nullptr; ){
            temp2 = temp1;
            temp1 = temp1->next;
            delete temp2;
        }
        head = tail = nullptr;
        SavingFilePath = "";
        highestNumber = 1000;
    }
}


void Account::copy(Account *src) {
    User *temp1, *temp2;

    /*将用户册src中的信息全部复制到当前的用户册中。首先对当前用户册进行清空，然后再对src的每一项进行复制。*/
    clearALl();

    total_num = src->total_num;
    highestNumber = src->highestNumber;
    for (temp2=src->head; temp2!=nullptr; temp2=temp2->next) {
        temp1 = new User;
        temp1->Number = temp2->Number;
        temp1->Name   = temp2->Name;
        temp1->isFreezed = temp2->isFreezed;
        temp1->Paid = temp2->Paid;
        temp1->Owed = temp2->Owed;
        temp1->Balance = temp2->Balance;
        temp1->next = nullptr;

        if (head == nullptr) {
            head = tail = temp1;
            temp1->prev = nullptr;
        }
        else {
            tail->next = temp1;
            temp1->prev = tail;
            tail = temp1;
        }
    }
}

















