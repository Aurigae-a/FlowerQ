#include "account.h"
#include <QFile>
#include <QDebug>

Account::Account(QString path)
{
    int f1;

    SavingFilePath = path;
    head = NULL;
    tail = NULL;

//    qDebug() << SavingFilePath;
    f1 = ReadFromFile(SavingFilePath);
}


Account::~Account()
{
    User *temp1, *temp2;

    if (head!=NULL)
    {
        for (temp1=head; temp1!=NULL; ){
            temp2 = temp1;
            temp1 = temp1->next;
            delete temp2;
        }
    }

}


int Account::ReadFromFile(QString path)
{
    QFile rd(path);
    char buffer[1024];
    int LineLength, m1, temp3=0, state=0, ret, m2, count;
    User *temp1, *temp2;
    QString temp4;


    if (head!=NULL){
        for (temp1=head; temp1!=NULL; ){
            temp2 = temp1;
            temp1 = temp1->next;
            delete temp2;
        }
    }

    if(!rd.open(QIODevice::ReadOnly))
    {
//        qDebug() << "Fuck!";
        ret = 0;
    }
    else
    {
        count = 0;
        while(!rd.atEnd())
        {
            LineLength = rd.readLine(buffer, 1024);

            for (m1=0; m1<LineLength; m1++)
            {
                if (buffer[m1] == '#')
                {
                    temp1          = new User;
                    temp1->Paid    = 0.0;
                    temp1->Owed    = 0.0;
                    temp1->Balance = 0.0;
                    if (head==NULL)
                    {
                        head        = temp1;
                        tail        = temp1;
                        temp1->next = NULL;
                        temp1->prev = NULL;
                    }
                    else
                    {
                        temp1->prev = tail;
                        temp1->next = NULL;
                        tail->next  = temp1;
                        tail        = temp1;
                    }
                    state = 1;
                }

                else if (buffer[m1] == '%')
                {
                    temp1->Number = temp3;
                    temp3         = 0;
                    state         = 2;
                }

                else if (buffer[m1] == '$')
                {
                    temp1->Name = temp4;
                    temp4       = "";
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
        ret = 1;
    }
//    qDebug() << "ahb!";
    total_num = count;
    rd.close();

/*
    for (temp1=head, m2=0; temp1!=NULL; temp1 = temp1->next)
    {
        qDebug() << ++m2;
        qDebug() << temp1->Number;
        qDebug() << temp1->Name;

    }
*/



    return ret;
}


int Account::SaveToFile(QString path)
{
    QFile wt(path);
//    QTextStream str(&wt);
    User *temp1;

//    qDebug() << path;
    if(!wt.open(QIODevice::WriteOnly | QIODevice::Text))
    {
//        qDebug() << "Fuck!";
    }
    else
    {
//        qDebug() << "Aha!";
    }
    QTextStream str(&wt);

    for (temp1=head; temp1!=NULL; temp1=temp1->next)
    {
        str << "#" << temp1->Number << "%" << temp1->Name << "$" << endl;
    }

    wt.close();
}


User* Account::FindByNumber(int required)
{
    User *temp;

    for (temp=head; temp!=NULL; temp=temp->next){
        if (temp->Number==required)     break;
    }

    return temp;
}



















