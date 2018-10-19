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
        return (value % 2);
    }
public:
    int type;
    int value;
    QString name;
};

struct CmdStatus {
    CmdStatus(Cmd *_cmd):cmd(_cmd),isFinish(false),isDepend(false)
    {
    }
    Cmd *cmd;
    bool isFinish;
    bool isDepend;
};

typedef QString VTP;
typedef QQueue<CmdStatus*> CmdQueue;
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
    void run(CmdStatus *cmdStatus, Function1 const &Execute, Function2 const &Result)
    {
        QtConcurrent::run(&pool, [&]() -> void {
          Execute();

          manage->lock();
          cmdStatus->isFinish = true;

          for (int i = 0; i < manage->totalq.size(); i++) {
              CmdStatus *_cmdStatus = manage->totalq.at(i);
              if (_cmdStatus->isFinish == true) {
                  if (_cmdStatus->cmd->GetResponse())
                      Result();
                   manage->dequeue(_cmdStatus);
              } else {
                    break;
              }
          }

          manage->unlock();
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
        CmdStatus *status = new CmdStatus(cmd);
        if (isDepdend(status) == true) {
           QtConcurrent::run(Execute);
           delete status;
           return;
        }        
        lock();
        enqueue(status);
        unlock();
        cmdq[cmd->type].run(status, Execute, Result);
    }
private:
    void enqueue(CmdStatus *status)
    {
        totalq.enqueue(status);
        cmdq[status->cmd->type].queue.enqueue(status);
    }
    void dequeue(CmdStatus *cmdStatus)
    {
        totalq.removeOne(cmdStatus);
        cmdq[cmdStatus->cmd->type].queue.removeOne(cmdStatus);
        delete cmdStatus;
//        for (int i = 0; i < totalq.size(); i++) {
//            CmdStatus *status = totalq.at(i);
//            if (status->cmd == cmd) {
//                totalq.removeOne(status);
//                delete status;
//                return;
//            }
//        }
//        for (int i = 0; i < cmdq[cmd->type].queue.size(); i++) {
//            CmdStatus *status = totalq.at(i);
//            if (status->cmd == cmd) {
//                cmdq[cmd->type].queue.removeOne(status);
//                delete status;
//                return;
//            }
//        }
    }
    bool isDepdend(CmdStatus *cmdStatus)
    {
        qDebug() << "is depend: " << cmdStatus->isDepend;
        return false;
    }

    inline CmdStatus *totalq_head()
    {
        return totalq.head();
    }
    inline void lock()
    {
        _lock.lock();
    }
    inline void unlock()
    {
        _lock.unlock();
    }
private:
    CmdQueueNode cmdq[2];
    CmdQueue totalq;
    QMutex   _lock;
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
