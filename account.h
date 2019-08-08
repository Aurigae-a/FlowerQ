#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QString>

struct User
{
    int      Number;
    float    Paid,    Owed, Balance;
    QString  Name;
    User    *next,   *prev;
};

class Account
{
public:

    int     total_num;
    User    *head, *tail;
    QString  SavingFilePath;

    int ReadFromFile(QString);
    int SaveToFile(QString);
    User* FindByNumber(int);

    Account(QString);
    ~Account();
};

#endif // ACCOUNT_H
