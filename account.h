#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QString>

/**
 * @brief 结构体 User：用于Account这个类中，作为账户链表的一个单元。在内存中暂时的存储某个用户的信息。
 * @variable	int 	Number		这个用户的编号；
 * @variable	String 	Name 		这个用户的名字；
 * @variable	double 	Paid		这个用户在当前账簿下总共支付了多少钱；
 * @variable    double 	Owed		这个用户在当前账簿下应该承担多少钱的花销；
 * @variable    double 	Balance		这个用户在当前账簿下的净余额是多少。Balance = Owed - Paid 。正数表示在清帐的时候要向外转账，负数表示在清帐的时候应该得到别人的转账；
 * @variable	User* 	next		指向链表中后一个对象的指针；
 * @variable	User* 	prev		指向链表中前一个对象的指针；
 */

struct User
{
    int      Number;
    double   Paid,    Owed, Balance;
    QString  Name;
    bool     isFreezed;
    User    *next,   *prev;
};

/**
 * @brief 类 Account：是
 *
 */

class Account
{
public:

    int     total_num, highestNumber;
    User    *head, *tail;
    QString  SavingFilePath;

    int readFromFile(QString);
    int saveToFile(QString);
    User* FindByNumber(int);
    void clearALl();
    void copy(Account*);

    Account();
    Account(QString);
    ~Account();
};

#endif // ACCOUNT_H
