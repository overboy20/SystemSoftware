#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "process.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QProcess>
#include "QStandardItemModel"
#include "QStandardItem"
#include <QDir>
#include <QStringListModel>
#include <vector>

std::vector <process*> PROC;
int MemoryTotal, SwapTotal;
//-------------------------------------------------
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    //заповнення таблиці з процесами
    ui->setupUi(this);
    ui->tabWidget->setTabText(0, "Process");
    ui->tabWidget->setTabText(1, "CPU");
    ui->tabWidget->setTabText(2, "Operating system");
    ui->tabWidget->setTabText(3, "Memory");
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    RefreshList();
    ui->tableView->resizeColumnsToContents();

    //інформація про процесор
    QFile file("/proc/cpuinfo");
    if (!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "info", file.errorString());
    QTextStream in(&file);
    ui->textBrowser->setText(in.readAll());

    //інформація про операційну систему
    QFile fileOS("/proc/version_signature");
    if (!fileOS.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "info", fileOS.errorString());
    QTextStream in2(&fileOS);
    ui->labelOS->setText(ui->labelOS->text()+in2.readAll());

    QFile fileKernel("/proc/version");
    if (!fileKernel.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "info", fileKernel.errorString());
    QTextStream in3(&fileKernel);
    QString strKernel = in3.readAll(), kernel;
    std::string stdstrKernel = strKernel.toStdString();
    int index = 0;
    //поки не буде символу "("
    while(stdstrKernel[index] != 40) {
        kernel.append(stdstrKernel[index]);
        index++;
    }

    //************************************************************************
    //*************************MEMORY TAB*************************************
    //************************************************************************
    ui->progressBarMem->setMinimum(0);
    ui->progressBarSwp->setMinimum(0);

    //get maximum values
    QFile filememory("/proc/meminfo");
    if (!filememory.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "info", filememory.errorString());
    QTextStream in4(&filememory);
    QString strMemory = in4.readAll();
    std::string stdstrMemory = strMemory.toStdString();

    int maxMempos = strMemory.indexOf("MemTotal: ");
    QString memstr;
    while (stdstrMemory[maxMempos + 10] != 'k'){
        if (stdstrMemory[maxMempos + 10] > 47 && stdstrMemory[maxMempos + 10] < 58)
            memstr.append(stdstrMemory[maxMempos + 10]);
        maxMempos++;
    }
    MemoryTotal = memstr.toInt();

    int maxSwppos = strMemory.indexOf("SwapTotal: ");
    QString swpstr;
    while (stdstrMemory[maxSwppos + 11] != 'k') {
        if (stdstrMemory[maxSwppos + 11] > 47 && stdstrMemory[maxSwppos + 11] < 58)
            swpstr.append(stdstrMemory[maxSwppos + 11]);
        maxSwppos++;
    }
    SwapTotal = swpstr.toInt();

    ui->progressBarMem->setMaximum(MemoryTotal);
    ui->progressBarSwp->setMaximum(SwapTotal);


    QObject::connect(&timer, SIGNAL(timeout()), SLOT(TimerTick()));
    timer.setInterval(1000); //ms
    timer.start();
    //****************************************************************************

    ui->labelKernel->setText(ui->labelKernel->text()+kernel);
    QPixmap pixmap(":/img/Linux-header.png");
    ui->labelImgOS->setPixmap(pixmap);
    ui->labelImgOS->show();
    ui->labelMemOf->setText("of " + QString::number(MemoryTotal) + " kB");
    ui->labelSwpOf->setText("of " + QString::number(SwapTotal) + " kB");
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::RefreshList(){
    UpdateVector();
    GenerateModel();
}

QStringList MainWindow::GetProcessList()
{
    QDir directory("/proc/");
    QStringList list(directory.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Unsorted));

    //видалити всі папки, назви яких не є числами
    for (int i = 0; i < list.length(); i++) {
        bool success;
        list.at(i).toInt(&success, 10);
        if (!success) {
            list.removeAt(i);
            i--;
        }
    }
    return list;
}

void MainWindow::UpdateVector(){
    PROC.clear();
    QStringList list = GetProcessList();

    //створюємо об’єкти процесів
    for (int i = 0; i < list.length(); i++) {
        bool ok;
        PROC.push_back(new process(list.at(i).toInt(&ok, 10)));
    }
}

void MainWindow::GenerateModel(){
    QStringList list = GetProcessList();

    QStringList horizontalHeader;
    horizontalHeader.append("PID");
    horizontalHeader.append("Name");
    horizontalHeader.append("Virt");
    horizontalHeader.append("Res");
    horizontalHeader.append("Shr");
    horizontalHeader.append("State");
    horizontalHeader.append("User");
    horizontalHeader.append("Command");
    horizontalHeader.append("Threads");

    QStandardItemModel *mod = new QStandardItemModel;
    QStandardItem *item;

    //Заголовки рядків
    QStringList verticalHeader;
    for (int i = 0; i < list.length(); i++)
        verticalHeader.append(QString::number(i));

    mod->setHorizontalHeaderLabels(horizontalHeader);
    mod->setVerticalHeaderLabels(verticalHeader);

    for (int i = 0; i < list.length(); i++){
        item = new QStandardItem(QString::number(PROC[i]->getPID()));
        mod->setItem(i, 0, item);

        item = new QStandardItem(PROC[i]->getName());
        mod->setItem(i, 1, item);

        item = new QStandardItem(QString::number(PROC[i]->getVirt()));
        mod->setItem(i, 2, item);

        item = new QStandardItem(QString::number(PROC[i]->getRes()));
        mod->setItem(i, 3, item);

        item = new QStandardItem(QString::number(PROC[i]->getShr()));
        mod->setItem(i, 4, item);

        item = new QStandardItem(PROC[i]->getState());
        mod->setItem(i, 5, item);

        item = new QStandardItem(PROC[i]->getUser());
        mod->setItem(i, 6, item);

        item = new QStandardItem(PROC[i]->getCommand());
        mod->setItem(i, 7, item);

        item = new QStandardItem(QString::number(PROC[i]->getThreads()));
        mod->setItem(i, 8, item);
    }

    ui->tableView->setModel(mod);
    //ui->tableView->resizeRowsToContents();
    //ui->tableView->resizeColumnsToContents();
}

void MainWindow::on_pushButton_clicked()
{
    //kill process
    QProcess p;
    int rowidx = ui->tableView->selectionModel()->currentIndex().row();

    if (rowidx > 0)
    {
        p.start("kill -9 " + QString::number(PROC[rowidx]->getPID()));
        p.waitForFinished();
        PROC.erase(PROC.begin()+rowidx);
        RefreshList();
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    RefreshList();
}

void MainWindow::TimerTick(void) {
    QFile filememory("/proc/meminfo");
    if (!filememory.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "info", filememory.errorString());
    QTextStream in4(&filememory);
    QString strMemory = in4.readAll();
    std::string stdstrMemory = strMemory.toStdString();

    int memFreepos = strMemory.indexOf("MemFree: ");
    QString memstr;
    while (stdstrMemory[memFreepos + 10] != 'k'){
        if (stdstrMemory[memFreepos + 10] > 47 && stdstrMemory[memFreepos + 10] < 58)
            memstr.append(stdstrMemory[memFreepos + 10]);
        memFreepos++;
    }

    int swpFreepos = strMemory.indexOf("SwapFree: ");
    QString swpstr;
    while (stdstrMemory[swpFreepos + 11] != 'k') {
        if (stdstrMemory[swpFreepos + 11] > 47 && stdstrMemory[swpFreepos + 11] < 58)
            swpstr.append(stdstrMemory[swpFreepos + 11]);
        swpFreepos++;
    }
    ui->progressBarMem->setValue(MemoryTotal - memstr.toInt());
    ui->progressBarSwp->setValue(SwapTotal - swpstr.toInt());
}
