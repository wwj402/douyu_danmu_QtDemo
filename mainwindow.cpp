#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stringgenerator.h"
#include "danmuconfig.h"
#include <QRegExp>
#include <QPainter>
#include <QPixmap>
#include <QColor>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <QDateTime>



MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    douyuTranslator = new QTranslator(this);
    qDebug() << douyuTranslator->load(QLocale(), "Douyu", "_", ":/qm");
    qApp->installTranslator(douyuTranslator);
    roomStatusTimer = new QTimer(this);

//    加载qt自带语言包
    qt_translator = new QTranslator(this);
    qDebug() << qt_translator->load(QLocale(), "qt", "_", ":/qm");
//    qt_translator->load(":/qm/qt_zh_CN.qm");
    qApp->installTranslator(qt_translator);
    qtw_translator = new QTranslator(this);
    qDebug() << qtw_translator->load(QLocale(), "qtwidgets", "_", ":/qm");
    qApp->installTranslator(qtw_translator);
    roomStatusBar = new QLabel;
    ui->statusBar->addPermanentWidget(roomStatusBar);
//    ui->tabWidget->addTab(new QPlainTextEdit(this),QString(tr("Status")));
//    ui->tabWidget->addTab(new QPlainTextEdit(this),QString(tr("Danmuku")));
//    ui->tabWidget->addTab(new QPlainTextEdit(this),QString(tr("Gift")));
//    ui->tabWidget->addTab(new QPlainTextEdit(this),QString(tr("Other")));

//    QPlainTextEdit *edit = static_cast<QPlainTextEdit*>(ui->tabWidget->widget(0));
//    edit->setReadOnly(true);
//    edit = static_cast<QPlainTextEdit*>(ui->tabWidget->widget(1));
//    edit->setReadOnly(true);
//    edit = static_cast<QPlainTextEdit*>(ui->tabWidget->widget(2));
//    edit->setReadOnly(true);
//    edit = static_cast<QPlainTextEdit*>(ui->tabWidget->widget(3));
//    edit->setReadOnly(true);
//    edit = NULL;
    network_access = new NetworkAccess();
    tcpSocket = new DouyuTcpSocket(this);
    connect(ui->pushButton_start,SIGNAL(clicked(bool)),this,SLOT(start()));
    connect(ui->pushButton_stop,SIGNAL(clicked(bool)),this,SLOT(stop()));
    connect(network_access,SIGNAL(pageLoadFinished(QString)),this,SLOT(htmlContent(QString)));
//    connect(tcpSocket,SIGNAL(chatMessageString(QString)),this,SLOT(showChatMessageString(QString)));
    connect(tcpSocket,SIGNAL(chatMessage(QMap<QString,QString>)),this,SLOT(showChatMessage(QMap<QString,QString>)));
    connect(roomStatusTimer, &QTimer::timeout, this,
            [=]() {roomStatus(roomidStatus);});
}

int MainWindow::getAvailDanmakuChannel(double currentSpeed)
{
//    qDebug() << "DanmakuLauncher::getAvailDanmakuChannel";
    int resWidth = 1280;
    int currentTime = assTime.elapsed();
    int i;
    for(i = 0; i < 24; i++)
    {
        if ((((currentTime - danmakuTimeNodeSeq[i]) * 60 * currentSpeed / 1000) - danmakuWidthSeq[i]) > 0)
        {
            if ((((double)(currentTime - danmakuTimeNodeSeq[i]) * danmakuSpeedSeq[i] / 16.67) - danmakuWidthSeq[i]) / (currentSpeed - danmakuSpeedSeq[i]) > ((double)resWidth / currentSpeed))
            {
                return i;
            }
            if (currentSpeed - danmakuSpeedSeq[i] <= 0)
            {
                return i;
            }
        }
    }
//    i = qrand()%24;
    return -4;
}


MainWindow::~MainWindow()
{
    delete tcpSocket;
    delete network_access;
    delete ui;
    delete danmakuRecorder;
}

void MainWindow::start()
{
    QString roomid = (ui->lineEdit_roomid->text()).trimmed();
    roomidStatus = roomid;
    QRegExp rx("[0-9a-zA-Z]+");
    rx.setMinimal(false);
    if(rx.exactMatch(roomid))
    {
        bool ok = false;
        roomid.toInt(&ok);
        if(!ok)
        {
            QString url_str = QString(_Douyu_RoomApiServer_HostName + "%1").arg(roomid);
            QUrl url = QUrl(QString(url_str));
            network_access->loadingPage(url);
        }
        else
        {
            roomStatusTimer->stop();
            roomStatus(roomid);
            if (ui->checkBox_ass->isChecked())
            {
                QString str_dir = QCoreApplication::applicationDirPath();
                QString str_date = QDateTime::currentDateTime().toString("yyyyMMdd");
                //QMessageBox::information(this, tr("Danmu Path"), str_dir + "/" + roomid + "_" + str_date + "_log.ass");
                logfilename = str_dir + "/" + roomid + "_" + str_date + "_log.ass";
                QFile f(logfilename);
                if (!f.exists())
                {
//                    f.open(QIODevice::WriteOnly);
//                    f.close();
                    for (int i = 0; i < 24; i++)
                    {
                        danmakuWidthSeq[i] = -1000000;
                        danmakuTimeNodeSeq[i] = -100000;
                    }
                    assTime.start();
                    font.setPixelSize(25);
                    danmakuRecorder = new DanmakuRecorder(1280, 720, logfilename);
                }
                else if (danmakuRecorder == nullptr)
                {
                    for (int i = 0; i < 24; i++)
                    {
                        danmakuWidthSeq[i] = -1000000;
                        danmakuTimeNodeSeq[i] = -100000;
                    }
                    assTime.start();
                    font.setPixelSize(25);
                    danmakuRecorder = new DanmakuRecorder(1280, 720, logfilename);
                    if (f.open(QIODevice::ReadOnly))
                    {
                        f.seek(f.size()-2);
                        int count = 0;
                        QString ch = "";
                        do
                        {
                            ch = f.read(1);
                            f.seek(f.pos()-2);
                            qDebug() << ch << f.pos() << f.size();
                        }while ( (ch == " ") || (ch == "\n") || (ch == "\r"));
                        while ( (ch != "\r") && (ch != "\n") && (f.pos() > 0) )
                        {
                            ch = f.read(1);
                            f.seek(f.pos()-2);
                            count++;
                        }
                        f.seek(f.pos()+2);
                        QByteArray blastline = f.read(count);
                        QString str = QString::fromUtf8(blastline);
                        QRegExp rxstr("Dialogue:.+");
                        rxstr.setMinimal(false);
                        if(rxstr.exactMatch(str))
                        {
                            QStringList list1 = str.split(",");
                            QTime strtime = QTime::fromString(list1[2], "h:mm:ss.z");
                            danmakuRecorder->baseTimeMs = strtime.msecsSinceStartOfDay();
//                            qDebug() << list1[2] << strtime << danmakuRecorder->baseTimeMs;
                        }
//                        qDebug() << str << count;
                        f.close();
                    }
                }
                else if (danmakuRecorder != nullptr)
                {
                    danmakuRecorder->resume();
                }
            }
            else
            {
                QString str_dir = QCoreApplication::applicationDirPath();
                QString str_date = QDateTime::currentDateTime().toString("yyyyMMdd");
                //QMessageBox::information(this, tr("Danmu Path"), str_dir + "/" + roomid + "_" + str_date + "_log.txt");
                logfilename = str_dir + "/" + roomid + "_" + str_date + "_log.txt";
                QFile f(logfilename);
                if (!f.exists())
                {
                    f.open(QIODevice::WriteOnly);
                    f.close();
                }
            }
            tcpSocket->connectDanmuServer(roomid);
            roomStatusTimer->start(_Douyu_RoomStatus_Intervals);
        }
    }
    else
    {
        ui->lineEdit_roomid->setText(QString(""));
    }
}

void MainWindow::stop()
{
    tcpSocket->tcpDanmuSoc.close();
    tcpSocket->timer->stop();
    if (danmakuRecorder != nullptr)
    {
        danmakuRecorder->pause();
    }
}

void MainWindow::htmlContent(const QString& html)
{
    //正则数据提取JSON
    QString pattern = _Douyu_Room_Pattern;
    QRegExp regExp(pattern);
    regExp.setMinimal(true);
    QString json;
    QMessageBox::information(this, tr("roomapi: "), html);
    int pos = 0;
    while((pos = regExp.indexIn(html,pos)) != -1)
    {
        json = regExp.capturedTexts().at(1);
        pos += regExp.matchedLength();
    }
    JSONParse parse;
    if(parse.init(json))
    {
        QString roomid = parse.getJsonValue(_Douyu_RoomId);
        ui->lineEdit_roomid->setText(QString(roomid));
        tcpSocket->connectDanmuServer(roomid);
    }
    else
    {
        ui->lineEdit_roomid->setText(QString(""));
        ui->lineEdit_roomid->setPlaceholderText(tr("load fail"));
//        ui->plainTextEdit->setPlainText(QString(""));

    }

}


void MainWindow::showChatMessage(QMap<QString,QString> messageMap)
{
    QPlainTextEdit *edit = nullptr;
    if(messageMap["type"] == "connectstate")
    {//连接状态
//        edit = static_cast<QPlainTextEdit *>(ui->tabWidget->widget(0));
        edit = static_cast<QPlainTextEdit *>(ui->plainTextEdit_status);
    }
    else if(messageMap["type"] == "chatmsg" || messageMap["type"] == "uenter")
    {//弹幕消息
//        edit = static_cast<QPlainTextEdit *>(ui->tabWidget->widget(1));
        edit = static_cast<QPlainTextEdit *>(ui->plainTextEdit_danmu);
    }
    else if(messageMap["type"] == "dgb" || messageMap["type"] == "bc_buy_deserve")
    {//打赏消息
//        edit = static_cast<QPlainTextEdit *>(ui->tabWidget->widget(2));
        edit = static_cast<QPlainTextEdit *>(ui->plainTextEdit_gift);
    }
    else
    {//其他
//        edit = static_cast<QPlainTextEdit *>(ui->tabWidget->widget(3));
        edit = static_cast<QPlainTextEdit *>(ui->plainTextEdit_other);
    }
    if(edit != nullptr)
    {
        QString editMessage = StringGenerator::getString(messageMap);
        edit->appendHtml(editMessage);
        showChatMessageString(editMessage);
    }

}

void MainWindow::showChatMessageString(QString message)
{
//    QString str_dir = QCoreApplication::applicationDirPath();
//    QString roomid = (ui->lineEdit_roomid->text()).trimmed();
//    QString str_date = QDateTime::currentDateTime().toString("yyyyMMdd");
//    QFile logfile(str_dir + "/" + roomid + "_" + str_date + "_log.txt");
    QFile logfile(logfilename);
    if(logfile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        QTextStream outputStream(&logfile);
        outputStream.setCodec("UTF-8");
        if (ui->checkBox_ass->isChecked())
        {
           Danmaku_t d;
           int resWidth = 1280;
           int resHeight = 720;
           d.speaker = "";
           d.text = message.remove(QRegExp("<[^>]*>"));
           d.posX = resWidth;
           QFontMetrics fm(font);
           d.length = fm.width(d.text);
           d.step = 2.0 * sqrt(sqrt(d.length/250.0)) + 0.5;
           int availDChannel = getAvailDanmakuChannel(d.step);
           if (availDChannel < 0 && danmakuRecorder == nullptr)
               return;
           int danmakuPos = availDChannel * (resHeight / 24);
           d.posY = danmakuPos;
           if (danmakuRecorder != nullptr)
           {
               danmakuRecorder->start();
               int duration = (resWidth + d.length) / (60.0 * d.step) * 1000.0;
               danmakuRecorder->danmaku2ASS(d.speaker, d.text, duration, d.length, 24, availDChannel);
           }
           if (availDChannel >= 0)
           {
               danmakuTimeNodeSeq[availDChannel] = assTime.elapsed();
               danmakuWidthSeq[availDChannel] = d.length;
               danmakuSpeedSeq[availDChannel] = d.step;
           }
        }
        else
        {
            outputStream << message.remove(QRegExp("<[^>]*>")) + "\n";
            outputStream.flush();
            logfile.close();
        }
    }
}


void MainWindow::on_actionEnglish_triggered()
{
    qApp -> removeTranslator(douyuTranslator);
    qApp -> removeTranslator(qt_translator);
    qApp -> removeTranslator(qtw_translator);
    qDebug() << "english";
}

void MainWindow::on_actionChimese_Simplifed_triggered()
{
    qt_translator->load(":/qm/qtwidgets_zh_CN.qm");
    qApp -> installTranslator(qt_translator);
    qtw_translator->load(":/qm/qt_zh_CN.qm");
    qApp -> installTranslator(qtw_translator);
    douyuTranslator->load(":/qm/Douyu_zh_CN.qm");
    qApp -> installTranslator(douyuTranslator);
    qDebug() << "chinese";
}

void MainWindow::changeEvent(QEvent *event)
{
    if(event -> type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
        ui->lineEdit_roomid->setText(QString(roomidStatus));
        roomStatus(roomidStatus);
//        ui->tabWidget->setTabText(0, QApplication::translate("MainWindow", "Status", 0));
//        ui->tabWidget->setTabText(1, QApplication::translate("MainWindow", "Danmuku", 0));
//        ui->tabWidget->setTabText(2, QApplication::translate("MainWindow", "Gift", 0));
//        ui->tabWidget->setTabText(3, QApplication::translate("MainWindow", "Other", 0));

        qDebug() << "language change";
    }
    else
    {
        QWidget::changeEvent(event);
    }
}

void MainWindow::roomStatus(QString& roomid)
{
    QString url_str = QString(_Douyu_RoomApiServer_HostName + "%1").arg(roomid);
    QUrl url = QUrl(url_str);
    auto *myManager = new QNetworkAccessManager();
    QNetworkRequest myRequest;
//    connect(&networkManager, SIGNAL(finished(QNetworkReply*)),
//            this, SLOT(roomJson(QNetworkReply*)));
    QObject::connect(myManager, &QNetworkAccessManager::finished,
    this, [=](QNetworkReply *myReply) {
        if (myReply->error()) {
            qDebug() << "error:" << myReply->errorString();
            return;
        }
        QByteArray answer = myReply->readAll();
        myReply->deleteLater();
        qDebug() << "answer:" << answer;
        myManager->deleteLater();
        QJsonDocument roomStatusJson;
        QJsonParseError roomStatusJsonError;
        roomStatusJson = QJsonDocument::fromJson(answer, &roomStatusJsonError);
        if (!roomStatusJson.isNull() && (roomStatusJsonError.error == QJsonParseError::NoError))
        {
            if (roomStatusJson.isObject())
            {
                QJsonObject object = roomStatusJson.object();
                if (object.contains("error"))
                {
                    QJsonValue errorValue = object.value("error");
                    if (errorValue == 0)
                    {
                        if (object.contains("data"))
                        {
                            QJsonValue dataValue = object.value("data");
                            if (dataValue.isObject())
                            {
                                const QString &douyutital = dataValue.toObject()["room_name"].toString();
                                this->setWindowTitle(douyutital);
                                roomStatusBar->setText(QString("%1%2  %3%4").arg(
                                                           tr("last streaming: "), dataValue.toObject()["start_time"].toString(),
                                                           tr("online hot: "), QString::number(dataValue.toObject()["hn"].toInt())));
//                                ui->statusBar->showMessage(QString("%1%2  %3%4").arg(
//                                                               tr("last streaming: "), dataValue.toObject()["start_time"].toString(),
//                                                               tr("online hot: "), QString::number(dataValue.toObject()["hn"].toInt())));
                            }
                        }
                    }
                }
            }
        } else {
            qDebug() << roomStatusJsonError.errorString();
        }
    }
    );
    myRequest.setUrl(url);
    myManager->get(myRequest);
    qDebug() << "roomStatus" << url_str;
}
