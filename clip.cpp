#include "clip.h"
#include "ui_clip.h"
#include <QDebug>
#include <QFileInfo>

Clip::Clip(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Clip)
{
    ui->setupUi(this);
    connect(ui->timeEditStart, SIGNAL(timeChanged(QTime)), this, SLOT(timeChange()));
    connect(ui->timeEditEnd, SIGNAL(timeChanged(QTime)), this, SLOT(timeChange()));
}

Clip::~Clip()
{
    delete ui;
}

void Clip::on_pushButtonCut_clicked()
{
    QString cmd;
    QString filepath_clip = QFileInfo(filepath).absolutePath() + "/" + QFileInfo(filepath).baseName() + "_" + clipname + "." + QFileInfo(filepath).suffix();
    cmd = "ffmpeg -i \"" + filepath + "\" -y -ss " + ui->timeEditStart->time().toString("hh:mm:ss") + " -to " + ui->timeEditEnd->time().toString("hh:mm:ss") + " -acodec copy -vcodec copy \"" + filepath_clip + "\"";
    if(ui->checkBox_intra->isChecked()){
        QString filename_intra = QFileInfo(filepath).baseName() + "_intra." + QFileInfo(filepath).suffix();
        QString filepath_intra = QFileInfo(filepath).absolutePath() + "/" + filename_intra;
        if(!QFileInfo(filepath_intra).exists()){
            ui->pushButtonCut->setEnabled(false);
            ui->progressBar->setMaximum(0);
            cmd = "ffmpeg -i \"" + filepath + "\" -qscale 0 -intra " + filepath_intra; //由帧间编码转为帧内编码，解决剪辑的视频某时间段黑屏问题，体积会增大。
            QProcess *process = new QProcess;
            process->setObjectName("intra");
            qDebug() << cmd;
            process->start(cmd);
            connect(process, SIGNAL(finished(int)), this, SLOT(processFinish(int)));
            connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(processOutput()));
            process->waitForFinished();
        }
        cmd = "ffmpeg -i \"" + filepath_intra + "\" -y -ss " + ui->timeEditStart->time().toString("hh:mm:ss") + " -to " + ui->timeEditEnd->time().toString("hh:mm:ss") + " -acodec copy -vcodec copy \"" + filepath_clip + "\"";
    }
    QProcess *process = new QProcess;
    process->setObjectName("cut");
    qDebug() << cmd;
    process->start(cmd);
    connect(process, SIGNAL(finished(int)), this, SLOT(processFinish(int)));
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(processOutput()));
    ui->progressBar->setMaximum(0);
}

void Clip::processFinish(int i)
{
    qDebug() << "finish" << i;
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(100);
    if (sender()->objectName() == "intra") ui->pushButtonCut->setEnabled(true);
    if (i==1) ui->progressBar->setFormat("剪辑失败");
}

void Clip::processOutput()
{
    qDebug() << process->readAllStandardOutput();
    ui->progressBar->setToolTip(QString(process->readAllStandardOutput()));
}

void Clip::timeChange()
{
    qint64 start = ui->timeEditStart->time().msecsSinceStartOfDay();
    qint64 end = ui->timeEditEnd->time().msecsSinceStartOfDay();
    QTime t(0,0,0);
    t = t.addMSecs(end-start);
    ui->timeEditDuration->setTime(t);
    emit areaChange(start, end);
}