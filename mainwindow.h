#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThreadPool>
#include <QQueue>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <functional>

namespace Ui {
class MainWindow;
}

class Cmd {
public:
    int type;
    int value;
    QString name;
};

typedef QString VTP;

template<typename CMD>
class CmdQueueNode {
//#define VTP QString
public:
    CmdQueueNode()
    {
        pool.setMaxThreadCount(1);
        pool.setExpiryTimeout(-1);
        qDebug() << "__ali__ cmd queue node";
    }
    ~CmdQueueNode()
    {
        pool.clear();
    }
public:
    template <typename Function>
    void run(Function const &fn)
    {
        QFuture<void> future = QtConcurrent::run(&pool, fn);
    }

    VTP getVTP() {return vtpId;}

private:
    VTP vtpId;
    QQueue<CMD> queue;
    QThreadPool pool;
    QFutureWatcher<void> watcher;
};

template<typename CMD>
class CmdQueueManage {
public:
    CmdQueueNode<CMD> cmdQueue[2];
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    void test_concurrent(int *i);

private slots:
    void on_pushButton_clicked();
    void handleFinished();
    void ExecuteCmd(Cmd *cmd);
    void handleTimer();

private:
    Ui::MainWindow *ui;

    CmdQueueManage<Cmd> cmdQueueManage;
};

#endif // MAINWINDOW_H
