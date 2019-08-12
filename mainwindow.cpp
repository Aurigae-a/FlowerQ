#include "mainwindow.h"
#include "accountdialog.h"
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    accountChanged = billChanged = false;

    QStringList t1Header, t2Header;

    // set basic main window
    this->setWindowTitle("BillPay Application");
//    this->resize(1440, 800);

    // set main menu
    pMenuBar       = menuBar();
    pMenu1_File    = pMenuBar->addMenu("File");
    pMenu1_Account = pMenuBar->addMenu("Account");
    pMenu1_Help    = pMenuBar->addMenu("Help");
    pAccount       = pMenu1_Account->addAction("account");
    pVersion       = pMenu1_Help->addAction("version");
    pNewAccount    = pMenu1_File->addAction("new/open account");
    pNewBill       = pMenu1_File->addAction("new/open bill");
    pMenu1_File->addSeparator();
    pSaveAccount   = pMenu1_File->addAction("save account");
    pSaveAsAccount = pMenu1_File->addAction("save account as");
    pMenu1_File->addSeparator();
    pSaveBill      = pMenu1_File->addAction("save bill");
    pSaveAsBill    = pMenu1_File->addAction("save bill as");



    // set the widgets on the main window
    CenterWindow = new QWidget;
    t1  = new QTableWidget(0,7);
    t1->horizontalHeader()->setVisible(true);
    t1->verticalHeader()->setVisible(false);
    t1Header << "NO." << "Time" << "Items" << "Money $" << "Paid By" << "Owed To" << "Indepedence";
    t1->setHorizontalHeaderLabels(t1Header);
    t1->setSelectionBehavior(QAbstractItemView::SelectRows);
    t1->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t1->setSelectionMode(QAbstractItemView::SingleSelection);

    t2  = new QTableWidget(1,5);
    t2->horizontalHeader()->setVisible(true);
    t2->verticalHeader()->setVisible(false);
    t2Header << "NO." << "Name" << "Money Out $" << "Money Owed $" << "Balance $";
    t2->setHorizontalHeaderLabels(t2Header);
    t2->setSelectionBehavior(QAbstractItemView::SelectRows);
    t2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t2->setSelectionMode(QAbstractItemView::SingleSelection);

    l1.setText("Bills:");
    l2.setText("Accounts:");

    b1.setText("New");
    b2.setText("Delete");
    b3.setText("Change");
    b4.setText("Calculate");
    b5.setText("Clear!");

    ly1 = new QGridLayout;
    ly1->addWidget(&l1,0,0,1,1);
    ly1->addWidget(&l2,0,7,1,1);
    ly1->addWidget( t1,1,0,5,7);
    ly1->addWidget( t2,1,7,5,5);
    ly1->addWidget(&b1,6,1,1,1);
    ly1->addWidget(&b2,6,3,1,1);
    ly1->addWidget(&b3,6,5,1,1);
    ly1->addWidget(&b4,6,8,1,1);
    ly1->addWidget(&b5,6,10,1,1);
    CenterWindow->setLayout(ly1);
    this->setCentralWidget(CenterWindow);

    connect(pAccount, &QAction::triggered, this, &MainWindow::accountExec);
    connect(pNewAccount, &QAction::triggered, this, &MainWindow::newOpenAccount);
    connect(pNewBill, &QAction::triggered, this, &MainWindow::newOpenBill);
    connect(pSaveAccount, &QAction::triggered, this, &MainWindow::accountSave);
    connect(pSaveAsAccount, &QAction::triggered, this, &MainWindow::accountSaveAs);
    connect(pSaveBill, &QAction::triggered, this, &MainWindow::billSave);
    connect(pSaveAsBill, &QAction::triggered, this, &MainWindow::billSaveAs);
    connect(&b1, &QPushButton::clicked, this, &MainWindow::newBillExec);
    connect(&b2, &QPushButton::clicked, this, &MainWindow::deleteBillExec);
    connect(&b3, &QPushButton::clicked, this, &MainWindow::changeBillExec);
    connect(&b4, &QPushButton::clicked, this, &MainWindow::calculateExec);
    connect(&b5, &QPushButton::clicked, this, &MainWindow::clearExec);

    /* 启动前，先读取用户册信息。会弹出一个询问加载方式的对话框，如果该对话框的返回是'!'，说明是要新建一个空的用户册；如果该对话框返回的是'？'，则表示用户点击了Cancel，
        这时，要弹出消息对话框警告用户没有正确的加载用户册，然后创建一个空的用户册；如果返回的是一个字符串，则表示用户选择了加载已有的用户册，通过readFromFile的方式进
        行加载。如果readFromFile返回的是0，表示未能正确加载文件，这个时候弹出消息对话框提示用户打开错误，然后返回开始处重新询问文件路径。如果返回1，则表明正确加载*/
    bool hasFinished = false;
    while(!hasFinished) {
        DatabasePathDialog *dpd1 = new DatabasePathDialog("Account path");
        dpd1->exec();
        if (QString::compare(dpd1->returnPath, "!") == 0) {
            current_account = new Account();
            accountSavingPath = acName = "";
            hasFinished = true;
        }
        else if (QString::compare(dpd1->returnPath, "?") == 0) {
            QMessageBox::warning(this, "Warning", "The account is not loaded correctly, app creates an empty account instead", QMessageBox::Ok);
            current_account = new Account();
            accountSavingPath = acName = "";
            hasFinished = true;
        }
        else {
            current_account = new Account();
            if (current_account->readFromFile(dpd1->returnPath) == 0) {
                current_account->clearALl();
                QMessageBox::warning(this, "Warning", "The target file cannot be loaded successfully. Please check the file format.", QMessageBox::Ok);
            }
            else{
                hasFinished = true;
                int seperatorIndex = dpd1->returnPath.lastIndexOf("/");
                accountSavingPath = dpd1->returnPath.mid(0,seperatorIndex);
                acName = dpd1->returnPath.mid(seperatorIndex+1);
            }
        }
        delete dpd1;
    }

    /* 在程序启动之初，先要加载账簿信息。弹出对话框询问主账簿数据文件的保存路径。对话框返回的若是!，则说明用户想新建一个账簿；返回的若是?，则说明用户点击了Cancel，这时应当
        先弹出消息对话框警告用户账簿没有正确加载，然后再自动创建一个空账簿；如果是其他情况，就可以尝试的加载账簿数据文件，如果返回值是1，就表明主副账簿均加载成功；如果返回值
        是0，表明主账簿打开失败，指定的文档不存在或不是正确的FlowerQ所规定的Bill格式；如果返回值是2，表明主账簿打开成功，但副账簿不存在或不是正确的FlowerQ所规定的Bill格
        式如果返回值是2，则表明副账簿与主账簿不匹配*/
    hasFinished = false;
    while(!hasFinished) {
        DatabasePathDialog *dpd1 = new DatabasePathDialog("Main bill path");
        dpd1->exec();
        current_bill = new Bill();

        if (QString::compare(dpd1->returnPath, "!") == 0) {
            hasFinished = true;
            billSavingPath = obName = sbName = "";
        }
        else if (QString::compare(dpd1->returnPath, "?") == 0) {
            QMessageBox::warning(this, "Warning", "The bill is not loaded correctly, app creates an empty account instead", QMessageBox::Ok);
            hasFinished = true;
            billSavingPath = obName = sbName = "";
        }
        else {
            int loadResult = current_bill->loadData(dpd1->returnPath);
            if (loadResult == 0) {
                QMessageBox::warning(this, "Warning", "Meet an error when opening the main bill, the target file does not exist or is not the correct file format.", QMessageBox::Ok);
            }
            else if (loadResult == 2) {
                QMessageBox::warning(this, "Warning", "Main bill loaded successfully. But, meet an error when opening the sub-bill, the sub-bill file does not exist or is not the correct file format.", QMessageBox::Ok);
            }
            else if (loadResult == 3) {
                QMessageBox::warning(this, "Warning", "The main bill and the sub-bill doesn't match with each other.", QMessageBox::Ok);
            }
            else{
                hasFinished = true;
                billSavingPath = current_bill->savingPath;
                obName = current_bill->obName;
                sbName = current_bill->sbName;
            }
        }
        delete dpd1;
    }
    calculateTotal();
    updateAccountTable();
    updateBillTable(); updateBillTable();
}

MainWindow::~MainWindow()
{
    //current_bill->saveToFile(OBillFileAddress, SBillFileAddress);

    clearTable(t1);
    delete t1;
    delete t2;
    delete ly1;

    delete current_bill;
    delete current_account;

}

// Account
void MainWindow::accountExec()
{
    AccountDialog *dlg;

    /* 将当前用户册传送给用户册修改对话框，当对话框以Accept的方式结束的时候，表明用户同意了修改的内容，这时候清除当前用户册，并复制对话框中的用户册。但是此时先不对数据文件
        进行修改，等到整个程序结束的时候，再询问用户是否进行文件的修改*/
    dlg = new AccountDialog(current_account);
    if (dlg->exec() == QDialog::Accepted) {
        current_account->copy(dlg->current_account);
        accountChanged = true;

        // calculate the paid, owed, and balance, and then update the account table
        calculateTotal();
        updateAccountTable();
        updateBillTable();
    }
    delete dlg;
}


void MainWindow::updateBillTable()
{
    OneBill          *temp1, *temp2, *host_bill=nullptr;
    int              count, m1, colorNum[3];
    QTableWidgetItem *it;
    QString          temp3, temp5;
    Divider          *temp4;


    clearTable(t1);
    if (current_bill->head == nullptr);
    else{
        // read total number of the active bills
        for (count=0, temp1=current_bill->head; temp1!=nullptr; temp1=temp1->next)
            for (temp2=temp1; temp2!=nullptr; temp2=temp2->pSubBill)    count++;

        t1->setRowCount(count);
        for (m1=0, temp1=current_bill->head; temp1!=nullptr; temp1=temp1->next)
            for (temp2=temp1; temp2!=nullptr; temp2=temp2->pSubBill, m1++){
                if (temp2->enable == true) {
                    colorNum[0] = 255;  colorNum[1] = 255;  colorNum[2] = 255;
                }
                else {
                    colorNum[0] = 255;  colorNum[1] = 0;  colorNum[2] = 0;
                }

                // set the number
                it = new QTableWidgetItem;
                t1->setItem(m1,0,it);
                it->setText(QString::number(temp2->Number));
                it->setTextColor(QColor(0,0,0));
                it->setBackgroundColor(QColor(colorNum[0],colorNum[1],colorNum[2]));

                // set the time
                it = new QTableWidgetItem;
                t1->setItem(m1,1,it);
                it->setText(QString::number(temp2->Time));
                it->setTextColor(QColor(0,0,0));
                it->setBackgroundColor(QColor(colorNum[0],colorNum[1],colorNum[2]));

                // find the number of the host bill for a sub-bill
                if (temp2->Independent == false){
                    for (host_bill=temp2; host_bill->pObjBill!=nullptr; host_bill=host_bill->pObjBill);
                }

                // set the item
                it = new QTableWidgetItem;
                t1->setItem(m1,2,it);
                if (temp2->Independent == true)     it->setText(temp2->Item);
                else                                it->setText(QString::number(host_bill->Number) + ":\n" + temp2->Item);
                it->setTextColor(QColor(0,0,0));
                it->setBackgroundColor(QColor(colorNum[0],colorNum[1],colorNum[2]));

                // set the money
                it = new QTableWidgetItem;
                t1->setItem(m1,3,it);
                it->setText(QString::number(temp2->Money));
                it->setTextColor(QColor(0,0,0));
                it->setBackgroundColor(QColor(colorNum[0],colorNum[1],colorNum[2]));

                // set the paid info
                it = new QTableWidgetItem;
                t1->setItem(m1,4,it);
                it->setTextColor(QColor(0,0,0));
                it->setBackgroundColor(QColor(colorNum[0],colorNum[1],colorNum[2]));

                if (temp2->PaidHeader != nullptr){
                    for(temp3="", temp4=temp2->PaidHeader; temp4!=nullptr; temp4=temp4->next){
                        User *tempPerson = current_account->FindByNumber(temp4->People);
                        if (tempPerson == nullptr)      temp5 = "Person Not Found";
                        else if (tempPerson->isFreezed) temp5 = tempPerson->Name + "(Freezed)";
                        else                            temp5 = tempPerson->Name;

                        temp3 = temp3 + QString::number(temp4->People) + ":" + temp5 + "\t$"+QString::number(temp4->Money) + "\n";
                    }
                    it->setText(temp3);
                }

                // set the owed info
                it = new QTableWidgetItem;
                t1->setItem(m1,5,it);
                it->setTextColor(QColor(0,0,0));
                it->setBackgroundColor(QColor(colorNum[0],colorNum[1],colorNum[2]));

                if (temp2->OwedHeader != nullptr){
                    for(temp3="", temp4=temp2->OwedHeader; temp4!=nullptr; temp4=temp4->next){
                        User *tempPerson = current_account->FindByNumber(temp4->People);
                        if (tempPerson == nullptr)      temp5 = "Person Not Found";
                        else if (tempPerson->isFreezed) temp5 = tempPerson->Name + "(Freezed)";
                        else                            temp5 = tempPerson->Name;

                        temp3 = temp3 + QString::number(temp4->People) + ":" + temp5 + "\t$"+QString::number(temp4->Money) + "\n";
                    }
                    it->setText(temp3);
                }

                // set the independence info
                it = new QTableWidgetItem;
                t1->setItem(m1,6,it);
                if (temp2->Independent == true) it->setText("Yes");
                else                            it->setText("No");
                it->setTextColor(QColor(0,0,0));
                it->setBackgroundColor(QColor(colorNum[0],colorNum[1],colorNum[2]));

            }
            t1->resizeRowsToContents();
            t1->resizeColumnsToContents();
        }
}


void MainWindow::clearTable(QTableWidget* t)
{
    int row, col, m1, m2;

    row = t->rowCount();
    col = t->columnCount();

    for (m1=0; m1<row; m1++){
        for (m2=0; m2<col; m2++){
            delete t->item(m1, m2);
        }
    }

    t->setRowCount(0);
//    t->setColumnCount(0);
}


void MainWindow::newBillExec()
{
//    qDebug() << "Fuck!";
    OneBill *temp1;

    // create and initialize a new OneBill
    temp1                = new OneBill;
    if (current_bill->tail==nullptr)   temp1->Number = 10000001;
    else                            temp1->Number = current_bill->tail->Number+1;
    temp1->Time          = 0;
    temp1->Item          = "";
    temp1->Money         = 0.0;
    temp1->PaidHeader    = nullptr;
    temp1->OwedHeader    = nullptr;
    temp1->pSubBill      = nullptr;
    temp1->pObjBill      = nullptr;
    temp1->prev          = nullptr;
    temp1->next          = nullptr;
    temp1->Independent   = true;
    temp1->enable        = true;

    // create a dialog and send this new OneBill to the dialog for modifying
    dlg1 = new BillEditDialog(temp1, current_account, current_bill);

    // if accept, then link this new OneBill to the tail of the current bill, if not, then delete this new OneBill
    if (dlg1->exec() == QDialog::Accepted){
        if (current_bill->head == nullptr){
            current_bill->head       = temp1;
            current_bill->tail       = temp1;
        }
        else{
            current_bill->tail->next = temp1;
            temp1->prev              = current_bill->tail;
            current_bill->tail       = temp1;
        }
        billChanged = true;
    }
    else{
        current_bill->deleteIndependentBill(2, temp1);
    }

    calculateTotal();
    updateBillTable();
    updateAccountTable();

    delete dlg1;
}


void MainWindow::changeBillExec()
{
    int row;
    OneBill *temp1, *temp2;

    // read the number of the selected row
    row   = t1->currentIndex().row();
    if (row == -1)  return;

    temp1 = current_bill->findByNumber(t1->item(row,0)->text().toInt());
    if (temp1->enable==false) {
        QMessageBox::information(this,"Warning","You cannot change this bill, because this bill has been cleared.",QMessageBox::Ok);
        return;
    }

    if (temp1->Independent == true){
        // make a copy of the selected one
        temp2 = current_bill->copyIndependentBill(temp1);

        // send the copyfile to the dialog for modifying
        dlg1 = new BillEditDialog(temp2, current_account, current_bill);

        // if accept, replace the old OneBill with the new one, and then delete the old one, if not, then just delete the new one
        if (dlg1->exec() == QDialog::Accepted){
            current_bill->replaceIndependentBill(temp1, temp2);
            current_bill->deleteIndependentBill(2, temp1);
            billChanged = true;
        }
        else{
            current_bill->deleteIndependentBill(2, temp2);
        }
        delete dlg1;

        calculateTotal();
        updateBillTable();
        updateAccountTable();
    }
    else{
        QMessageBox *dlg2 = new QMessageBox(QMessageBox::Warning, "Warning",
                                            "You cannot change the sub-bills directly, please change it from its host bill.",
                                            QMessageBox::Ok);
        dlg2->exec();
        delete dlg2;
    }
}


void MainWindow::deleteBillExec()
{
    int row;
    OneBill *temp1;

    // read the number of the selected row
    row   = t1->currentIndex().row();
    if (row == -1)      return;

    temp1 = current_bill->findByNumber(t1->item(row,0)->text().toInt());

    if (temp1->Independent == true){
        // delete the selected independent bill
        current_bill->deleteIndependentBill(1, temp1);
        billChanged = true;
        calculateTotal();
        updateBillTable();
        updateAccountTable();
    }
    else{
        QMessageBox *dlg2 = new QMessageBox(QMessageBox::Warning, "Warning",
                                            "You cannot delete the sub-bills directly, please delete it from its host bill.",
                                            QMessageBox::Ok);
        dlg2->exec();
        delete dlg2;
    }

}


void MainWindow::calculateTotal()
{
    User    *temp1;
    OneBill *temp2, *temp3;
    Divider *temp4;
    // clear all accounts
    for (temp1=current_account->head; temp1!=nullptr; temp1=temp1->next){
        temp1->Owed    = 0.0;
        temp1->Paid    = 0.0;
        temp1->Balance = 0.0;
    }
    // go through each bill, add their values to their corresponding accounts
    for (temp2=current_bill->head; temp2!=nullptr; temp2=temp2->next){
        for (temp3=temp2; temp3!=nullptr; temp3=temp3->pSubBill){
            if (temp3->enable == false)     continue;

            // record the paid info
            for (temp4=temp3->PaidHeader; temp4!=nullptr; temp4=temp4->next){
                temp1 = current_account->FindByNumber(temp4->People);
                if (temp1==nullptr)     continue;
                temp1->Paid += temp4->Money;
            }
            // record the owed info
            for (temp4=temp3->OwedHeader; temp4!=nullptr; temp4=temp4->next){
                temp1 = current_account->FindByNumber(temp4->People);
                if (temp1==nullptr)     continue;
                temp1->Owed += temp4->Money;
            }
        }
    }
    // calculate the balance
    for (temp1=current_account->head; temp1!=nullptr; temp1=temp1->next)
        temp1->Balance = temp1->Owed - temp1->Paid;
}


void MainWindow::updateAccountTable()
{
    QTableWidgetItem *it;
    User             *temp1;
    int               m1,    count;

    // clear the account table
    clearTable(t2);
    // get the row number
    for (count=0, temp1=current_account->head; temp1!=nullptr; temp1=temp1->next)
        if (temp1->isFreezed==false)    count++;
    t2->setRowCount(count);
    // set the updated account table
    for (m1=0, temp1=current_account->head; temp1!=nullptr; temp1=temp1->next){
        if (temp1->isFreezed==false) {
            it = new QTableWidgetItem;
            t2->setItem(m1, 0, it);
            it->setText(QString::number(temp1->Number));
            it->setTextColor(QColor(0,0,0));
            it->setBackgroundColor(QColor(255,255,255));

            it = new QTableWidgetItem;
            t2->setItem(m1, 1, it);
            it->setText(temp1->Name);
            it->setTextColor(QColor(0,0,0));
            it->setBackgroundColor(QColor(255,255,255));

            it = new QTableWidgetItem;
            t2->setItem(m1, 2, it);
            it->setText(QString::number(temp1->Paid));
            it->setTextColor(QColor(0,0,0));
            it->setBackgroundColor(QColor(255,255,255));

            it = new QTableWidgetItem;
            t2->setItem(m1, 3, it);
            it->setText(QString::number(temp1->Owed));
            it->setTextColor(QColor(0,0,0));
            it->setBackgroundColor(QColor(255,255,255));

            it = new QTableWidgetItem;
            t2->setItem(m1, 4, it);
            it->setText(QString::number(temp1->Balance));
            it->setTextColor(QColor(0,0,0));
            it->setBackgroundColor(QColor(255,255,255));

            m1++;
        }
    }
    t2->resizeRowsToContents();
    t2->resizeColumnsToContents();
}


void MainWindow::closeEvent(QCloseEvent* event1) {
    QPushButton *yesButton, *noButton, *cancelButton;
    QMessageBox *msg1;
    int saveResult=0;

    msg1 = new QMessageBox(this);
    msg1->setIcon(QMessageBox::Question);
    msg1->setWindowTitle("Quiting");
    yesButton = msg1->addButton("Yes",QMessageBox::ActionRole);
    noButton = msg1->addButton("No",QMessageBox::ActionRole);
    cancelButton = msg1->addButton("Cancel",QMessageBox::ActionRole);

    if (accountChanged) {
        msg1->setText("The user account has been changed and not yet saved, would you like to save the change?");
        msg1->exec();
        if (msg1->clickedButton() == yesButton) {
            do{
                saveResult = accountSave();
            } while(saveResult==2);
            if (saveResult == 0) {
                delete msg1;
                return event1->ignore();
            }
        }
        else if (msg1->clickedButton() == noButton) {}
        else {
            delete msg1;
            return event1->ignore();
        }
    }

    if (billChanged) {
        msg1->setText("The bill has been changed and not yet saved, would you like to save the change?");
        msg1->exec();
        if (msg1->clickedButton() == yesButton) {
            do{
                saveResult = billSave();
            } while(saveResult==2);
            if (saveResult == 0) {
                delete msg1;
                return event1->ignore();
            }
        }
        else if (msg1->clickedButton() == noButton) {}
        else {
            delete msg1;
            return event1->ignore();
        }
    }
    delete msg1;
    event1->accept();
}


int MainWindow::accountSaveAs(){
    SaveDialog *dlg1;
    QString savingPath1;
    int ret;

    dlg1 = new SaveDialog(0);
    if (dlg1->exec()==QDialog::Accepted){
        // 弹出对话框以accept的方式结束
        savingPath1 = dlg1->e1.text() + "/" + dlg1->e2.text();
        if (current_account->saveToFile(savingPath1) == 0) {
            // 保存失败，文件未能正常打开
            QMessageBox::warning(this, "warning", "Cannot write account data to the target file, please check and choose the saving path again.",
                                 QMessageBox::Ok);
            ret = 2;
        }
        else {// 保存成功
            accountSavingPath = dlg1->e1.text();
            acName = dlg1->e2.text();
            accountChanged = false;
            QMessageBox::information(this, "", "Account file is saved successfully.", QMessageBox::Ok);
            ret = 1;
        }
    }
    else {
        // 弹出对话框以cancel的方式结束
        QMessageBox::information(this, "", "Account file has not been saved.", QMessageBox::Ok);
        ret = 0;
    }
    delete dlg1;
    return ret;
}


int MainWindow::accountSave() {
    if (accountSavingPath.compare("")==0) {
        return accountSaveAs();
    }else {
        if (current_account->saveToFile(accountSavingPath+"/"+acName) == 0) {
            // 保存失败，文件未能正常打开
            QMessageBox::warning(this, "warning", "Cannot write account data to the target file, please check and choose the saving path again.",
                                 QMessageBox::Ok);
            return 2;
        }
        else {// 保存成功
            accountChanged = false;
            return 1;
        }
    }
}


int MainWindow::billSaveAs() {
    SaveDialog *dlg1;
    int ret;

    dlg1 = new SaveDialog(1);
    if (dlg1->exec()==QDialog::Accepted){
        // 弹出对话框以accept的方式结束
        if (current_bill->saveToFile(dlg1->e1.text(), dlg1->e2.text(), dlg1->e3.text()) == 0) {
            // 保存失败，文件未能正常打开
            QMessageBox::warning(this, "warning", "Cannot write bill data to the target file, please check and choose the saving path again.",
                                 QMessageBox::Ok);
            ret = 2;
        }
        else {// 保存成功
            billSavingPath = dlg1->e1.text();
            obName = dlg1->e2.text();
            sbName = dlg1->e3.text();
            billChanged = false;
            QMessageBox::information(this, "", "Bill file is saved successfully.",
                                 QMessageBox::Ok);
            ret = 1;
        }
    }
    else {
        // 弹出对话框以cancel的方式结束
        QMessageBox::information(this, "", "Bill file has not been saved.", QMessageBox::Ok);
        ret = 0;
    }
    delete dlg1;
    return ret;
}


int MainWindow::billSave() {
    if (billSavingPath.compare("")==0) {
        return billSaveAs();
    }else {
        if (current_bill->saveToFile(billSavingPath, obName, sbName) == 0) {
            // 保存失败，文件未能正常打开
            QMessageBox::warning(this, "warning", "Cannot write bill data to the target file, please check and choose the saving path again.",
                                 QMessageBox::Ok);
            return 2;
        }
        else {// 保存成功
            billChanged = false;
            return 1;
        }
    }
}


void MainWindow::newOpenAccount() {
    QPushButton *yesButton, *noButton, *cancelButton;
    QMessageBox *msg1;

    if (accountChanged) {
        // 首先对当前的用户册进行检查，如果已经修改过了，就需要询问是否保存，如果选择yes，则跳入保存模块，如果在保存过程中出现了cancel的情况，那么虽然跳出了保存过程，但在
        //  提示完未保存成功后，仍然进入打开阶段；如果选择no，则不进行保存，直接进入打开阶段；如果选择cancel，则结束打开操作，直接回到原来的用户册。
        msg1 = new QMessageBox(this);
        msg1->setIcon(QMessageBox::Question);
        msg1->setText("The user account has been changed and not yet saved, would you like to save the change?");
        msg1->setWindowTitle("Quiting");
        yesButton = msg1->addButton("Yes",QMessageBox::ActionRole);
        noButton = msg1->addButton("No",QMessageBox::ActionRole);
        cancelButton = msg1->addButton("Cancel",QMessageBox::ActionRole);

        if (msg1->clickedButton() == yesButton) {
            accountSave();
        }
        else if (msg1->clickedButton() == noButton) {}
        else {
            delete msg1;
            return;
        }
        delete msg1;
    }

    // 进入文件新建/打开阶段
    DatabasePathDialog *dpd1 = new DatabasePathDialog("Account path");
    dpd1->exec();
    if (QString::compare(dpd1->returnPath, "!") == 0) { // 新建一个空用户册
        current_account->clearALl();
        accountSavingPath = acName = "";
        accountChanged = false;
    }
    else if (QString::compare(dpd1->returnPath, "?") == 0) { /*取消'打开/新建'操作*/ }
    else { // 从已有文件中打开
        Account *tempAccount = new Account();
        if (tempAccount->readFromFile(dpd1->returnPath) == 0) { // 打开失败
            delete tempAccount;
            QMessageBox::warning(this, "Warning", "The target file cannot be loaded successfully. Please check the file format.", QMessageBox::Ok);
        }
        else{ // 打开成功
            delete current_account;
            current_account = tempAccount;

            int seperatorIndex = dpd1->returnPath.lastIndexOf("/");
            accountSavingPath = dpd1->returnPath.mid(0,seperatorIndex);
            acName = dpd1->returnPath.mid(seperatorIndex+1);
            accountChanged = false;
        }
    }
    delete dpd1;

    calculateTotal();
    updateAccountTable();
    updateBillTable(); updateBillTable();
}


void MainWindow::newOpenBill() {
    QPushButton *yesButton, *noButton, *cancelButton;
    QMessageBox *msg1;

    if (billChanged) {
        // 首先对当前的账簿进行检查，如果已经修改过了，就需要询问是否保存，如果选择yes，则跳入保存模块，如果在保存过程中出现了cancel的情况，那么虽然跳出了保存过程，但在
        //  提示完未保存成功后，仍然进入打开阶段；如果选择no，则不进行保存，直接进入打开阶段；如果选择cancel，则结束打开操作，直接回到原来的账簿。
        msg1 = new QMessageBox(this);
        msg1->setIcon(QMessageBox::Question);
        msg1->setText("The user bill has been changed and not yet saved, would you like to save the change?");
        msg1->setWindowTitle("Quiting");
        yesButton = msg1->addButton("Yes",QMessageBox::ActionRole);
        noButton = msg1->addButton("No",QMessageBox::ActionRole);
        cancelButton = msg1->addButton("Cancel",QMessageBox::ActionRole);

        if (msg1->clickedButton() == yesButton) {
            billSave();
        }
        else if (msg1->clickedButton() == noButton) {}
        else {
            delete msg1;
            return;
        }
        delete msg1;
    }

    // 进入文件新建/打开阶段
    DatabasePathDialog *dpd1 = new DatabasePathDialog("Main bill path");
    dpd1->exec();

    if (QString::compare(dpd1->returnPath, "!") == 0) { // 新建
        current_bill->deleteAll();
        billSavingPath = obName = sbName = "";
    }
    else if (QString::compare(dpd1->returnPath, "?") == 0) { /*取消新建操作*/ }
    else { // 打开
        Bill *tempBill = new Bill();
        int loadResult = tempBill->loadData(dpd1->returnPath);
        if (loadResult == 0) { // 主账簿文件打开失败
            delete tempBill;
            QMessageBox::warning(this, "Warning", "Meet an error when opening the main bill, the target file does not exist or is not the correct file format.", QMessageBox::Ok);
        }
        else if (loadResult == 2) { // 副账簿文件打开失败
            delete tempBill;
            QMessageBox::warning(this, "Warning", "Main bill loaded successfully. But, meet an error when opening the sub-bill, the sub-bill file does not exist or is not the correct file format.", QMessageBox::Ok);
        }
        else if (loadResult == 3) { // 主副账簿不匹配
            delete tempBill;
            QMessageBox::warning(this, "Warning", "The main bill and the sub-bill doesn't match with each other.", QMessageBox::Ok);
        }
        else{ // 打开成功
            current_bill->deleteAll();
            current_bill = tempBill;

            billSavingPath = current_bill->savingPath;
            obName = current_bill->obName;
            sbName = current_bill->sbName;
        }
    }
    delete dpd1;

    calculateTotal();
    updateAccountTable();
    updateBillTable(); updateBillTable();
}


void MainWindow::calculateExec() {
    CalculateDialog *clg1 = new CalculateDialog(0, current_account, current_bill);
    clg1->exec();
    delete clg1;
}

void MainWindow::clearExec() {
    CalculateDialog *clg1 = new CalculateDialog(1, current_account, current_bill);
    clg1->exec();
    delete clg1;
    billChanged = true;

    calculateTotal();
    updateAccountTable();
    updateBillTable();
}








