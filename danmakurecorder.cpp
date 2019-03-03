#include "danmakurecorder.h"
#include <QDebug>
#include <utility>

DanmakuRecorder::DanmakuRecorder(int width, int height, QString fileName)
{
    videoHeight = height;
    videoWidth = width;
    this->fileName = std::move(fileName);
//    startTime.start();
    initDRecorder();
}

void DanmakuRecorder::initDRecorder()
{
    QFile file1(fileName);
    if (!file1.exists())
    {
        QFile file(":/ass/ass_template");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QTextStream in(&file);
        in.setCodec("UTF-8");
        assHeaderTemplate = in.readAll();
        file.close();

        assHeaderTemplate.replace(QRegExp("<qlp_width>"), QString().number(videoWidth));
        assHeaderTemplate.replace(QRegExp("<qlp_height>"), QString().number(videoHeight));

        QFile file1(fileName);
        if (!file1.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
            return;
        QTextStream out(&file1);
        out.setCodec("UTF-8");
        out << assHeaderTemplate;
        out.flush();
        file1.close();
    }


    assDanmakuTemplate = "Dialogue: 0,<qlp_startTime>,<qlp_endTime>,Default,<qlp_speaker>,0,0,0,,{\\move(<qlp_x1>,<qlp_y1>,<qlp_x2>,<qlp_y2>)}<qlp_text>";
}

void DanmakuRecorder::addASS(const QString& assSpeaker, const QString& assText, int durationMs, int x1, int y1, int x2, int y2)
{
//    qDebug() << "DanmakuRecorder::addASS";
    QString tempTime;
    QTime time(0,0);
    QString temp(assDanmakuTemplate);

    time = time.addMSecs(startTime.elapsed() - pausedTimeMs + baseTimeMs);
    tempTime = time.toString("h:mm:ss.zzz");
    tempTime.chop(1);
    temp.replace(QRegExp("<qlp_startTime>"), tempTime);
    time = time.addMSecs(durationMs);
    tempTime = time.toString("h:mm:ss.zzz");
    tempTime.chop(1);
    temp.replace(QRegExp("<qlp_endTime>"), tempTime);

    temp.replace(QRegExp("<qlp_speaker>"), assSpeaker);
    temp.replace(QRegExp("<qlp_x1>"), QString().number(x1));
    temp.replace(QRegExp("<qlp_y1>"), QString().number(y1));
    temp.replace(QRegExp("<qlp_x2>"), QString().number(x2));
    temp.replace(QRegExp("<qlp_y2>"), QString().number(y2));
    temp.replace(QRegExp("<qlp_text>"), assText);

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return;
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << temp + "\n";
    out.flush();
    file.close();

}

void DanmakuRecorder::danmaku2ASS(QString assSpeaker, QString assText, int duration, double length, int channelNum, int channel)
{
//    qDebug() << "DanmakuRecorder::danmaku2ASS";
    int x1, y1, x2, y2;
    y1 = channel * (videoHeight/channelNum);
//    qDebug() << y1;
    y2 = y1;
    x1 = videoWidth;
    x2 = -length;
    addASS(std::move(assSpeaker), std::move(assText), duration, x1, y1, x2, y2);
}

void DanmakuRecorder::pause()
{
//    qDebug() << "DanmakuRecorder::pause";
    if(!paused)
    {
        pausedStartTimeMs = startTime.elapsed();
        paused = true;
    }
}

void DanmakuRecorder::resume()
{
//    qDebug() << "DanmakuRecorder::resume";
    if(paused)
    {
        pausedTimeMs += startTime.elapsed() - pausedStartTimeMs;
        paused = false;
    }
}

void DanmakuRecorder::start()
{
    if (!started) {
        startTime.start();
        started = true;
    }
}
