#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThreadPool>
#include <QQueue>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <functional>
#include <QMutex>
#include <QSemaphore>
#include <QEventLoop>

namespace Ui {
class MainWindow;
}

class Cmd {

public:
    QString GetSender() {return (type? "A": "B");}
    bool GetResponse()
    {
        return true;
        return (value % 2);
    }
public:
    int type;
    int value;
    QString name;
    QSemaphore sem;
    QEventLoop ev;
};

struct CmdStatus {
    CmdStatus(Cmd *_cmd):cmd(_cmd),isFinish(false),isDepend(true)
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
    void run(CmdStatus *__cmdStatus, Function1 const &Execute, Function2 const &Result);

    VTP getVTP() {return vtpId;}

private:
    VTP vtpId;
    CmdQueue queue;
    QThreadPool pool;
//    QFutureWatcher<void> watcher;
    CmdQueueManage *manage;
    QMutex lock;
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
    void asyncRun(Cmd *cmd, Function1 const &Execute, Function2 const &Result);

private:
    void enqueue(CmdStatus *cmdStatus)
    {
        totalq.enqueue(cmdStatus);
        cmdq[cmdStatus->cmd->type].queue.enqueue(cmdStatus);
        qDebug() << "enqueue totalq size: " << totalq.size();
    }
    void dequeue(CmdStatus *cmdStatus)
    {
        totalq.removeOne(cmdStatus);
        cmdq[cmdStatus->cmd->type].queue.removeOne(cmdStatus);
        delete cmdStatus->cmd;
        delete cmdStatus;
        qDebug() << "dequeue totalq size: " << totalq.size();
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

template <typename Function1, typename Function2>
void CmdQueueNode::run(CmdStatus *__cmdStatus, Function1 const &Execute, Function2 const &Result)
{
    QtConcurrent::run(&pool, [&](CmdStatus *cmdStatus) -> void {
      lock.lock();
      Execute(cmdStatus->cmd);

      manage->lock();
      cmdStatus->isFinish = true;
      if (cmdStatus->cmd->GetResponse() == false) {
          manage->dequeue(cmdStatus);
      }
// total queue process
#define TOTAL_QUEUE_DEF
#ifdef TOTAL_QUEUE_DEF
      for (CmdStatus *_cmdStatus : manage->totalq) {
          if (_cmdStatus->isFinish == true) {
              Result(_cmdStatus->cmd);
              manage->dequeue(_cmdStatus);
          } else {
                break;
          }
      }
#endif

      manage->unlock();
      lock.unlock();
    }, __cmdStatus);
}

template <typename Function1, typename Function2>
void CmdQueueManage::asyncRun(Cmd *cmd, Function1 const &Execute, Function2 const &Result)
{
    CmdStatus *cmdStatus = new CmdStatus(cmd);
    if (cmdStatus->isDepend == false) {
       QtConcurrent::run(QThreadPool::globalInstance(), [&](CmdStatus *_cmdStatus) -> void {
           Execute(_cmdStatus->cmd);
           delete _cmdStatus;
       }, cmdStatus);

       return;
    }
    lock();
    enqueue(cmdStatus);
    unlock();
    cmdq[cmd->type].run(cmdStatus, Execute, Result);
}

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
