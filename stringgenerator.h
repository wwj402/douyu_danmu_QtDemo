﻿#ifndef STRINGGENERATOR_H
#define STRINGGENERATOR_H
#include <QtCore>

/**
 * @brief The StringGenerator class
 * 生成界面显示字符串
 */
class StringGenerator
{
public:
    StringGenerator();
    static QString getString(QMap<QString,QString> &messageMap);
    static QString getDashedLine();
private:
    static QString TE(const QString& color, const QString& font_family, const QString& txt);

};

#endif // STRINGGENERATOR_H
