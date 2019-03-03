#include "networkaccess.h"
#include <QDebug>

NetworkAccess::NetworkAccess(QObject *parent):
    QObject(parent)
{
    codec = QTextCodec::codecForName("UTF-8");
    QObject::connect(&manager,SIGNAL(finished(QNetworkReply*)),
                     this,SLOT(replyFinished(QNetworkReply*)));
}

NetworkAccess::~NetworkAccess()
= default;

void NetworkAccess::loadingPage(QUrl &pageUrl,QTextCodec *codec)
{

    if(codec != nullptr)
        this->codec = codec;
//    QSslConfiguration config;
//    config.setPeerVerifyMode(QSslSocket::VerifyNone);
//    config.setProtocol(QSsl::TlsV1SslV3);
//    request.setSslConfiguration(config);
    request.setUrl(pageUrl);
    QNetworkReply* reply = manager.get(request);
    QUrl redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if(redirect.isValid() && reply->url() != redirect)
    {
        if(redirect.isRelative())
            redirect = reply->url().resolved(redirect);
        QNetworkRequest req(redirect);
        manager.get(req);
    }
}


void NetworkAccess::replyFinished(QNetworkReply *reply)
{
    QByteArray replyByte = reply->readAll();
    qDebug() << replyByte;
    QString html = this->codec->toUnicode(replyByte);
    qDebug() << html;
    reply->deleteLater();
    emit pageLoadFinished(html);
}
