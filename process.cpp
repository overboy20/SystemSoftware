#include "process.h"
#include <string>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <pwd.h>
#include <fstream>
#include <iostream>

process::process(int pid, QString usr, int virt, int res, int shr,
        QString state, QString name, QString comm, int th){
    setPID(pid);
    setUser(usr);
    setVirt(virt);
    setRes(res);
    setShr(shr);
    setState(state);
    setName(name);
    setCommand(comm);
    setThreads(th);
}

process::process(int pid){
    setPID(pid);
    setUser(FindUser());
    setState("");
    setName("");
    setThreads(1);
    FindNameStateThreads();
    setCommand(FindCommand());
    findMemory();
}

void process::FindNameStateThreads(){
    QString name, state, statusFile, threads;
    //зчитується весь статус-файл
    statusFile = ReadStatusFile();
    //позиція входження
    int npos = statusFile.indexOf("Name:");

    //перетворюється в простий масив символів
    std::string status = statusFile.toStdString();
    //поки не буде прочитано символ повертання каретки
    while(status[npos + 6]  != 10) {
        name.append(status[npos+6]);
        npos++;
    }
    setName(name);

    int statepos = statusFile.indexOf("State:");
    while(status[statepos + 7] != 10) {
        state.append(status[statepos + 7]);
        statepos++;
    }
    setState(state);

    int threadspos = statusFile.indexOf("Threads:");
    while(status[threadspos + 9] != 10) {
        threads.append(status[threadspos + 9]);
        threadspos++;
    }
    setThreads(threads.toInt());

}

QString process::ReadStatusFile() {
    QFile file("/proc/" + QString::number(getPID()) + "/status");
    if (!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "Error!", "Помилка при читанні status-файлу!");
    QTextStream in(&file);
    QString statusFile = in.readAll();
    return statusFile;
}

QString process::FindCommand() {
    char command[4096+1];
    QString str = "/proc/" + QString::number(getPID()) + "/cmdline";
    FILE* file = fopen(str.toStdString().c_str(), "r");
    int amtRead = fread(command, 1, sizeof(command) - 1, file);
    if (amtRead > 0){
        for (int i = 0; i < amtRead; i++)
            if (command[i] == '\0' || command[i] == '\n'){
                command[i] = ' ';
            }
    }
    command[amtRead] = '\0';
    fclose(file);
    QString comm;
    comm = (QString) strdup(command);
    if (comm.length() == 0 || comm.length() > 30) comm = this->getName();

    return comm;
}

QString process::FindUser(){
    QString statusFile, n;
    statusFile = ReadStatusFile();
    int pos = statusFile.indexOf("Uid:");
    std::string status = statusFile.toStdString();
    while(status[pos + 5]  != 9) {
        n.append(status[pos + 5]);
        pos++;
    }

    //використовується структура
    //pw з pwd.h
    register uid_t uid;
    register struct passwd *pw;
    uid = n.toInt();
    //функція проходиться про всьому списку користувачів
    //і шукає збіги зі зчитаним uid
    pw = getpwuid(uid);
    if (pw) return (QString) pw->pw_name;
    else    return "root";

}

void process::findMemory(){
    int size = 0, resident = 0, share = 0;
    QString str = "/proc/" + QString::number(getPID()) + "/statm";
    std::ifstream buffer(str.toStdString().c_str());
    buffer >> size >> resident >> share;
    buffer.close();
    // в файлі дані вказані в сторінках, 1 сторінка = 4 кб
    setVirt(size * 4);
    setRes(resident * 4);
    setShr(share * 4);
}

//getters
int process::getPID(){
    return PID;
}
QString process::getUser(){
    return USer;
}
int process::getVirt(){
    return Virt;
}
int process::getRes(){
    return Res;
}
int process::getShr(){
    return Shr;
}

QString process::getState(){
    return State;
}
QString process::getName(){
    return Name;
}
QString process::getCommand(){
    return Command;
}
int process::getThreads(){
    return threads;
}

//setters
void process::setPID(int pid){
    this->PID = pid;
}
void process::setUser(QString user){
    this->USer = user;
}
void process::setVirt(int virt){
    this->Virt = virt;
}
void process::setRes(int res){
    this->Res = res;
}
void process::setShr(int shr){
    this->Shr = shr;
}

void process::setState(QString state){
    this->State = state;
}
void process::setName(QString n){
    this->Name = n;
}
void process::setCommand(QString c){
    this->Command = c;
}
void process::setThreads(int n){
    threads = n;
}
