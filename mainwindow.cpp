#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThreadPool>
#include <QtConcurrent>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    i(0)
{
    ui->setupUi(this);
    pool.setMaxThreadCount(1);
    pool.setExpiryTimeout(-1);
//    queue[0].enqueue(1);
//    queue[0].enqueue(2);
//    queue[0].enqueue(3);
//    qDebug() << queue[0].at(0);
//    qDebug() << queue[0].at(1);
//    qDebug() << queue[0].at(2);
//    queue[0].enqueue(1);
//    qDebug() << queue[0].at(0);
//    qDebug() << "dequeue " << queue[0].dequeue();
//        qDebug() << queue[0].at(0);
//    connect(&watcher, SIGNAL(finished())
//            , this, SLOT(handleFinished()));

    QTimer *tr = new QTimer(this);
    connect(tr, SIGNAL(timeout()), this, SLOT(handleTimer()));
    tr->start(5000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{

    QFuture<void> future = QtConcurrent::run(&pool, [&]() {
        // Code in this block will run in another thread
                    qDebug() << "__ali__ start";

        qDebug() << "__ali__ end";

    });

    watcher.setFuture(future);
}


void MainWindow::test_concurrent(int *i)
{
       int j = *i;
       qDebug() << "__ali__ concurrent running start <<" << j;
       QThread::sleep(10);
       qDebug() << "__ali__ concurrent running end >> " << j;
}

void MainWindow::handleFinished()
{
    qDebug() << "finished";
}


void MainWindow::ExecuteCmd(Cmd &cmd)
{
//
        qDebug() << "...execute cmd...";
}

void MainWindow::handleTimer()
{
//

        Cmd *cmd = new Cmd;
        static int a;
        cmd->value = a++;
        static bool b = false;
        b = !b;
        cmd->type = (int)b;
         qDebug() << "...handle timer... " << cmd->value;

         queue[cmd->type].enqueue(*cmd);
         QFuture<void> future = QtConcurrent::run(&pool, [&]() {
             // Code in this block will run in another thread
//             qDebug() << "cmd start";
             ExecuteCmd(*cmd);
//             qDebug() << "cmd end";
//             qDebug() << "__ali__ end queue[0].size: " << queue[0].size();
//             queue[cmd->type].removeOne(*cmd);
             qDebug() << "queue[cmd->type].indexOf(*cmd): " << queue[cmd->type].indexOf(*cmd);
         });

}
