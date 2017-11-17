#include "clip.h"
#include "ui_clip.h"
#include <QDebug>
#include <QFileInfo>

Clip::Clip(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Clip)
{
    ui->setupUi(this);    
}

Clip::~Clip()
{
    delete ui;
}

void Clip::on_pushButtonCut_clicked()
{
    // ffmpeg -i E:\B\1.flv -y -ss 00:30:00 -t 01:00:00 -acodec copy -vcodec copy E:\B\1_2.flv
    // -y 覆盖操作
    // -ss 起始时间
    // -t 时长
    // -to 结束时间，于-t单独用
    // -acodec copy 音频编码不变
    // -vcodec copy  视频编码不变
    QString filenameNew = QFileInfo(filename).absolutePath() + "/" + QFileInfo(filename).baseName() + "_clip." + QFileInfo(filename).completeSuffix();
    QString cmd = "ffmpeg -i " + filename + " -y -ss " + ui->timeEditStart->time().toString("hh:mm:ss") + " -to " + ui->timeEditEnd->time().toString("hh:mm:ss") + " -acodec copy -vcodec copy " + filenameNew;
    qDebug() << cmd;
    process = new QProcess;
    process->start(cmd);
    connect(process,SIGNAL(finished(int)),this,SLOT(processFinish(int)));
    connect(process,SIGNAL(readyReadStandardOutput()),this,SLOT(processOutput()));
    ui->progressBar->setMaximum(0);
}

void Clip::processFinish(int i)
{
    qDebug() << "finish" << i;
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(100);
    if(i==1)ui->progressBar->setFormat("不支持");
}

void Clip::processOutput()
{
    qDebug() << process->readAllStandardOutput();
    ui->progressBar->setToolTip(QString(process->readAllStandardOutput()));
}
