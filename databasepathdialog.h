#ifndef DATABASEPATHDIALOG_H
#define DATABASEPATHDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QGridLayout>
#include <QButtonGroup>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>

class DatabasePathDialog : public QDialog
{
public:
    QGridLayout *lMain;
    QLabel l1;
    QRadioButton b1, b2;
    QButtonGroup g1;
    QLineEdit e1;
    QPushButton p1, p2, p3;
    QString returnPath;

    DatabasePathDialog(QString);
    ~DatabasePathDialog();
    void updateState(int);
    void openPathDialog();
    void acceptExec();
    void cancelExec();
};

#endif // DATABASEPATHDIALOG_H
