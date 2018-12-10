#include "clip.h"
#include "ui_clip.h"
#include <QDebug>
#include <QFileInfo>

Clip::Clip(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Clip)
{
    ui->setupUi(this);
    connect(ui->timeEditStart,SIGNAL(timeChanged(QTime)),this,SLOT(timeChange()));
    connect(ui->timeEditEnd,SIGNAL(timeChanged(QTime)),this,SLOT(timeChange()));
}

Clip::~Clip()
{
    delete ui;
}

void Clip::on_pushButtonCut_clicked()
{
    QString filepathNew = QFileInfo(filepath).absolutePath() + "/" + QFileInfo(filepath).baseName() + "_" + clipname + "." + QFileInfo(filepath).completeSuffix();
    QString cmd = "ffmpeg -i \"" + filepath + "\" -y -ss " + ui->timeEditStart->time().toString("hh:mm:ss") + " -to " + ui->timeEditEnd->time().toString("hh:mm:ss") + " -acodec copy -vcodec copy \"" + filepathNew + "\"";
    qDebug() << cmd;
    process = new QProcess;
    process->start(cmd);
    connect(process,SIGNAL(finished(int)),this,SLOT(processFinish(int)));
    connect(process,SIGNAL(readyReadStandardOutput()),this,SLOT(processOutput()));
    ui->progressBar->setMaximum(0);
//    intraPath = QFileInfo(filepath).absolutePath() + "/" + QFileInfo(filepath).baseName() + "_intra." + QFileInfo(filepath).suffix();
//    QString cmd = "ffmpeg -i " + filepath + " -qscale 0 -intra " + intraPath ; //由帧间编码转为帧内编码，解决剪辑的视频某时间段黑屏问题，体积会增大。
//    qDebug() << cmd;
//    QProcess *process = new QProcess;
//    process->start(cmd);
}

void Clip::processFinish(int i)
{
    qDebug() << "finish" << i;
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(100);
    if(i==1)ui->progressBar->setFormat("剪辑失败");
}

void Clip::processOutput()
{
    qDebug() << process->readAllStandardOutput();
    ui->progressBar->setToolTip(QString(process->readAllStandardOutput()));
}

void Clip::timeChange()
{
    int start = ui->timeEditStart->time().msecsSinceStartOfDay();
    int end = ui->timeEditEnd->time().msecsSinceStartOfDay();
    QTime t(0,0,0);
    t = t.addMSecs(end-start);
    ui->timeEditDuration->setTime(t);
    emit areaChange(start, end);
}
