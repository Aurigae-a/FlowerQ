#include "bill.h"
#include <QFile>
#include <QIODevice>
#include <QDebug>


Bill::Bill() {
    head            = nullptr;
    tail            = nullptr;
    savingPath = "";
    obName = "";
    sbName = "";
}


Bill::~Bill()
{
    deleteAll();
}

int Bill::readFromFile(QString path)
{
    QFile     rd(path);
    char      buffer[1024];
    bool      isint=false;
    qint64    LineLength;
    int       ret=0, m1, state, temp1=0;
    double    temp4=0.0, deci=0.0;
    OneBill  *temp2=nullptr;
    QString   temp3;
    Divider  *temp5=nullptr, *tail2=nullptr;
    OneBill  *temp6, *obill;

    /* 以只读和文本的方式打开数据文件，如果无法打开，则返回0；如果打开了，但是第一行不是FlowerQBill文件的头，则证明所指明的文件不正确，返回0；如果数据文件打开成功，头一行
        也正确，但是SubBill找不到它的主单，则返回2，其他情况下，按照正常的顺序来读取文件*/
    if(!rd.open(QIODevice::ReadOnly | QIODevice::Text)){
        ret = 0;
    }
    else{
        // 读取第一行，以判断是否是正确的FlowerQBill格式
        rd.readLine(buffer, 1024);
        if (QString(buffer).compare("FlowerQBill:Yes\n")!=0) {
            return 0;
        }

        // 读取第二行，获得和它配合的数据文件的名字
        rd.readLine(buffer, 1024);
        QString tempStr = QString(buffer);
        int tempIndexStart = tempStr.lastIndexOf(':') + 1;
        int tempIndexEnd = tempStr.lastIndexOf('\n') - 1;
        if (buffer[0] == '0') {
            sbName = tempStr.mid(tempIndexStart, tempIndexEnd-tempIndexStart+1);
        }
        else {
            if (obName.compare(tempStr.mid(tempIndexStart, tempIndexEnd-tempIndexStart+1))!=0){
                return 2;
            }
        }

        state = 0;
        while(!rd.atEnd()) {
            LineLength = rd.readLine(buffer, 1024);
            for (m1=0; m1<LineLength; m1++) {
                // 如果读到#，则说明要开始进行账单编号的读取了，所以应该进入整数读取状态
                if (buffer[m1] == '#') {
                    state      = 1;
                    temp1      = 0;
                }

                // 如果读到%，则说明编号读取完毕，要开始进行日期的读取了，所以应该将编号存入当前的OneBill结构体中，然后进入整数读取状态
                else if (buffer[m1] == '%') {
                    state         = 1;
                    temp2->Number = temp1;
                    temp1         = 0;
                }

                // 如果读到^，则说明日期读取完毕，要开始进行名称的读取了，所以应该将日期存入当前的OneBill结构体中，然后进入字符串读取状态
                else if (buffer[m1] == '^') {
                    state         = 2;
                    temp2->Time   = temp1;
                    temp3         = "";
                }

                // 如果读到$，则说明名称读取完毕，要开始进行金额的读取了，所以应该将名称存入当前的OneBill结构体中，然后进入小数读取状态
                else if (buffer[m1] == '$') {
                    state         = 3;
                    temp2->Item   = temp3;
                    temp4         = 0.0;
                    isint         = true;
                }

                // 如果读到@，则说明金额读取完毕，如果下一位是&，那么说明这是一个sub-bill，这时，应该跳过支付选项的读取；如果不是，则证明这是一个主单，这时应进入支付选项
                //  的读取。新建一个Divider结构体，把当前OneBill的PaidHeader指向该Divider结构体，然后进入整数读取状态以读取用户的编号
                else if (buffer[m1] == '@') {
                    state         = 1;
                    temp2->Money  = temp4;
                    temp1         = 0;

                    if (buffer[m1+1] == '&'){}
                    else{
                        temp5             = new Divider;
                        temp5->next       = nullptr;
                        temp2->PaidHeader = temp5;
                        tail2             = temp5;
                    }
                }

                // 如果读到～，则说明当前Divider结构体中的用户编号读取完毕，此时应该将编号存入Divider结构体中，然后进入小数读取状态以读取金额
                else if (buffer[m1] == '~') {
                    state         = 3;
                    temp5->People = temp1;
                    temp4         = 0.0;
                    isint         = true;
                }

                // 如果读到/，则说明当前Divider结构体中的金额读取完毕，并且还有下一个Divider结构体需要读取。此时应该将金额存入Divider结构体中，然后新建一个结构体，并进
                //  入整数读取状态以读取用户编号
                else if (buffer[m1] == '/') {
                    state         = 1;
                    temp5->Money  = temp4;
                    temp1         = 0;

                    temp5             = new Divider;
                    temp5->next       = nullptr;
                    tail2->next       = temp5;
                    tail2             = temp5;
                }

                // 如果读到&，说明Paid的部分已经读取完毕，下面要进入Owed部分的读取。如果前一位是@，则说明这是一个sub-bill，那就直接进入Owed读取部分。如果前一位不是@，则说
                //  明这是一个object-bill，此时就需要将最后一个Divider结构体的金额进行保存，然后再进入Owed的部分。Owed部分的读取，要新建一个Divider结构体，然后再进入整
                //  数读取状态，以读取用户编号
                else if (buffer[m1] == '&') {
                    if (buffer[m1-1] != '@'){
                        temp5->Money  = temp4;
                    }

                    state         = 1;
                    temp1         = 0;

                    temp5             = new Divider;
                    temp5->next       = nullptr;
                    temp2->OwedHeader = temp5;
                    tail2             = temp5;
                }

                // 如果读到!，说明一个OneBill已经读取完毕，并且要进入新的OneBill的读取状态。这时，不论是主单还是从单，都需要新建一个OneBill，并且读取!后面的数字来标记它们
                //  是主单还是从单，以及它们的Enable状态。如果是主单的话，则需要接到主单链表的结尾处。如果是从单，则不需要做任何操作，因为它们等读到=的时候会连接到相应的主单
                //  上。
                else if (buffer[m1] == '!') {
                    temp2             = new OneBill;
                    temp2->next       = nullptr;
                    temp2->prev       = nullptr;
                    temp2->PaidHeader = nullptr;
                    temp2->OwedHeader = nullptr;
                    temp2->pSubBill   = nullptr;
                    temp2->pObjBill   = nullptr;

                    if (buffer[++m1] == '0')  temp2->Independent = false;
                    else                    temp2->Independent = true;

                    if (buffer[++m1] == '1')  temp2->enable = true;
                    else                    temp2->enable = false;

                    if (temp2->Independent == true){
                        if (head==nullptr)
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
                }

                // 如果读到+，说明一个Owed的部分已经读取完毕，这时要将最后一个Divider结构体的金额保存进去。如果当前单是主单，则该OneBill已经读取完毕。如果当前单是从单，则
                //  需要再读取它相应主单的编号，因此要进入整数读取状态
                else if (buffer[m1] == '+') {
                    temp5->Money  = temp4;

                    if (temp2->Independent == false){
                        state = 1;
                        temp1 = 0;
                    }
                }

                // 如果读到=，则说明它是从单，并且主单号也读取完毕。这时，要根据读到的主单号找到相应的主单，然后把再把它接到主单所对应的从单链表的结尾。
                else if (buffer[m1] == '=') {
                    obill = findByNumber(temp1);
                    if (obill == nullptr)   return 2;
                    for (temp6=obill; temp6->pSubBill!=nullptr; temp6=temp6->pSubBill);

                    temp6->pSubBill = temp2;
                    temp2->pObjBill = temp6;

                }

                // 数据读取状态，state=1:整数读取；state=2:字符串读取；state=3:小数读取
                else {
                    if (state==1)   temp1 = temp1 * 10 + (buffer[m1] - '0');
                    if (state==2)   temp3 = temp3 + QString(buffer[m1]);
                    if (state==3) {
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
        ret = 1;
        rd.close();
    }
    return ret;
}


OneBill* Bill::findByNumber(int required)
{
    OneBill *temp1, *temp2;
    bool    got_answer;

    for (temp1=head, temp2=nullptr, got_answer=false; temp1!=nullptr; temp1=temp1->next){
        for (temp2=temp1; temp2!=nullptr; temp2=temp2->pSubBill){
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
    for (temp1=delete_object->OwedHeader; temp1!=nullptr; ){
        temp2 = temp1;
        temp1 = temp1->next;
        delete temp2;
    }
    for (temp1=delete_object->PaidHeader; temp1!=nullptr; ){
        temp2 = temp1;
        temp1 = temp1->next;
        delete temp2;
    }
    // delete the sub-bills
    for (temp3=delete_object->pSubBill; temp3!=nullptr; ){
        temp4 = temp3;
        temp3 = temp3->pSubBill;
        deleteSubBill(1, temp4);
    }
    if (command == 1){
        // adjust the positions
        if (delete_object->prev == nullptr){
            if (delete_object->next == nullptr){
                head = nullptr;
                tail = nullptr;
            }
            else{
                delete_object->next->prev = nullptr;
                head                      = delete_object->next;
            }
        }
        else{
            if (delete_object->next == nullptr){
                delete_object->prev->next = nullptr;
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
    for (temp3=delete_object->OwedHeader; temp3!=nullptr; ){
        temp4 = temp3;
        temp3 = temp3->next;
        delete temp4;
    }
    for (temp3=delete_object->PaidHeader; temp3!=nullptr; ){
        temp4 = temp3;
        temp3 = temp3->next;
        delete temp4;
    }

    if (command == 1){
        // adjust the positions
        if (delete_object->pSubBill == nullptr){
            delete_object->pObjBill->pSubBill = nullptr;
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
    copy_bill->PaidHeader = nullptr;
    copy_bill->OwedHeader = nullptr;
    copy_bill->pObjBill   = nullptr;
    copy_bill->pSubBill   = nullptr;
    copy_bill->prev       = nullptr;
    copy_bill->next       = nullptr;

    // copy the basic info
    copy_bill->Number       = object_bill->Number;
    copy_bill->Time         = object_bill->Time;
    copy_bill->Item         = object_bill->Item;
    copy_bill->Money        = object_bill->Money;
    copy_bill->Independent  = object_bill->Independent;
    copy_bill->enable       = object_bill->enable;

    // copy the paid and owed dividers
    copyDivider(object_bill->PaidHeader, copy_bill->PaidHeader);
    copyDivider(object_bill->OwedHeader, copy_bill->OwedHeader);

    // copy the sub-bills
    for (temp1=object_bill->pSubBill, tail=copy_bill; temp1!=nullptr; temp1=temp1->pSubBill){
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
    copy_bill->PaidHeader = nullptr;
    copy_bill->OwedHeader = nullptr;
    copy_bill->pObjBill   = nullptr;
    copy_bill->pSubBill   = nullptr;
    copy_bill->prev       = nullptr;
    copy_bill->next       = nullptr;

    // copy the basic info
    copy_bill->Number       = object_bill->Number;
    copy_bill->Time         = object_bill->Time;
    copy_bill->Item         = object_bill->Item;
    copy_bill->Money        = object_bill->Money;
    copy_bill->Independent  = object_bill->Independent;
    copy_bill->enable       = object_bill->enable;

    // copy the paid and owed dividers
    copyDivider(object_bill->PaidHeader, copy_bill->PaidHeader);
    copyDivider(object_bill->OwedHeader, copy_bill->OwedHeader);

    return copy_bill;
}


void Bill::copyDivider(Divider* &original_head, Divider* &copyfile_head)
{
    Divider *temp1, *temp2, *tail=nullptr;


    for (temp1=original_head; temp1!=nullptr; temp1=temp1->next){
        temp2         = new Divider;
        temp2->People = temp1->People;
        temp2->Money  = temp1->Money;
        temp2->next   = nullptr;

        if (copyfile_head == nullptr){
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
    if (original->prev == nullptr){
        if (original->next == nullptr){
            head           = replacer;
            tail           = replacer;
            replacer->prev = nullptr;
            replacer->next = nullptr;
        }
        else{
            original->next->prev = replacer;
            replacer->next       = original->next;
            replacer->prev       = nullptr;
            head                 = replacer;
        }
    }
    else{
        if (original->next == nullptr){
            original->prev->next = replacer;
            replacer->next       = nullptr;
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
    original->prev = nullptr;
    original->next = nullptr;
}


void Bill::replaceSubBill(OneBill *original, OneBill *replacer)
{
    if (original->pSubBill == nullptr){
        original->pObjBill->pSubBill = replacer;
        replacer->pObjBill           = original->pObjBill;
        replacer->pSubBill           = nullptr;
    }
    else{
        original->pObjBill->pSubBill = replacer;
        replacer->pObjBill           = original->pObjBill;
        original->pSubBill->pObjBill = replacer;
        replacer->pSubBill           = original->pSubBill;
    }

    original->pSubBill = nullptr;
    original->pObjBill = nullptr;
}


int Bill::saveToFile(QString savingPath, QString obName, QString sbName)
{
    QString objective_path = savingPath + "/" +obName;
    QString subjective_path = savingPath + "/" +sbName;
    OneBill *temp1, *temp2, *host;
    Divider *temp3;
    QFile   obj_wt(objective_path), sub_wt(subjective_path);

    if ((obj_wt.open(QIODevice::WriteOnly|QIODevice::Text)) && (sub_wt.open(QIODevice::WriteOnly|QIODevice::Text))) {
        QTextStream out1(&obj_wt), out2(&sub_wt);

        out1 << "FlowerQBill:Yes\n" << "0:" << sbName << "\n";
        out2 << "FlowerQBill:Yes\n" << "1:" << obName << "\n";

        for (temp1=head; temp1!=nullptr; temp1=temp1->next)
            for (temp2=temp1; temp2!=nullptr; temp2=temp2->pSubBill){

                if (temp2->Independent == true){
                    // output the basic info
                    out1 << "!1" << ((temp2->enable==true)?"1":"0") <<"#" << QString::number(temp2->Number) << "%" << QString::number(temp2->Time)
                         << "^" << temp2->Item << "$"   << QString::number(temp2->Money)  << "@";
                    // output the paid divider
                    for (temp3=temp2->PaidHeader; temp3!=nullptr; temp3=temp3->next){
                        out1 << QString::number(temp3->People) << "~" << QString::number(temp3->Money);
                        if (temp3->next != nullptr)    out1 << "/";
                        else                        out1 << "&";
                    }
                    // output the owed divider
                    for (temp3=temp2->OwedHeader; temp3!=nullptr; temp3=temp3->next){
                        out1 << QString::number(temp3->People) << "~" << QString::number(temp3->Money);
                        if (temp3->next != nullptr)    out1 << "/";
                        else                        out1 << "+";
                    }
                    out1 << "\n";
                }
                else{
                    // output the basic info
                    out2 << "!0" << ((temp2->enable==true)?"1":"0") << "#" << QString::number(temp2->Number) << "%" << QString::number(temp2->Time)
                         << "^" << temp2->Item << "$"   << QString::number(temp2->Money)  << "@&";
                    // output the owed divider
                    for (temp3=temp2->OwedHeader; temp3!=nullptr; temp3=temp3->next){
                        out2 << QString::number(temp3->People) << "~" << QString::number(temp3->Money);
                        if (temp3->next != nullptr)    out2 << "/";
                        else                        out2 << "+";
                    }
                    // find the host bill's number
                    for (host=temp2; host->pObjBill!=nullptr; host=host->pObjBill);
                    // output the host bill number
                    out2 << host->Number << "=\n";
                }
            }

        obj_wt.close();
        sub_wt.close();
        return 1;
    }
    else {
        return 0;
    }
}


int Bill::loadData(QString obPath) {
    /* 根据所指定的主账簿的路径，依次读取主账簿和副账簿。副账簿必须存放在主账簿的统一路径，并且它的名字在主账簿中说明。
        返回值 0: 主账簿打开失败或不是FlowerQ所指定的Bill数据文件类型
              1: 主、副账簿均成功读取
              2: 主账簿成功读取，但是副账簿打开失败或不是FlowerQ所指定的Bill数据文件类型
              3: 主账簿成功读取，副账簿成功打开，但是副账簿中的子单无法在主账簿中找到它相应的主单，说明副账簿与主账簿不匹配*/
    int seperatorIndex = obPath.lastIndexOf('/');
    savingPath = obPath.mid(0,seperatorIndex);
    obName = obPath.mid(seperatorIndex+1);

    if (readFromFile(obPath)==0) {
        deleteAll();
        savingPath = "";    obName = "";   sbName = "";
        return 0;
    }
    else {
        int subOpen= readFromFile(savingPath+"/"+sbName);
        if (subOpen == 0) {
            deleteAll();
            savingPath = "";    obName = "";   sbName = "";
            return 2;
        }
        else if (subOpen == 2) {
            deleteAll();
            savingPath = "";    obName = "";   sbName = "";
            return 3;
        }
        return 1;
    }
}


void Bill::deleteAll() {
    OneBill         *temp1, *temp2;

    for (temp1=head; temp1!=nullptr;){
        temp2 = temp1;
        temp1 = temp1->next;
        deleteIndependentBill(1, temp2);
    }
    head = tail = nullptr;
    savingPath = obName = sbName = "";
}














