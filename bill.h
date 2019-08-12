#ifndef BILL_H
#define BILL_H
#include <QString>
#include "account.h"

struct Divider{
    int      People;
    double   Money;
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
    double          Money;
    Divider         *PaidHeader;
    Divider         *OwedHeader;
    bool            Independent;
    bool            enable;
    OneBill         *pSubBill;
    OneBill         *pObjBill;
    OneBill         *prev;
    OneBill         *next;
};


class Bill
{

public:
    OneBill *head, *tail;
    QString savingPath, obName, sbName;

    int readFromFile(QString);
    int saveToFile(QString, QString, QString);
    void deleteIndependentBill(int, OneBill*);
    void deleteSubBill(int, OneBill*);
    void replaceIndependentBill(OneBill*, OneBill*);
    void replaceSubBill(OneBill*, OneBill*);
    void copyDivider(Divider*&, Divider*&);
    void deleteAll();
    OneBill* findByNumber(int);
    OneBill* copyIndependentBill(OneBill*);
    OneBill* copySubBill(OneBill*);
    int loadData(QString);

    Bill();
    ~Bill();
};

#endif // BILL_H


























