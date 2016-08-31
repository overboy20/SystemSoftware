#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    //повертає список імен директорій процесів (list of PIDs)
    QStringList GetProcessList();
    //оновлює дані в таблиці
    void RefreshList();
    //оновлює дані в списку об’єктів процесів
    void UpdateVector();
    //генерує модель на основі всіх даних та встановлює
    //її як модель таблиці
    void GenerateModel();
    
private slots:

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    //для оновлення інформації про пам’ять
    void TimerTick(void);

private:
    Ui::MainWindow *ui;
    QTimer timer;
};

#endif // MAINWINDOW_H
