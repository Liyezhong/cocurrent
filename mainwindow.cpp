#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QSharedPointer>
#include <QSharedDataPointer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    i = 0;

#if 1
//    QSharedPointer<QTimer> tr = QSharedPointer<QTimer>(new QTimer(this), &QObject::deleteLater);
////    QSharedPointer<QTimer> tr = QSharedPointer<QTimer>(new QTimer(this));
//    connect(tr.data(), SIGNAL(timeout()), this, SLOT(handleTimer()));
//    tr.data()->start(5000);
    QScopedPointer<QTimer> tr(new QTimer(this));
        connect(tr.data(), SIGNAL(timeout()), this, SLOT(handleTimer()));
        tr.data()->start(5000);
        tr.take();
#else
    QTimer *tr = new QTimer(this);
    connect(tr, SIGNAL(timeout()), this, SLOT(handleTimer()));
    tr->start(5000);
    QScopedPointer<QTimer> tr2(tr);
    qDebug() << tr2.take();
#endif
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
        cmd->type = (int)b;
        qDebug() << "...handle timer... " << cmd->value;
        cmdQueueManage.asyncRun(
              cmd,
              [&]() -> void {
                 // running.
                 ExecuteCmd(cmd);
              },
              [&]() -> void {
                 // process result.
                 qDebug() << "push cmd queue to scheduler" << i << " i + 1" ;
              }
        );
}
