#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThreadPool>
#include <QQueue>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <functional>
#include <QMutex>

namespace Ui {
class MainWindow;
}

class Cmd {

public:
    QString GetSender() {return (type? "A": "B");}
    bool GetResponse()
    {
        return (value / 2);
    }
public:
    int type;
    int value;
    QString name;
};

typedef QString VTP;
typedef QQueue<Cmd*> CmdQueue;
class CmdQueueManage;

class CmdQueueNode {
friend class CmdQueueManage;
public:
    CmdQueueNode()
    {
        pool.setMaxThreadCount(1);
        pool.setExpiryTimeout(-1);
    }
    ~CmdQueueNode()
    {
        pool.clear();
    }
public:
    template <typename Function1, typename Function2>
    void run(Cmd *cmd, Function1 const &Execute, Function2 const &Result)
    {
        QtConcurrent::run(&pool, [&]() -> void {
          Execute();
          cmd;
          cmd->GetSender();
          if (cmd->GetResponse())
            Result();
          manage->dequeue(cmd);
        });
    }

    VTP getVTP() {return vtpId;}

private:
    VTP vtpId;
    CmdQueue queue;
    QThreadPool pool;
//    QFutureWatcher<void> watcher;
    CmdQueueManage *manage;
};

class CmdQueueManage {
    friend class CmdQueueNode;
public:
    CmdQueueManage()
    {
        cmdq[0].manage = this;
        cmdq[1].manage = this;
    }
    template <typename Function1, typename Function2>
    void asyncRun(Cmd *cmd, Function1 const &Execute, Function2 const &Result)
    {
        if (isDepdend(cmd) == true) {
           QtConcurrent::run(Execute);
           return;
        }
        enqueue(cmd);
        cmdq[cmd->type].run(cmd, Execute, Result);
    }
private:
    void enqueue(Cmd *cmd)
    {
        lock.lock();
        totalq.enqueue(cmd);
        cmdq[cmd->type].queue.enqueue(cmd);
        lock.unlock();
    }
    void dequeue(Cmd *cmd)
    {
        lock.lock();
        if (!totalq.isEmpty()) {
            qDebug() << "totalq.size(): <<<" << totalq.size();
            totalq.removeOne(cmd);
            qDebug() << "totalq.size(): >>>" << totalq.size();
        }
        if (!cmdq[cmd->type].queue.isEmpty()) {
            qDebug() << "cmdq[cmd->type].queue.size(): <<<" << cmdq[cmd->type].queue.size();
            cmdq[cmd->type].queue.removeOne(cmd);
            qDebug() << "cmdq[cmd->type].queue.size(): >>>" << cmdq[cmd->type].queue.size();
        }
        lock.unlock();
    }
    bool isDepdend(Cmd *cmd)
    {
        qDebug() << "is depend: " << cmd->value;
        return false;
    }
private:
    CmdQueueNode cmdq[2];
    CmdQueue totalq;
    QMutex   lock;
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

    int i;
    CmdQueueManage cmdQueueManage;
};

#endif // MAINWINDOW_H
