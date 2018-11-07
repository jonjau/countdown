#ifndef TIMER_H
#define TIMER_H

#include <QTimer>

#include <QDebug>

class Timer : public QTimer
{
    Q_OBJECT
public:
    explicit Timer(int countdownTime);

    int countdownTime() const { return countdownTime_; }
    int secondsLeft() const { return secondsLeft_; }

    void setCountdownTime(int newCountdownTime) { countdownTime_ = newCountdownTime; }
    void setSecondsLeft(int newSecondsLeft) { secondsLeft_ = newSecondsLeft; }

signals:
    void update();
    void finish();

public slots:
    void tick();

private:

    int countdownTime_;
    int secondsLeft_;
};

#endif // TIMER_H
