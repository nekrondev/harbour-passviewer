#include "currencyformat.h"

CurrencyFormat::CurrencyFormat(QObject *parent) :
    QObject(parent),
    m_currencies(),
    m_decimalPoint()
{
    setlocale(LC_ALL, "");
    lconv* lc = localeconv();
    m_decimalPoint = QString(lc->mon_decimal_point);
    if (m_decimalPoint == "")
        m_decimalPoint = QString(lc->decimal_point);
    QFile currencyFile(SailfishApp::pathTo("qml/lib/currencies.json").path());
    if (currencyFile.open(QFile::ReadOnly | QFile::Text)) {
        m_currencies = QJsonDocument::fromJson(currencyFile.readAll());
    }
}

QString CurrencyFormat::format(double value, QString isoName) {
    QJsonObject currency = m_currencies.object().value(isoName).toObject();
    QString symbol(isoName);
    int fractional = 2;
    if (!currency.isEmpty()) {
        symbol = currency.value("symbol").toString();
        fractional = currency.value("fractional").toInt();
    }
    double integral;
    double decimal = modf(value, &integral);
    QString formatted(symbol + " " + QString::number(integral, 'f', 0));
    if (decimal != 0.0) {
        decimal = floor(fabs(decimal) * pow(10, fractional));
        formatted += m_decimalPoint + QString::number(decimal, 'f', 0);
    }
    return formatted;
}
