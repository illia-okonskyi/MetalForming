#pragma once
#include <QtCore/QString>
typedef enum _tagCOutputFormat
{
    // Definitions:
    //   a = digit
    //   b = whitespace
    //   . = point
    //   c = didit/sign
    //   d = digit/whitespace
    //   - = minus

    // Range : (-10; 100)
    // Format: ca
    // ex.   : b1
    // ex.   : -1
    // ex.   : 12
    OutputFormat_I2 = 0x00,
    OutputFormat_I2_RangeMin = -10,
    OutputFormat_I2_RangeMax = 100,

    // Range : (-10000; 100000)
    // Format: cccca
    // ex.   : bb123
    // ex.   : b-123
    // ex.   : bbbb1
    OutputFormat_I5 = 0x01,
    OutputFormat_I5_RangeMin = -10000,
    OutputFormat_I5_RangeMax = 100000,

    // Range : (-100; 1000)
    // Format: cca.aa
    // ex.   : 123.00
    // ex.   : b23.15
    // ex.   : bb5.00
    // ex.   : 123.45
    // ex.   : b-1.50
    // ex.   : bb0.12
    OutputFormat_F6_2 = 0x10,
    OutputFormat_F6_2_RangeMin = -100,
    OutputFormat_F6_2_RangeMax = 1000,

    // Range : (-1000000; 10000000)
    // Format: cccccca.aa
    // ex.   : bbbb123.00
    // ex.   : bbbbb23.15
    // ex.   : bbbbbb5.00
    // ex.   : bbbb123.45
    // ex.   : bbbbb-1.50
    // ex.   : bbbbbb0.12
    OutputFormat_F10_2 = 0x11,
    OutputFormat_F10_2_RangeMin = -1000000,
    OutputFormat_F10_2_RangeMax = 10000000,
} COutputFormat;

class COutputFormatter
{
public:
    static QString formatValue(unsigned int value, COutputFormat of);
    static QString formatValue(int value, COutputFormat of);
    static QString formatValue(double value, COutputFormat of);
    static QString formatEndl();
};
