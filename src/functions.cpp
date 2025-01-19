#include "functions.h"


void writeCursorPos(HWND hWnd, POINT& CursorPos)
{
    GetCursorPos(&CursorPos);
    ScreenToClient(hWnd, &CursorPos);
}

POINT getCursorOffset(bool isFirstMove, POINT& prevCursorPos)
{
    // Текущая позиция курсора
    POINT currentCursorPos;
    GetCursorPos(&currentCursorPos);

    // Смещение курсора
    POINT offset = { 0, 0 };

    if (!isFirstMove) 
    {
        offset.x = currentCursorPos.x - prevCursorPos.x;
        offset.y = currentCursorPos.y - prevCursorPos.y;
    }

    // Обновляем предыдущую позицию курсора
    prevCursorPos = currentCursorPos;
    isFirstMove = false;

    return offset;
}

void setWhiteBackground(HDC hdc, HWND hwnd) {
    // Установить белый цвет фона
    SetBkColor(hdc, RGB(255, 255, 255));

    // Залить клиентскую область окна белым цветом
    RECT rect;
    GetClientRect(hwnd, &rect);
    FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));
}










