#ifndef CLIP_H
#define CLIP_H

#include <QWidget>
#include <QProcess>

namespace Ui {
class Clip;
}

class Clip : public QWidget
{
    Q_OBJECT

public:
    explicit Clip(QWidget *parent = 0);
    ~Clip();
    Ui::Clip *ui;
    QString filepath, clipname;

private:
    QProcess *process;

private slots:
    void on_pushButtonCut_clicked();
    void processFinish(int i);
    void processOutput();
    void timeChange();

signals:
    void areaChange(int start, int end);

};

#endif // CLIP_H
