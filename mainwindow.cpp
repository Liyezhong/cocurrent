#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QSharedPointer>
#include <QSharedDataPointer>
#include <unistd.h>
#include <QEventLoop>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    i = 0;


    QTimer *tr = new QTimer(this);
    connect(tr, SIGNAL(timeout()), this, SLOT(handleTimer()));
    tr->start(1000);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{

}


void MainWindow::test_concurrent(int *i)
{

}

void MainWindow::handleFinished()
{
    qDebug() << "finished";
}


void MainWindow::ExecuteCmd(Cmd *cmd)
{
//
        qDebug() << "          ...execute cmd..." << "\ncmd->type: "<< cmd->type << "\ncmd->value:" << cmd->value;
}

void MainWindow::handleTimer()
{
        Cmd *cmd = new Cmd;
//        QSharedPointer<Cmd> cmd1(cmd);
        static int a;
        cmd->value = a++;
        static bool b = false;
        b = !b;
        cmd->type = (int)0;
        cmdQueueManage.asyncRun(
              cmd,
              [&](Cmd *_cmd) -> void {
                 // running.
                 ExecuteCmd(_cmd);
                 int sleeptime = (size_t)_cmd % 5;
                 qDebug() << "sleep time: " << sleeptime;
                 QThread::sleep(sleeptime);
//                 sleep(sleeptime);
//                 _cmd->sem.acquire();

              },
              [&](Cmd *_cmd) -> void {
                 // process result.
                 qDebug() << "push cmd queue to scheduler cmd-value: " << _cmd->value ;
              }
        );
}
