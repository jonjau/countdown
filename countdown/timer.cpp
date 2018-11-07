#include "timer.h"

Timer::Timer(int countdownTime) :
    QTimer(),
    countdownTime_(countdownTime),
    secondsLeft_(countdownTime)
{
    connect(this, SIGNAL(timeout()), this, SLOT(tick()));
}

void Timer::tick()
{
    //qDebug() << secondsLeft() << "seconds left.";
    secondsLeft_  -= 1;
    if(secondsLeft() <= 0) {
        emit finish();
        this->stop();
        return;
    }
    else {
        emit update();
    }
}
