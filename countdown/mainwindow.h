#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QTimer>
#include <QProcess>
#include <QSettings>
#include <QFileDialog>


#include <QDebug>

//#include "timer.h"
#include "pausabletimer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void stop();

public slots:
    void on_timerUpdate(int, int);
    void on_timerFinished();

    void on_browsePushButton_clicked();
    void on_startPushButton_clicked();
    void on_pausePushButton_clicked();
    void on_stopPushButton_clicked();

private:
    Ui::MainWindow *ui;
    QString settingsFile;

    PausableTimer* timer;
    // total seconds in most recent countdown, used to resume countdown.
    //int tempTotalSeconds;
    // previous progress
    //int tempSecondsLeft;

    void loadSettings();
    void saveSettings();
    int timeInSeconds(int hours, int minutes, int seconds);
    QString secondsInTimeString(int seconds);
};

#endif // MAINWINDOW_H
