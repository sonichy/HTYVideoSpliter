#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QListWidgetItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QMediaPlayer *player;
    QVideoWidget *video;
    QString filepath, intraPath;
    qint64 pStart, pEnd;

private slots:
    void on_action_open_triggered();
    void on_action_about_triggered();
    void on_pushButtonPlay_clicked();
    void on_pushButtonStart_clicked();
    void on_pushButtonEnd_clicked();
    void on_pushButtonAdd_clicked();
    void on_pushButtonDelete_clicked();
    void on_pushButtonDirectory_clicked();
    void durationChange(qint64);
    void positionChange(qint64);
    void setSTime(int v);
    void setMPPosition();
    void stateChange(QMediaPlayer::State state);
    void playClip(QListWidgetItem* item);
    void seekBackward();
    void seekForward();
    void areaChange(qint64 start, qint64 end);

public slots:


};

#endif // MAINWINDOW_H
