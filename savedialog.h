#ifndef SAVEDIALOG_H
#define SAVEDIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>

class SaveDialog : public QDialog
{
public:
    QGridLayout *ly1;
    QLabel l1, l2, l3;
    QPushButton b1, b4, b5;
    QLineEdit e1, e2, e3;
    int command;

    SaveDialog(int);
    void searchExec();
    void acceptExec();
    void cancelExec();
    ~SaveDialog();
};

#endif // SAVEDIALOG_H
