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
    QString filenameNew = QFileInfo(filename).absolutePath() + "/" + QFileInfo(filename).baseName() + "_" + clipname + "." + QFileInfo(filename).completeSuffix();
    QString cmd = "ffmpeg -i \"" + filename + "\" -y -ss " + ui->timeEditStart->time().toString("hh:mm:ss") + " -to " + ui->timeEditEnd->time().toString("hh:mm:ss") + " -acodec copy -vcodec copy \"" + filenameNew + "\"";
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

void Clip::timeChange()
{
    int start = ui->timeEditStart->time().msecsSinceStartOfDay();
    int end = ui->timeEditEnd->time().msecsSinceStartOfDay();
    QTime t(0,0,0);
    t = t.addMSecs(end-start);
    ui->timeEditDuration->setTime(t);
    emit areaChange(start, end);
}
