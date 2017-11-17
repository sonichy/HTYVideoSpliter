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
    filename = "";
    ui->setupUi(this);
    move((QApplication::desktop()->width() - width())/2, (QApplication::desktop()->height() - height())/2);    
    ui->pushButtonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));    
    ui->pushButtonStart->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    ui->pushButtonEnd->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui->pushButtonDelete->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
    ui->pushButtonDirectory->setIcon(style()->standardIcon(QStyle::SP_DirIcon));

    video = new QVideoWidget;
    video->setStyleSheet("background:black;");
    ui->layoutVideo->addWidget(video);
    video->setMouseTracking(true);
    video->show();
    player = new QMediaPlayer;
    player->setVolume(100);
    player->setVideoOutput(video);
    connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(durationChange(qint64)));
    connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(positionChange(qint64)));
    connect(player,SIGNAL(stateChanged(QMediaPlayer::State)),SLOT(stateChange(QMediaPlayer::State)));
    connect(ui->sliderProgress,SIGNAL(valueChanged(int)),this,SLOT(setSTime(int)));
    connect(ui->sliderProgress,SIGNAL(sliderReleased()),this,SLOT(setMPPosition()));
    connect(new QShortcut(QKeySequence(Qt::Key_Space),this), SIGNAL(activated()),this, SLOT(on_pushButtonPlay_clicked()));
    connect(ui->listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(playClip(QListWidgetItem*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_open_triggered()
{
    if(filename==""){
        filename = QFileDialog::getOpenFileName(this, "打开媒体文件", ".");
    }else{
        filename = QFileDialog::getOpenFileName(this, "打开媒体文件", filename);
    }
    if(!filename.isEmpty()){        
        player->setMedia(QUrl::fromLocalFile(filename));
        ui->statusBar->showMessage("打开 " + filename);
        ui->sliderProgress->setStyleSheet("");
        ui->listWidget->clear();
    }
}

void MainWindow::on_action_about_triggered()
{
    QMessageBox aboutMB(QMessageBox::NoIcon, "关于", "海天鹰视频分割器 1.0\n一款基于 Qt 和 FFmpeg 的视频分割器。\n作者：黄颖\nE-mail: sonichy@163.com\n主页：sonichy.96.lt");
    aboutMB.setIconPixmap(QPixmap(":/icon.jpg").scaled(100,100));
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
    float p1 = (float)pStart/ui->sliderProgress->maximum();
    float p2 = (float)pEnd/ui->sliderProgress->maximum();
    ui->sliderProgress->setStyleSheet(QString("background-color: qlineargradient( spread:pad, x1:0, y1:0, x2:1, y2:0,"
                                              "stop:0 transparent, stop:%1 transparent,"
                                              "stop:%2 #0000FF, stop:%3 #0000FF,"
                                              "stop:%4 transparent, stop:1 transparent );")
                                      .arg(p1-0.001)
                                      .arg(p1)
                                      .arg(p2-0.001)
                                      .arg(p2));
}

void MainWindow::on_pushButtonEnd_clicked()
{
    pEnd = ui->sliderProgress->value();
    float p1 = (float)pStart/ui->sliderProgress->maximum();
    float p2 = (float)pEnd/ui->sliderProgress->maximum();
    ui->sliderProgress->setStyleSheet(QString("background-color: qlineargradient( spread:pad, x1:0, y1:0, x2:1, y2:0,"
                                              "stop:0 transparent, stop:%1 transparent,"
                                              "stop:%2 #0000FF, stop:%3 #0000FF,"
                                              "stop:%4 transparent, stop:1 transparent );")
                                      .arg(p1-0.001)
                                      .arg(p1)
                                      .arg(p2-0.001)
                                      .arg(p2));
}

void MainWindow::on_pushButtonAdd_clicked()
{
    Clip *clip = new Clip;
    clip->filename = filename;
    QTime t(0,0,0);
    t = t.addMSecs(pStart);
    clip->ui->timeEditStart->setTime(t);
    t.setHMS(0,0,0);
    t = t.addMSecs(pEnd);
    clip->ui->timeEditEnd->setTime(t);
    QListWidgetItem *LWI = new QListWidgetItem(ui->listWidget);
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
    if(filename != ""){
        QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(filename).absolutePath()));
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
    if(p > pEnd) player->pause();
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
        ui->pushButtonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    }else if(state == QMediaPlayer::PausedState){
        ui->pushButtonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }else if(state == QMediaPlayer::StoppedState){
        ui->pushButtonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
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

