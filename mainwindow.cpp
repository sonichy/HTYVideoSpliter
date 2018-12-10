#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clip.h"
#include "ui_clip.h"
#include <QDesktopWidget>
#include <QFileDialog>
#include <QTime>
#include <QMessageBox>
#include <QShortcut>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    filepath = ".";
    ui->setupUi(this);
    move((QApplication::desktop()->width() - width())/2, (QApplication::desktop()->height() - height())/2);

    video = new QVideoWidget;
    video->setStyleSheet("background:black;");
    ui->layoutVideo->addWidget(video);
    video->setMouseTracking(true);
    video->show();
    player = new QMediaPlayer;
    //player->setVolume(100);
    player->setVideoOutput(video);
    connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(durationChange(qint64)));
    connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(positionChange(qint64)));
    connect(player,SIGNAL(stateChanged(QMediaPlayer::State)),SLOT(stateChange(QMediaPlayer::State)));
    connect(ui->sliderProgress,SIGNAL(valueChanged(int)),this,SLOT(setSTime(int)));
    connect(ui->sliderProgress,SIGNAL(sliderReleased()),this,SLOT(setMPPosition()));
    connect(new QShortcut(QKeySequence(Qt::Key_Space),this), SIGNAL(activated()),this, SLOT(on_pushButtonPlay_clicked()));
    connect(new QShortcut(QKeySequence(Qt::Key_Left),this), SIGNAL(activated()),this, SLOT(seekBackward()));
    connect(new QShortcut(QKeySequence(Qt::Key_Right),this), SIGNAL(activated()),this, SLOT(seekForward()));
    connect(ui->listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(playClip(QListWidgetItem*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_open_triggered()
{
    //if(filepath == "") filepath = ".";
    filepath = QFileDialog::getOpenFileName(this, "打开媒体文件", filepath);
    if(!filepath.isEmpty()){
        player->setMedia(QUrl::fromLocalFile(filepath));
        ui->statusBar->showMessage("打开 " + filepath);
        ui->sliderProgress->setStyleSheet("");
        ui->listWidget->clear();
    }
}

void MainWindow::on_action_about_triggered()
{
    QMessageBox aboutMB(QMessageBox::NoIcon, "关于", "海天鹰视频分割器 1.0\n一款基于 Qt 和 FFmpeg 的视频分割器。\n作者：黄颖\nE-mail: sonichy@163.com\n主页：https://www.github.com/sonichy");
    aboutMB.setIconPixmap(QPixmap(":/icon.jpg").scaled(150,150));
    aboutMB.exec();
}

void MainWindow::on_pushButtonPlay_clicked()
{
    if(player->state() == QMediaPlayer::PlayingState){
        player->pause();
    }else{
        player->play();
    }
}

void MainWindow::on_pushButtonStart_clicked()
{
    pStart = ui->sliderProgress->value();
    areaChange(pStart, pEnd);
    if(ui->listWidget->currentRow() != -1){
        Clip *clip = (Clip*)(ui->listWidget->itemWidget(ui->listWidget->item(ui->listWidget->currentRow())));
        QTime t(0,0,0);
        t = t.addMSecs(pStart);
        clip->ui->timeEditStart->setTime(t);
    }
}

void MainWindow::on_pushButtonEnd_clicked()
{
    pEnd = ui->sliderProgress->value();
    areaChange(pStart, pEnd);
    if(ui->listWidget->currentRow() != -1){
        Clip *clip = (Clip*)(ui->listWidget->itemWidget(ui->listWidget->item(ui->listWidget->currentRow())));
        QTime t(0,0,0);
        t = t.addMSecs(pEnd);
        clip->ui->timeEditEnd->setTime(t);
    }
}

void MainWindow::areaChange(qint64 start, qint64 end)
{
    float p1 = (float)start/ui->sliderProgress->maximum();
    float p2 = (float)end/ui->sliderProgress->maximum();
    ui->sliderProgress->setStyleSheet(QString("background-color: qlineargradient( spread:pad, x1:0, y1:0, x2:1, y2:0,"
                                              "stop:0 #FFFFFF, stop:%1 #FFFFFF,"
                                              "stop:%2 #0000FF, stop:%3 #0000FF,"
                                              "stop:%4 #FFFFFF, stop:1 #FFFFFF );")
                                      .arg(p1-0.001)
                                      .arg(p1)
                                      .arg(p2-0.001)
                                      .arg(p2));
}

void MainWindow::on_pushButtonAdd_clicked()
{
    Clip *clip = new Clip;
    connect(clip,SIGNAL(areaChange(int,int)),this,SLOT(areaChange(int,int)));
    clip->filepath = filepath;
    QDateTime datetime = QDateTime::currentDateTime();
    clip->clipname = "clip" + datetime.toString("yyyyMMddhhmmss");
    QTime t(0,0,0);
    t = t.addMSecs(ui->sliderProgress->maximum());
    clip->ui->timeEditStart->setMaximumTime(t);
    clip->ui->timeEditEnd->setMaximumTime(t);
    t.setHMS(0,0,0);
    t = t.addMSecs(pStart);
    clip->ui->timeEditStart->setTime(t);
    t.setHMS(0,0,0);
    t = t.addMSecs(pEnd);
    clip->ui->timeEditEnd->setTime(t);
    t.setHMS(0,0,0);
    t = t.addMSecs(pEnd - pStart);
    clip->ui->timeEditDuration->setTime(t);
    QListWidgetItem *LWI = new QListWidgetItem(ui->listWidget);
    LWI->setSizeHint(QSize(500,32));
    ui->listWidget->addItem(LWI);
    ui->listWidget->setItemWidget(LWI,clip);
}

void MainWindow::on_pushButtonDelete_clicked()
{
    if(ui->listWidget->currentRow() != -1){
        ui->listWidget->takeItem(ui->listWidget->currentRow());
        ui->listWidget->setCurrentRow(-1);
    }
}

void MainWindow::on_pushButtonDirectory_clicked()
{
    if(filepath != ""){
        QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(filepath).absolutePath()));
    }
}

void MainWindow::durationChange(qint64 d)
{
    ui->sliderProgress->setMaximum(d);
    QTime t(0,0,0);
    t = t.addMSecs(d);
    QString sTimeTotal = t.toString("hh:mm:ss");
    ui->labelTimeTotal->setText(sTimeTotal);
    pStart = 0;
    pEnd = d;
}

void MainWindow::positionChange(qint64 p)
{
    ui->sliderProgress->setValue(p);
    //qDebug() << p << pEnd;
    //if (p > pEnd) player->pause(); // 崩溃
}

void MainWindow::setSTime(int v)
{
    QTime t(0,0,0);
    t = t.addMSecs(v);
    QString sTimeElapse = t.toString("hh:mm:ss");
    ui->labelTimeNow->setText(sTimeElapse);
}

void MainWindow::setMPPosition()
{
    player->setPosition(ui->sliderProgress->value());
}

void MainWindow::stateChange(QMediaPlayer::State state)
{
    //qDebug() << state;
    if(state == QMediaPlayer::PlayingState){
        ui->pushButtonPlay->setIcon(QIcon::fromTheme("media-playback-pause"));
        ui->pushButtonPlay->setToolTip("暂停 Space");
    }else if(state == QMediaPlayer::PausedState){
        ui->pushButtonPlay->setIcon(QIcon::fromTheme("media-playback-start"));
        ui->pushButtonPlay->setToolTip("播放 Space");
    }else if(state == QMediaPlayer::StoppedState){
        ui->pushButtonPlay->setIcon(QIcon::fromTheme("media-playback-start"));
        ui->pushButtonPlay->setToolTip("播放 Space");
    }
}

void MainWindow::playClip(QListWidgetItem* item)
{
    Clip *clip = (Clip*)(ui->listWidget->itemWidget(item));
    pStart = clip->ui->timeEditStart->time().msecsSinceStartOfDay();
    pEnd = clip->ui->timeEditEnd->time().msecsSinceStartOfDay();
    player->setPosition(pStart);
    player->play();
}

void MainWindow::seekBackward()
{
    player->setPosition(player->position() - 5000);
}

void MainWindow::seekForward()
{
    player->setPosition(player->position() + 5000);
}