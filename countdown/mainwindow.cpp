#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    timer(new Timer{0}),
    tempTotalSeconds(0),
    tempSecondsLeft(0)
{
    // setup ui first, then load settings, otherwise no widgets to set
    ui->setupUi(this);

    settingsFile = QApplication::applicationDirPath() + "/more/settings.ini";
    loadSettings();
    qDebug() << "settings:" << settingsFile;
}

MainWindow::~MainWindow()
{
    // save current settings before deleting the ui
    saveSettings();
    delete ui;
}

void MainWindow::loadSettings()
{
    // QSettings::IniFormat is file-based, unlike NativeFormat
    QSettings settings(settingsFile, QSettings::IniFormat);
    QString filepath = settings.value("filepath","").toString();
    qDebug() << filepath;
    ui->fileLineEdit->setText(filepath);
}

void MainWindow::saveSettings()
{
    QSettings settings(settingsFile, QSettings::IniFormat);
    QString filepath = ui->fileLineEdit->text();
    settings.setValue("filepath", filepath);
}

void MainWindow::on_browsePushButton_clicked()
{
    qDebug() << tempSecondsLeft;

    QUrl url = QFileDialog::getOpenFileUrl(this);
    qDebug() << url;
    ui->fileLineEdit->setText(url.toString());
}

void MainWindow::on_startPushButton_clicked()
{
    int hours = ui->hoursSpinBox->value();
    int minutes = ui->minutesSpinBox->value();
    int seconds = ui->secondsSpinBox->value();

    int totalSeconds = timeInSeconds(hours, minutes, seconds);

    // time spinboxes must be non-zero, no negative input for this widget
    if (totalSeconds == 0) {
        ui->progressBar->setValue(0);
        ui->statusBar->showMessage("Input non-zero time.");
    }
    else {

        // stop previous timer if any
        if (timer->isActive()) {

            qDebug() << "restarting";
            ui->statusBar->showMessage("restarting.");
            timer->stop();
        }

        ui->progressBar->setMaximum(totalSeconds);
        // load previous countdown if temp variables are non-zero
        if (tempSecondsLeft != 0 && tempTotalSeconds != 0) {
            qDebug() << "continue timer " << tempSecondsLeft << " of "<< tempTotalSeconds;

            ui->progressBar->setMaximum(tempTotalSeconds);
            ui->progressBar->setValue(tempTotalSeconds - tempSecondsLeft);
            totalSeconds = tempSecondsLeft;

            timer = new Timer{totalSeconds};
            //timer->setCountdownTime();
            //timer->setSecondsLeft(tempSecondsLeft);
        }
        // save new countdown time for resuming later
        else {
            qDebug() << "new timer " << totalSeconds;

            ui->progressBar->setMaximum(totalSeconds);
            tempTotalSeconds = totalSeconds;

            timer = new Timer{totalSeconds};
        }

        qDebug() << "starting timer " << totalSeconds;




        connect(timer, SIGNAL(timeout()), this, SLOT(on_timerUpdate()));
        connect(timer, SIGNAL(finish()), this, SLOT(on_timerFinished()));
        // ticks every 1000ms = 1s
        timer->start(1000);

    }
}

void MainWindow::on_stopPushButton_clicked()
{
    timer->stop();
    tempSecondsLeft = 0;
    tempTotalSeconds = 0;

    ui->hoursSpinBox->setValue(0);
    ui->minutesSpinBox->setValue(0);
    ui->secondsSpinBox->setValue(0);

    ui->progressBar->setValue(0);
}

void MainWindow::on_pausePushButton_clicked()
{
    if (timer->isActive()) {
        timer->stop();
        tempSecondsLeft = timer->secondsLeft();
        tempTotalSeconds = timer->countdownTime();

        qDebug() << "paused" << tempSecondsLeft << " of " << tempTotalSeconds;
        ui->statusBar->showMessage("--PAUSE-- : Timer paused.");
    }
    else {
        ui->statusBar->showMessage("--ERROR-- : Timer is not active.");
    }
}


void MainWindow::on_timerUpdate()
{
    if(timer->isActive()) {
        ui->progressBar->setValue(timer->countdownTime() - timer->secondsLeft());
        ui->statusBar->showMessage(QString(secondsInTimeString(timer->secondsLeft())) + " remaining");
    }
    else {
        ui->progressBar->setValue(timer->countdownTime());
    }
}

void MainWindow::on_timerFinished()
{
    tempSecondsLeft = 0;
    tempTotalSeconds = 0;

    if (ui->fileLineEdit->text() == "") {
        ui->statusBar->showMessage("--DONE-- : No file path to be opened.");
    }
    else {
        QString command( "powershell.exe" );
        QString filepath = QString("\"%1\"").arg(ui->fileLineEdit->text());

        ui->statusBar->showMessage("--DONE-- : Opening " + filepath + " ...");
        QStringList params = QStringList() << "Start-Process" << "-FilePath" << filepath;

        //qDebug() << command << params;
        QProcess::startDetached(command, params);
    }
}

int MainWindow::timeInSeconds(int hours, int minutes, int seconds)
{
    return hours*3600 + minutes*60 + seconds;
}

QString MainWindow::secondsInTimeString(int totalSeconds)
{
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;

    QString time_str;
    if (hours == 0 && minutes == 0) {
        time_str = QString("%1 s").arg(seconds);
    }
    else if (hours == 0) {
        time_str = QString("%1 m %2 s").arg(minutes).arg(seconds);
    }
    else {
        time_str = QString("%1 h %2 m %3 s").arg(hours).arg(minutes).arg(seconds);
    }
    return time_str;
}

