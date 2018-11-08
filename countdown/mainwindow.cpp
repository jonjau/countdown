#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    timer(new PausableTimer{0})
{
    // setup ui first, then load settings, otherwise no widgets to set
    ui->setupUi(this);

    // *no named arguments in connect statements
    connect(timer, SIGNAL(update(int, int)), this, SLOT(on_timerUpdate(int, int)));
    connect(timer, SIGNAL(finish()), this, SLOT(on_timerFinished()));

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
    qDebug() << timer->maxSeconds() << timer->countdownTime() << timer->secondsElapsed();

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
        return;
    }

    if(timer->isActive()) {
        ui->statusBar->showMessage("--RESTARTED--");
        timer->stop();
    }
    timer->setMaxSeconds(totalSeconds);
    timer->start();
    ui->statusBar->showMessage("--STARTED--");
}

void MainWindow::on_stopPushButton_clicked()
{
    timer->stop();
    ui->statusBar->showMessage("--STOPPED--");
    ui->progressBar->setValue(0);
}

void MainWindow::on_pausePushButton_clicked()
{
    timer->pause();
        ui->statusBar->showMessage("--PAUSED--");
}


void MainWindow::on_timerUpdate(int currentSeconds, int maxSeconds)
{
    ui->progressBar->setMaximum(maxSeconds);
    ui->progressBar->setValue(currentSeconds);
    ui->statusBar->showMessage(QString(secondsInTimeString(maxSeconds - currentSeconds)) + " remaining");
}

void MainWindow::on_timerFinished()
{
    timer->stop();
    ui->progressBar->setValue(ui->progressBar->maximum());

    if (ui->fileLineEdit->text() == "") {
        ui->statusBar->showMessage("--DONE-- : No file path to be opened.");
        return;
    }

    QString os = osName();
    qDebug() << os;

    QString command( "powershell.exe" );
    QString filepath = QString("\"%1\"").arg(ui->fileLineEdit->text());

    ui->statusBar->showMessage("--DONE-- : Opening " + filepath + " ...");
    QStringList params = QStringList() << "Start-Process" << "-FilePath" << filepath;

    //qDebug() << command << params;

    //bool exists = QFileInfo(filepath).isDir() || QFileInfo(filepath).isFile();
    //bool exists = QFileInfo(QDir::toNativeSeparators(filepath)).isDir();
    //bool exists = QFileInfo::exists(filepath);
    auto exists = QFileInfo(filepath);

    qDebug() << "exists: " << exists;
    bool success = QProcess::startDetached(command, params);
    qDebug() << "succ: " << success;

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

QString MainWindow::osName()
{
    #if defined(Q_OS_WIN)
    return QLatin1String("windows");
    #elif defined(Q_OS_MACOS)
    return QLatin1String("macos");
    #elif defined(Q_OS_LINUX)
    return QLatin1String("linux");
    #elif defined(Q_OS_UNIX)
    return QLatin1String("unix");
    #else
    return QLatin1String("unknown");
    #endif
}
