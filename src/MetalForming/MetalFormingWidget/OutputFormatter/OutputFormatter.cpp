#include "OutputFormatter.h"
#include <QtCore/QTextStream>

QString COutputFormatter::formatValue( unsigned int value, COutputFormat of )
{
    return formatValue(int(value), of);
}

QString COutputFormatter::formatValue( int value, COutputFormat of )
{
    int iRangeMin = 0;
    int iRangeMax = 0;
    QTextStream::FieldAlignment fa = QTextStream::AlignRight;
    int iFieldWidth = 0;

    switch(of)
    {
    case OutputFormat_I2:
        iRangeMin = OutputFormat_I2_RangeMin;
        iRangeMax = OutputFormat_I2_RangeMax;
        fa = QTextStream::AlignRight;
        iFieldWidth = 2;
        break;
    case OutputFormat_I5:
        iRangeMin = OutputFormat_I5_RangeMin;
        iRangeMax = OutputFormat_I5_RangeMax;
        fa = QTextStream::AlignRight;
        iFieldWidth = 5;
        break;
    default:
        return (QString("COutputFormatter::wrong format value passed.\n"
            "Value type: int\nFormat: %1\n").arg(of, 2, 16, QChar('0')));
    }

    if ((value <= iRangeMin) || (value >= iRangeMax))
        return (QString("COutputFormatter:: value range limit exceeded.\n"
        "Value: %1\nFormat: %2").arg(value).arg(of, 2, 16, QChar('0')));

    QString sResult;
    QTextStream ss(&sResult);

    ss.setFieldAlignment(fa);
    ss.setFieldWidth(iFieldWidth);

    ss << value << flush;
    return sResult;
}

QString COutputFormatter::formatValue( double value, COutputFormat of )
{
    double fRangeMin = 0;
    double fRangeMax = 0;
    QTextStream::FieldAlignment fa = QTextStream::AlignRight;
    QTextStream::RealNumberNotation rnNotation = QTextStream::SmartNotation;
    int iFieldWidth = 0;
    int iFieldPrecision = 0;

    switch(of)
    {
    case OutputFormat_F6_2:
        fRangeMin = OutputFormat_F6_2_RangeMin;
        fRangeMax = OutputFormat_F6_2_RangeMax;
        fa = QTextStream::AlignRight;
        rnNotation = QTextStream::FixedNotation;
        iFieldWidth = 6;
        iFieldPrecision = 2;
        break;
    case OutputFormat_F10_2:
        fRangeMin = OutputFormat_F10_2_RangeMin;
        fRangeMax = OutputFormat_F10_2_RangeMax;
        fa = QTextStream::AlignRight;
        rnNotation = QTextStream::FixedNotation;
        iFieldWidth = 10;
        iFieldPrecision = 2;
        break;
    default:
        return (QString("COutputFormatter::wrong format value passed.\n"
            "Value type: int\nFormat: %1\n").arg(of, 2, 16, QChar('0')));
    }

    if ((value <= fRangeMin) || (value >= fRangeMax))
        return (QString("COutputFormatter:: value range limit exceeded.\n"
        "Value: %1\nFormat: %2").arg(value).arg(of, 2, 16, QChar('0')));

    QString sResult;
    QTextStream ss(&sResult);

    ss.setFieldAlignment(fa);
    ss.setRealNumberNotation(rnNotation);
    ss.setFieldWidth(iFieldWidth);
    ss.setRealNumberPrecision(iFieldPrecision);

    ss << value << flush;
    return sResult;
}


QString COutputFormatter::formatEndl()
{
    QString sResult;
    QTextStream ss(&sResult);
    ss << endl << flush;
    return sResult;
}
