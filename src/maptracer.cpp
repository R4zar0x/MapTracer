#include "tracer.h"

#define PAGES           19
#define SHIFT           12

#define ID_STATIC_IP            1
#define ID_EDIT_IP              2
#define ID_BUTTON_IP            3

#define ID_BUTTON_L             4
#define ID_BUTTON_R             5
#define ID_STATIC_P             6


// Функция для создания элементов управления
void CreateControls(HWND hwnd);

void StartTraceroute(HWND hwnd, std::vector<RouteTreeUI>* map, size_t currentMap);

DWORD WINAPI TracerouteThread(LPVOID lpParam);

// Функция окна (обрабатывает сообщения)
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Главная точка входа
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
    // Регистрация класса окна
    const char CLASS_NAME[] = "Sample Window Class";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Создание окна
    HWND hwnd = CreateWindowEx(
        0,                          // Дополнительные стили окна
        CLASS_NAME,                 // Имя класса окна
        "maptracer",              // Заголовок окна
        WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX,        // Стиль окна
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,                       // Родительское окно
        NULL,                       // Меню
        hInstance,                  // Дескриптор приложения
        NULL                        // Дополнительные параметры
    );

    if (hwnd == NULL) 
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Цикл обработки сообщений
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
    static std::vector<RouteTreeUI>* map = nullptr;
    static size_t currentMap = NULL;
    
    switch (uMsg) 
    {
    case WM_CREATE:
    {
        map = new std::vector<RouteTreeUI>;
        currentMap = 0;

        for (int i = 0; i <= PAGES; i++)
        {
            map->push_back(RouteTreeUI());
        }

        CreateControls(hWnd);
    }
        break;
    case WM_PAINT:
    {   
        map->at(currentMap).draw(hWnd);
    }
        break;
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_BUTTON_IP:
        {
            StartTraceroute(hWnd, map, currentMap);
        }
            break;
        case ID_BUTTON_L:
        {
            if (currentMap > 0)
            {
                currentMap--;

                std::string str = "page: " + std::to_string(currentMap);
                SetWindowText(GetDlgItem(hWnd, ID_STATIC_P), str.c_str());

                InvalidateRect(hWnd, NULL, TRUE);
            }
            else
            {
                MessageBox(hWnd, "Page can`t be negative.", "Error", MB_OK);
            }
        }
            break; 
        case ID_BUTTON_R:
        {
            if (currentMap < PAGES)
            {
                currentMap++;
                
                std::string str = "page: " + std::to_string(currentMap);
                SetWindowText(GetDlgItem(hWnd, ID_STATIC_P), str.c_str());

                InvalidateRect(hWnd, NULL, TRUE);
            }
            else
            {
                MessageBox(hWnd, "Page can`t be more 19.", "Error", MB_OK);
            }
        }
            break; 
        default:
            break;
        }
    }
        break;

    case WM_KEYDOWN:
    {
        // Обработка клавиш стрелок для перемещения карты
        switch (wParam)
        {
        case VK_LEFT:
        {
            POINT shift{ -SHIFT, 0 };
            map->at(currentMap).moveOn(shift);
            InvalidateRect(hWnd, NULL, TRUE);
        } 
        break;
        case VK_RIGHT:
        {
            POINT shift{ SHIFT, 0 };
            map->at(currentMap).moveOn(shift);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;
        case VK_UP:
        {
            POINT shift{ 0, -SHIFT };
            map->at(currentMap).moveOn(shift);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;
        case VK_DOWN:
        {
            POINT shift{ 0, SHIFT };
            map->at(currentMap).moveOn(shift);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;
        default:
            break;
        }
    }
        break;
    case WM_RBUTTONDOWN:
    {
        SetFocus(hWnd);
    }
        break;
    case WM_DESTROY:
        if (map) delete map;

        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


void CreateControls(HWND hwnd)
{
    // IP

    CreateWindowEx(0, "STATIC", "target:", WS_CHILD | WS_VISIBLE,
        1080, 20, 50, 25, hwnd, (HMENU)ID_STATIC_IP, GetModuleHandle(NULL), NULL);

    CreateWindowEx(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
        1140, 20, 200, 25, hwnd, (HMENU)ID_EDIT_IP, GetModuleHandle(NULL), NULL);

    CreateWindowEx(0, "BUTTON", "trace", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_NOTIFY,
        1350, 20, 60, 25, hwnd, (HMENU)ID_BUTTON_IP, GetModuleHandle(NULL), NULL);

    // pages

    CreateWindowEx(0, "STATIC", "page: 0", WS_CHILD | WS_VISIBLE,
        40, 1, 60, 25, hwnd, (HMENU)ID_STATIC_P, GetModuleHandle(NULL), NULL);

    CreateWindowEx(0, "BUTTON", "left", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_NOTIFY,
        0, 0, 40, 25, hwnd, (HMENU)ID_BUTTON_L, GetModuleHandle(NULL), NULL);

    CreateWindowEx(0, "BUTTON", "right", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_NOTIFY,
        100, 0, 40, 25, hwnd, (HMENU)ID_BUTTON_R, GetModuleHandle(NULL), NULL);

    // targets

    CreateWindowEx(0, "EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        1100, 500, 320, 220, hwnd, (HMENU)ID_EDIT_TARGETS, GetModuleHandle(NULL), NULL);
}

void StartTraceroute(HWND hwnd, std::vector<RouteTreeUI>* map, size_t currentMap) 
{
    ThreadContext* context = new ThreadContext;
    context->hWnd = hwnd;
    context->map = map;
    context->currentMap = currentMap;

    CreateThread(NULL, 0, TracerouteThread, context, 0, NULL);
    InvalidateRect(hwnd, NULL, FALSE);
}

DWORD WINAPI TracerouteThread(LPVOID lpParam)
{
    ThreadContext* context = (ThreadContext*)lpParam;  // Cast the parameter to the appropriate type
    HWND hWnd = context->hWnd;
    std::vector<RouteTreeUI>* map = context->map;
    size_t currentMap = context->currentMap;

    char text[256];
    std::vector<Router> routers{};

    // Get the IP address from the input field
    GetWindowText(GetDlgItem(hWnd, ID_EDIT_IP), text, sizeof(text));
    std::string address = text;

    auto result = traceroute(address.data());

    if (!result.first)
    {
        MessageBox(hWnd, "Can't find IP.", "Error", MB_OK);
    }
    else if (address.size() == 0)
    {
        MessageBox(hWnd, "Addres is empty.", "Error", MB_OK);
    }
    else
    {
        for (std::string& path : result.second)
        {
            routers.push_back(Router(path));
        }

        map->at(currentMap).addTarget(routers, address);

        // Ensure thread-safe UI updates after traceroute completion
        PostMessage(hWnd, WM_USER + 1, 0, 0);  // Custom message to trigger re-drawing
    }

    delete context;  // Free the context memory after thread execution
    return 0;
}



