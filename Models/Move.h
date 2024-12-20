﻿#pragma once
#include <stdlib.h>

typedef int8_t POS_T;
//структура хода
struct move_pos
{
    POS_T x, y;             // откуда
    POS_T x2, y2;           // куда
    POS_T xb = -1, yb = -1; // координаты, если кого то "побили"

    move_pos(const POS_T x, const POS_T y, const POS_T x2, const POS_T y2) : x(x), y(y), x2(x2), y2(y2)
    {
    }
    move_pos(const POS_T x, const POS_T y, const POS_T x2, const POS_T y2, const POS_T xb, const POS_T yb)
        : x(x), y(y), x2(x2), y2(y2), xb(xb), yb(yb)
    {
    }

    bool operator==(const move_pos &other) const
    {
        return (x == other.x && y == other.y && x2 == other.x2 && y2 == other.y2);
    }
    bool operator!=(const move_pos &other) const
    {
        return !(*this == other);
    }
};
