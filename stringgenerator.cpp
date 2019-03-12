﻿#include "stringgenerator.h"
#include "douyutcpsocket.h"
#include <QMessageBox>

/**
 * @brief StringGenerator::StringGenerator
 * html字符串生成
 */
StringGenerator::StringGenerator()
= default;

QString StringGenerator::getDashedLine()
{
    return TE(QString("#787775"),QString("consolas"),QString("----------------------------"));
}

QString StringGenerator::getString(QMap<QString, QString> &messageMap)
{
    QString str = "";
    QString font_color_blue = "#3B91C5";
    QString font_color_red = "#E34945";
    QString font_color_black = "#454545";
    QString font_MS = "Microsoft YaHei";
    QString font_Con = "consolas";
    QString datetime = QDateTime::currentDateTime().toString("MM-dd hh:mm:ss");
    QString Ctime = "";

    if(messageMap["type"] == "chatmsg") //聊天
    {
        Ctime = QTime::currentTime().toString("hh:mm:ss.zzz");
        str = QString("%1 %2 %3 %4 %5").arg(TE(font_color_red,font_MS,Ctime))
                .arg(TE(font_color_blue,font_MS,messageMap["nn"]))
                .arg(TE(font_color_red,font_Con,"[lv."+messageMap["level"]+"]"))
                .arg(TE(font_color_blue,font_MS,":"))
                .arg(TE(font_color_black,font_MS,messageMap["txt"]));
     }
    else if(messageMap["type"] == "ssd") //超级弹幕消息
    {
        str = QString("%1 %2 %3").arg(TE(font_color_red,font_Con,"["+datetime+"]"))
                .arg(TE(font_color_blue,font_MS,"超级弹幕id："+messageMap["sdid"]))
                .arg(TE(font_color_black,font_MS,"内容："+messageMap["content"]));
    }

    else if(messageMap["type"] == "onlinegift") //领取鱼丸暴击
    {
        str = QString("%1 %2 %3").arg(TE(font_color_red,font_Con,"["+datetime+"]"))
                .arg(TE(font_color_blue,font_MS,"用户："+messageMap["nn"]))
                .arg(TE(font_color_black,font_MS,"暴击鱼丸"+messageMap["sil"]));
    }
    else if(messageMap["type"] == "ggbb") //房间用户抢红包
    {
        str = QString("%1 %2 %3").arg(TE(font_color_red,font_Con,"["+datetime+"]"))
                .arg(TE(font_color_blue,font_MS,"用户："+messageMap["did"]))
                .arg(TE(font_color_black,font_MS,"抢到礼包鱼丸"+messageMap["sl"]));
    }
    else if(messageMap["type"] == "ranklist") //广播排行榜消息
    {
        str = QString("%1 %2 %3").arg(TE(font_color_red,font_Con,"["+datetime+"]"))
                .arg(TE(font_color_blue,font_MS,"房间id："+messageMap["rid"]))
                .arg(TE(font_color_black,font_MS,"日榜："+messageMap["list_day"]));
    }
    else if(messageMap["type"] == "rankup") //房间排行榜变化消息
    {
        str = QString("%1 %2 %3").arg(TE(font_color_red,font_Con,"["+datetime+"]"))
                .arg(TE(font_color_blue,font_MS,"用户id："+messageMap["nk"]))
                .arg(TE(font_color_black,font_MS,"排名："+messageMap["rn"]));
    }
    else if(messageMap["type"] == "rss") //房间开播提醒
    {
        str = QString("%1 %2 %3").arg(TE(font_color_red,font_Con,"["+datetime+"]"))
                .arg(TE(font_color_blue,font_MS,"直播状态："+messageMap["ss"]))
                .arg(TE(font_color_black,font_MS,"开关播原因："+messageMap["rt"]));
    }
    else if(messageMap["type"] == "dgb") //赠送礼物
    {
        /*-----------------------------------------------------------------------------------
         * gfid    |  191   |     193        |  192       |  194     |  54         |     59     |
         * ---------------------------------------------------------------------------------
         * 表示内容| 100鱼丸|弱鸡(0.2鱼翅)|赞(0.1鱼翅)|666(6鱼翅)|飞机(100鱼翅)|火箭(500鱼翅)|
         * ----------------------------------------------------------------------------------
         *
         */

        int gfid = messageMap["gfid"].toInt();
        QString gfid_str = "";
        switch (gfid) {
        case 191:
            gfid_str = "100鱼丸";
            break;
        case 194:
            gfid_str = "6鱼翅(666)";
            break;
        case 193:
            gfid_str = "弱鸡(0.2鱼翅)";
            break;
        case 195:
            gfid_str = "100鱼翅(飞机)";
            break;
        case 192:
            gfid_str = "0.1鱼翅(赞)";
            break;
        case 59:
            gfid_str = "500鱼翅(火箭)";
            break;
        default:
            gfid_str = "什么鬼?(礼物)";
            break;
        }
        Ctime = QTime::currentTime().toString("hh:mm:ss.zzz");
        str = QString("%1 %2 %3 %4").arg(TE(font_color_red,font_MS,Ctime))
                .arg(TE(font_color_blue,font_MS,messageMap["nn"]))
                .arg(TE(font_color_black,font_MS,"赠送给主播"))
                .arg(TE(font_color_red,font_Con+","+font_MS,gfid_str+"("+messageMap["gfid"]+")"));
    }
    else if(messageMap["type"] == "uenter") //特殊身份用户进入房间
    {
        Ctime = QTime::currentTime().toString("hh:mm:ss.zzz");
        str = QString("%1 %2 %3 %4 %5").arg(TE(font_color_red,font_MS,Ctime))
                .arg(TE(font_color_black,font_MS,messageMap["nn"]))
                .arg(TE(font_color_red,font_MS,"[lv."+messageMap["level"]+"]"))
                .arg(TE(font_color_blue,font_MS,"进入直播间"))
                .arg(TE(font_color_black,font_MS,messageMap["rid"]));
    }
    else if(messageMap["type"] == "bc_buy_deserve") //用户赠送酬勤通知消息
    {
        Ctime = QTime::currentTime().toString("hh:mm:ss.zzz");
        str = QString("%1 %2 %3 %4 %5").arg(TE(font_color_red,font_MS,Ctime))
                .arg(TE(font_color_black,font_MS,"酬勤赠送:"))
                .arg(TE(font_color_red,font_MS,"赠送数量:"+messageMap["cnt"]))
                .arg(TE(font_color_blue,font_MS,"酬勤等级:"+messageMap["lev"]))
                .arg(TE(font_color_black,font_MS,"用户信息:"+messageMap["sui"]));

    }
    else if(messageMap["type"] == "connectstate")
    {
        str = QString("%1 %2 %3").arg(TE(font_color_red,font_Con,"["+messageMap["time"]+"]"))
                .arg(TE(font_color_blue,font_MS,"房间号:"+messageMap["roomid"]))
                .arg(TE(font_color_black,font_MS,messageMap["state"]));
    }
    else
    {
        str = QString("%1 %2 %3").arg(TE(font_color_red,font_Con,"["+messageMap["time"]+"]"))
                .arg(TE(font_color_blue,font_MS,messageMap["type"]))
                .arg(TE(font_color_black,font_MS,messageMap["gid"]));
    }
    return str;
}


QString StringGenerator::TE(const QString& color, const QString& font_family,const QString& txt)
{
    return QString("<font style=\"font-family:%1;color:%2\">%3</font>").arg(font_family)
            .arg(color).arg(txt);
}
