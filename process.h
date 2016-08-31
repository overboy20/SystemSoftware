#ifndef PROCESS_H
#define PROCESS_H
#include <string>
#include <QString>

class process
{
public:
    process(int pid, QString usr, int virt, int res, int shr,
            QString state, QString name, QString comm, int th);
    process(int pid);

    //getters
    int getPID();
    QString getUser();
    int getVirt();
    int getRes();
    int getShr();
    QString getState();
    QString getName();
    QString getCommand();
    int getThreads();

    //setters
    void setPID(int pid);
    void setUser(QString user);
    void setVirt(int virt);
    void setRes(int res);
    void setShr(int shr);
    void setState(QString state);
    void setName(QString n);
    void setCommand(QString c);
    void setThreads(int n);

private:
    int PID, threads, Virt, Res, Shr;
    QString USer, State, Name, Command;

    //методи для формування параметрів
    void FindNameStateThreads();
    //визначає ім’я користувача по його Uid
    QString FindUser();
    //визначає команду
    QString FindCommand();
    //визначає три параметри використання пам’яті
    void findMemory();

    //допоміжні методи
    QString ReadStatusFile();
};
#endif // PROCESS_H
