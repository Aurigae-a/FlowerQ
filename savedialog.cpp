#include "savedialog.h"

SaveDialog::SaveDialog(int cmd)
{
    command = cmd;

    ly1 = new QGridLayout();
    l1.setText("Path name");
    b1.setText("Search");
    b4.setText("Accept");
    b5.setText("Cancel");
    ly1->addWidget(&l1, 0, 0, 1, 1);
    ly1->addWidget(&e1, 0, 1, 1, 1);
    ly1->addWidget(&b1, 0, 2, 1, 1);
    ly1->addWidget(&l2, 1, 0, 1, 1);
    ly1->addWidget(&e2, 1, 1, 1, 1);

    // cmd=0: account保存界面，cmd=1:bill保存界面
    if (cmd == 0) {
        l2.setText("Account file name");
        ly1->addWidget(&b4, 2, 0, 1, 1);
        ly1->addWidget(&b5, 2, 2, 1, 1);
    }
    else {
        l2.setText("Main bill file name");
        l3.setText("Sub-bill file name");
        ly1->addWidget(&l3, 2, 0, 1, 1);
        ly1->addWidget(&e3, 2, 1, 1, 1);
        ly1->addWidget(&b4, 3, 0, 1, 1);
        ly1->addWidget(&b5, 3, 2, 1, 1);
    }

    connect(&b1, &QPushButton::clicked, this, &SaveDialog::searchExec);
    connect(&b4, &QPushButton::clicked, this, &SaveDialog::acceptExec);
    connect(&b5, &QPushButton::clicked, this, &SaveDialog::cancelExec);

    this->setLayout(ly1);

}


SaveDialog::~SaveDialog() {
    delete ly1;
}


void SaveDialog::searchExec() {
    QString path;
    int seperatorIndex;

    if (command == 0) {
        path = QFileDialog::getSaveFileName(this, "Choose the account file", ".", nullptr, nullptr, QFileDialog::DontConfirmOverwrite);
        seperatorIndex = path.lastIndexOf('/');
        e1.setText(path.mid(0, seperatorIndex));
        e2.setText(path.mid(seperatorIndex+1));
    }
    if (command == 1) {
        path = QFileDialog::getSaveFileName(this, "Choose the master bill file", ".", nullptr, nullptr, QFileDialog::DontConfirmOverwrite);
        seperatorIndex = path.lastIndexOf('/');
        e1.setText(path.mid(0, seperatorIndex));
        e2.setText(path.mid(seperatorIndex+1));
        e3.setText(path.mid(seperatorIndex+1)+"Subbill");
    }

}


void SaveDialog::acceptExec() {
    done(Accepted);
}


void SaveDialog::cancelExec() {
    done(Rejected);
}






