#include "bill.h"
#include <QFile>
#include <QIODevice>
#include <QDebug>

Bill::Bill(QString input_objective, QString input_subjective)
{
    OneBill *temp;
    head            = NULL;
    tail            = NULL;

    objective_saving_path  = input_objective;
    subjective_saving_path = input_subjective;

    readFromFile(objective_saving_path);
    readFromFile(subjective_saving_path);
}

Bill::~Bill()
{
    OneBill         *temp1, *temp2;

    for (temp1=head; temp1!=NULL;){
        temp2 = temp1;
        temp1 = temp1->next;
        deleteIndependentBill(1, temp2);
    }
}

void Bill::readFromFile(QString path)
{
    QFile     rd(path);
    char      buffer[1024];
    bool      isint;
    int       LineLength, ret, m1, state, temp1;
    float     temp4, deci;
    OneBill  *temp2;
    QString   temp3;
    Divider  *temp5, *tail2;
    OneBill  *temp6, *obill;

    if(!rd.open(QIODevice::ReadOnly))
    {

    }
    else{
        state = 0;
        while(!rd.atEnd())
        {
            LineLength = rd.readLine(buffer, 1024);
            for (m1=0; m1<LineLength; m1++)
            {
                if (buffer[m1] == '#')
                {
                    state      = 1;
                    temp1      = 0;
                }
                else if (buffer[m1] == '%')
                {
                    state         = 1;
                    temp2->Number = temp1;
                    temp1         = 0;
//                    qDebug() << temp2->Number;
                }
                else if (buffer[m1] == '^')
                {
                    state         = 2;
                    temp2->Time   = temp1;
                    temp3         = "";
//                    qDebug() << temp1;
                }
                else if (buffer[m1] == '$')
                {
                    state         = 3;
                    temp2->Item   = temp3;
                    temp4         = 0.0;
                    isint         = true;
//                    qDebug() << temp3;
                }
                else if (buffer[m1] == '@')
                {
                    state         = 1;
                    temp2->Money  = temp4;
                    temp1         = 0;

                    if (buffer[m1+1] == '&'){

                    }
                    else{
                        temp5             = new Divider;
                        temp5->next       = NULL;
                        temp2->PaidHeader = temp5;
                        tail2             = temp5;
                    }
//                    qDebug() << temp4;
                }
                else if (buffer[m1] == '~')
                {
                    state         = 3;
                    temp5->People = temp1;
                    temp4         = 0.0;
                    isint         = true;
//                    qDebug() << temp1;
                }
                else if (buffer[m1] == '/')
                {
                    state         = 1;
                    temp5->Money  = temp4;
                    temp1         = 0;

                    temp5             = new Divider;
                    temp5->next       = NULL;
                    tail2->next       = temp5;
                    tail2             = temp5;
//                    qDebug() << temp4;
                }
                else if (buffer[m1] == '&')
                {
                    if (buffer[m1-1] != '@'){
                        temp5->Money  = temp4;
                    }

                    state         = 1;
                    temp1         = 0;

                    temp5             = new Divider;
                    temp5->next       = NULL;
                    temp2->OwedHeader = temp5;
                    tail2             = temp5;
//                   qDebug() << temp4;
                }
                else if (buffer[m1] == '!')
                {
                    temp2             = new OneBill;
                    temp2->next       = NULL;
                    temp2->prev       = NULL;
                    temp2->PaidHeader = NULL;
                    temp2->OwedHeader = NULL;
                    temp2->pSubBill   = NULL;
                    temp2->pObjBill   = NULL;

                    m1 = m1 + 1;
                    if (buffer[m1] == '0')  temp2->Independent = false;
                    else                    temp2->Independent = true;

                    if (temp2->Independent == true){
                        if (head==NULL)
                        {
                            head        = temp2;
                            tail        = temp2;
                        }
                        else
                        {
                            tail->next  = temp2;
                            temp2->prev = tail;
                            tail        = temp2;
                        }
                    }


//                    qDebug() << temp4;
//                    qDebug() << temp2->Independent;
                }
                else if (buffer[m1] == '+')
                {
                    temp5->Money  = temp4;

                    if (temp2->Independent == false){
                        state = 1;
                        temp1 = 0;
                    }

//                    qDebug() << "Fucker!";
                }
                else if (buffer[m1] == '=')
                {
                    obill = findByNumber(temp1);
//                    qDebug() << obill;
                    for (temp6=obill; temp6->pSubBill!=NULL; temp6=temp6->pSubBill);

                    temp6->pSubBill = temp2;
                    temp2->pObjBill = temp6;

                }
                else{
                    if (state==1)   temp1 = temp1 * 10 + (buffer[m1] - '0');
                    if (state==2)   temp3 = temp3 + QString(buffer[m1]);
                    if (state==3)
                    {
                        if (buffer[m1] == '.')
                        {
                            isint = false;
                            deci  = 0.1;
                        }
                        else
                        {
                            if (isint == true)  temp4 = temp4 * 10 + (buffer[m1]-'0');
                            else{
                                temp4 = temp4 + deci * (buffer[m1]-'0');
                                deci  = deci / 10;
                            }
                        }
                    }

                }
            }
        }
        ret = 0;
    }


    rd.close();
}


OneBill* Bill::findByNumber(int required)
{
    OneBill *temp1, *temp2;
    bool    got_answer;

    for (temp1=head, temp2=NULL, got_answer=false; temp1!=NULL; temp1=temp1->next){
        for (temp2=temp1; temp2!=NULL; temp2=temp2->pSubBill){
            if (temp2->Number-required==0){
                got_answer = true;
                break;
            }
        }
        if (got_answer == true)     break;
    }
    return temp2;
}


void Bill::deleteIndependentBill(int command, OneBill* delete_object)
{
    Divider         *temp1, *temp2;
    OneBill         *temp3, *temp4;

    // delete the paid dividers and owed dividers
    for (temp1=delete_object->OwedHeader; temp1!=NULL; ){
        temp2 = temp1;
        temp1 = temp1->next;
        delete temp2;
    }
    for (temp1=delete_object->PaidHeader; temp1!=NULL; ){
        temp2 = temp1;
        temp1 = temp1->next;
        delete temp2;
    }
    // delete the sub-bills
    for (temp3=delete_object->pSubBill; temp3!=NULL; ){
        temp4 = temp3;
        temp3 = temp3->pSubBill;
        deleteSubBill(1, temp4);
    }
    if (command == 1){
        // adjust the positions
        if (delete_object->prev == NULL){
            if (delete_object->next == NULL){
                head = NULL;
                tail = NULL;
            }
            else{
                delete_object->next->prev = NULL;
                head                      = delete_object->next;
            }
        }
        else{
            if (delete_object->next == NULL){
                delete_object->prev->next = NULL;
                tail                      = delete_object->prev;
            }
            else{
                delete_object->next->prev = delete_object->prev;
                delete_object->prev->next = delete_object->next;
            }
        }
    }

    // delete the object
    delete delete_object;
}


void Bill::deleteSubBill(int command, OneBill* delete_object)
{
    Divider         *temp3, *temp4;

    // delete the paid dividers and owed dividers
    for (temp3=delete_object->OwedHeader; temp3!=NULL; ){
        temp4 = temp3;
        temp3 = temp3->next;
        delete temp4;
    }
    for (temp3=delete_object->PaidHeader; temp3!=NULL; ){
        temp4 = temp3;
        temp3 = temp3->next;
        delete temp4;
    }

    if (command == 1){
        // adjust the positions
        if (delete_object->pSubBill == NULL){
            delete_object->pObjBill->pSubBill = NULL;
        }
        else{
            delete_object->pObjBill->pSubBill = delete_object->pSubBill;
            delete_object->pSubBill->pObjBill = delete_object->pObjBill;
        }
    }

    // delete the object
    delete delete_object;
}


OneBill* Bill::copyIndependentBill(OneBill *object_bill)
{
    OneBill *copy_bill = new OneBill;
    OneBill *temp1, *temp2, *tail;

    // initialize the pointers
    copy_bill->PaidHeader = NULL;
    copy_bill->OwedHeader = NULL;
    copy_bill->pObjBill   = NULL;
    copy_bill->pSubBill   = NULL;
    copy_bill->prev       = NULL;
    copy_bill->next       = NULL;

    // copy the basic info
    copy_bill->Number       = object_bill->Number;
    copy_bill->Time         = object_bill->Time;
    copy_bill->Item         = object_bill->Item;
    copy_bill->Money        = object_bill->Money;
    copy_bill->Independent  = object_bill->Independent;

    // copy the paid and owed dividers
    copyDivider(object_bill->PaidHeader, copy_bill->PaidHeader);
    copyDivider(object_bill->OwedHeader, copy_bill->OwedHeader);

    // copy the sub-bills
    for (temp1=object_bill->pSubBill, tail=copy_bill; temp1!=NULL; temp1=temp1->pSubBill){
        temp2           = copySubBill(temp1);
        tail->pSubBill  = temp2;
        temp2->pObjBill = tail;
        tail            = temp2;
    }

    return copy_bill;
}


OneBill* Bill::copySubBill(OneBill *object_bill)
{
    OneBill *copy_bill = new OneBill;

    // initialize the pointers
    copy_bill->PaidHeader = NULL;
    copy_bill->OwedHeader = NULL;
    copy_bill->pObjBill   = NULL;
    copy_bill->pSubBill   = NULL;
    copy_bill->prev       = NULL;
    copy_bill->next       = NULL;

    // copy the basic info
    copy_bill->Number       = object_bill->Number;
    copy_bill->Time         = object_bill->Time;
    copy_bill->Item         = object_bill->Item;
    copy_bill->Money        = object_bill->Money;
    copy_bill->Independent  = object_bill->Independent;

    // copy the paid and owed dividers
    copyDivider(object_bill->PaidHeader, copy_bill->PaidHeader);
    copyDivider(object_bill->OwedHeader, copy_bill->OwedHeader);

    return copy_bill;
}


void Bill::copyDivider(Divider* &original_head, Divider* &copyfile_head)
{
    Divider *temp1, *temp2, *tail;


    for (temp1=original_head; temp1!=NULL; temp1=temp1->next){
        temp2         = new Divider;
        temp2->People = temp1->People;
        temp2->Money  = temp1->Money;
        temp2->next   = NULL;

        if (copyfile_head == NULL){
            copyfile_head         = temp2;
            tail                  = temp2;
        }
        else{
            tail->next            = temp2;
            tail                  = temp2;
        }
    }
}


void Bill::replaceIndependentBill(OneBill *original, OneBill *replacer)
{
    if (original->prev == NULL){
        if (original->next == NULL){
            head           = replacer;
            tail           = replacer;
            replacer->prev = NULL;
            replacer->next = NULL;
        }
        else{
            original->next->prev = replacer;
            replacer->next       = original->next;
            replacer->prev       = NULL;
            head                 = replacer;
        }
    }
    else{
        if (original->next == NULL){
            original->prev->next = replacer;
            replacer->next       = NULL;
            replacer->prev       = original->prev;
            tail                 = replacer;
        }
        else{
            original->next->prev = replacer;
            replacer->next       = original->next;
            original->prev->next = replacer;
            replacer->prev       = original->prev;
        }

    }
    original->prev = NULL;
    original->next = NULL;
}


void Bill::replaceSubBill(OneBill *original, OneBill *replacer)
{
    if (original->pSubBill == NULL){
        original->pObjBill->pSubBill = replacer;
        replacer->pObjBill           = original->pObjBill;
        replacer->pSubBill           = NULL;
    }
    else{
        original->pObjBill->pSubBill = replacer;
        replacer->pObjBill           = original->pObjBill;
        original->pSubBill->pObjBill = replacer;
        replacer->pSubBill           = original->pSubBill;
    }

    original->pSubBill = NULL;
    original->pObjBill = NULL;
}


void Bill::saveToFile(QString objective_path, QString subjective_path)
{
    OneBill *temp1, *temp2, *host;
    Divider *temp3;
    QFile   obj_wt(objective_path), sub_wt(subjective_path);

    obj_wt.open(QIODevice::WriteOnly|QIODevice::Text);
    sub_wt.open(QIODevice::WriteOnly|QIODevice::Text);

    QTextStream out1(&obj_wt), out2(&sub_wt);

    for (temp1=head; temp1!=NULL; temp1=temp1->next)
        for (temp2=temp1; temp2!=NULL; temp2=temp2->pSubBill){

            if (temp2->Independent == true){
                // output the basic info
                out1 << "!1#" << QString::number(temp2->Number) << "%" << QString::number(temp2->Time) << "^" << temp2->Item
                     << "$"   << QString::number(temp2->Money)  << "@";
                // output the paid divider
                for (temp3=temp2->PaidHeader; temp3!=NULL; temp3=temp3->next){
                    out1 << QString::number(temp3->People) << "~" << QString::number(temp3->Money);
                    if (temp3->next != NULL)    out1 << "/";
                    else                        out1 << "&";
                }
                // output the owed divider
                for (temp3=temp2->OwedHeader; temp3!=NULL; temp3=temp3->next){
                    out1 << QString::number(temp3->People) << "~" << QString::number(temp3->Money);
                    if (temp3->next != NULL)    out1 << "/";
                    else                        out1 << "+";
                }
                out1 << "\n";
            }
            else{
                // output the basic info
                out2 << "!0#" << QString::number(temp2->Number) << "%" << QString::number(temp2->Time) << "^" << temp2->Item
                     << "$"   << QString::number(temp2->Money)  << "@&";
                // output the owed divider
                for (temp3=temp2->OwedHeader; temp3!=NULL; temp3=temp3->next){
                    out2 << QString::number(temp3->People) << "~" << QString::number(temp3->Money);
                    if (temp3->next != NULL)    out2 << "/";
                    else                        out2 << "+";
                }
                // find the host bill's number
                for (host=temp2; host->pObjBill!=NULL; host=host->pObjBill);
                // output the host bill number
                out2 << host->Number << "=\n";
            }
        }

    obj_wt.close();
    sub_wt.close();
}
















