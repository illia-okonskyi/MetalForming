#pragma once

enum CScaleIndexes
{
    ScaleIndexesCount = 7,
    ScaleIndex_Min = 0,
    ScaleIndex_Max = 6,
    ScaleIndex_Default = 3
};

const double g_fScales [ScaleIndexesCount] = {
    0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 5.0f, 10.0f };

