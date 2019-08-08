#ifndef BILL_H
#define BILL_H
#include <QString>
#include "account.h"

struct Divider{
    int      People;
    float    Money;
    Divider *next;
};
/*
struct SubBillReminder{
    int              BillNO;
    SubBillReminder *next;
};
*/
struct OneBill{
    int             Number;
    int             Time;
    QString         Item;
    float           Money;
    Divider         *PaidHeader;
    Divider         *OwedHeader;
    bool            Independent;
    OneBill         *pSubBill;
    OneBill         *pObjBill;
    OneBill         *prev;
    OneBill         *next;
};


class Bill
{

public:
    OneBill *head, *tail;
    QString objective_saving_path, subjective_saving_path;

    void readFromFile(QString);
    void saveToFile(QString, QString);
    void deleteIndependentBill(int, OneBill*);
    void deleteSubBill(int, OneBill*);
    void replaceIndependentBill(OneBill*, OneBill*);
    void replaceSubBill(OneBill*, OneBill*);
    void copyDivider(Divider*&, Divider*&);
    OneBill* findByNumber(int);
    OneBill* copyIndependentBill(OneBill*);
    OneBill* copySubBill(OneBill*);

    Bill(QString, QString);
    ~Bill();
};

#endif // BILL_H


























