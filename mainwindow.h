#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThreadPool>
#include <QQueue>
#include <QtConcurrent>
#include <QFutureWatcher>

namespace Ui {
class MainWindow;
}

class Cmd {
public:
    int type;
    int value;
    QString name;
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
    void ExecuteCmd(Cmd &cmd);
    void handleTimer();

private:
    Ui::MainWindow *ui;
    QThreadPool pool;
    int i;
    QQueue<Cmd> queue[2];
    QFutureWatcher<void> watcher;
};

#endif // MAINWINDOW_H
