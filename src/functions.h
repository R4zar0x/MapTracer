#pragma once
#include "RouteTreeUI.h"

struct ThreadContext 
{
    HWND hWnd;
    std::vector<RouteTreeUI>* map;
    size_t currentMap;
};

void writeCursorPos(HWND hWnd, POINT& CursorPos);

POINT getCursorOffset(bool isFirstMove, POINT& prevCursorPos);

void setWhiteBackground(HDC hdc, HWND hwnd);