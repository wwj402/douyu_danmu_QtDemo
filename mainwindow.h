#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTranslator>
#include <QApplication>
#include <QLabel>
#include "networkaccess.h"
#include "jsonparse.h"
#include "douyutcpsocket.h"
#include "danmakurecorder.h"

namespace Ui {
class MainWindow;
}

struct Danmaku_t
{
    QString speaker;
    QString text;
    double posX;
    double posY;
    double length;
    double step;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int getAvailDanmakuChannel(double currentSpeed);
    QString logfilename;
    QTimer *roomStatusTimer;
private slots:
    void htmlContent(const QString& html);
    void showChatMessage(QMap<QString, QString>);
    void showChatMessageString(QString message);
    void start();
    void stop();

    void on_actionEnglish_triggered();
    void on_actionChimese_Simplifed_triggered();

    void roomStatus(QString& roomid);

private:
    void changeEvent(QEvent *event);
    Ui::MainWindow *ui;
    NetworkAccess *network_access;
    DouyuTcpSocket *tcpSocket;
    DanmakuRecorder* danmakuRecorder = nullptr;
    int danmakuTimeNodeSeq[24] = {0};
    double danmakuWidthSeq[24]= {0};
    double danmakuSpeedSeq[24] = {0};
    QTime assTime;
    QLabel *roomStatusBar;
    QFont font;
    QTranslator *douyuTranslator;
    QTranslator *qt_translator;
    QTranslator *qtw_translator;
    QString roomidStatus;

};

#endif // MAINWINDOW_H
