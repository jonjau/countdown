#ifndef PAUSABLETIMER_H
#define PAUSABLETIMER_H


#include <QTimer>

#include <QDebug>

class PausableTimer : public QObject
{
    Q_OBJECT
public:
    explicit PausableTimer(int maxSeconds, QObject *parent = nullptr);

    void start();
    void pause();
    void stop();

    bool isActive();

    int countdownTime() const { return countdownTime_; }
    int secondsElapsed() const { return secondsElapsed_; }
    int maxSeconds() const { return maxSeconds_; }
    //int currentSeconds() const { return currentSeconds_; }

    void setCountdownTime(int newCountdownTime) { countdownTime_ = newCountdownTime; }
    void setSecondsElapsed(int newSecondsLeft) { secondsElapsed_ = newSecondsLeft; }
    void setMaxSeconds(int newMaxSeconds) { maxSeconds_ = newMaxSeconds; }
    //void setCurrentSeconds(int newCurrentSeconds) { currentSeconds_ = newCurrentSeconds; }

signals:
    void update(int, int);
    void finish();


public slots:
    //TODO: PRIVATE SLOTS?
    void tick();

private:
    QTimer* timer;

    void debugStatus(QString event);

    int maxSeconds_;

    int countdownTime_;
    int secondsElapsed_;

    // total seconds in most recent countdown, used to resume countdown.
    // previous progress
    int tempSecondsElapsed_;
};

#endif // PAUSABLETIMER_H
