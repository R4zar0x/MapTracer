#include "RouteTreeUI.h"


RouteTreeUI::RouteTreeUI() :
    routeTree()
{}

RouteTreeUI::RouteTreeUI(const RouteTree& tree) :
    routeTree(tree)
{}

void RouteTreeUI::draw(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    // Залить клиентскую область окна белым цветом
    RECT rect;
    GetClientRect(hWnd, &rect);
    FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

    // Устанавливаем шрифт
    HFONT hFont = CreateFont(
        fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Consolas")
    );
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    // Рисуем дерево
    drawTree(hdc);

    // Отображаем список целей в окне
    std::string targets;
    for (size_t iter = 0; iter < targetURL.size(); iter++)
    {
        targets += targetURL[iter] + ": " + targetIP[iter] + "\r\n";
    }
    SetWindowText(GetDlgItem(hWnd, ID_EDIT_TARGETS), targets.c_str());

    // Очистка ресурсов
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    EndPaint(hWnd, &ps);
}

void RouteTreeUI::moveOn(POINT& shift)
{
    this->coordRoot.x += shift.x;
    this->coordRoot.y += shift.y;
}

void RouteTreeUI::addTarget(std::vector<Router> routers, std::string target)
{
    for (auto address : this->targetURL)
    {
        if (address == target)
        {
            return;
        }
    }

    this->targetURL.push_back(target);
    this->targetIP.push_back(routers[routers.size() - 1].getStringAddress());

    this->routeTree.addRoute(routers);
}

RouteTree* RouteTreeUI::getRouteTreePtr()
{
    return &routeTree;
}



POINT RouteTreeUI::findPointOnCircle(POINT center, int r, POINT target)
{
    double vx = static_cast<double>(target.x - center.x);
    double vy = static_cast<double>(target.y - center.y);

    double length = std::sqrt(vx * vx + vy * vy);

    if (length == 0)
    {
        return { center.x, center.y };
    }

    double ux = r * (vx / length);
    double uy = r * (vy / length);

    int x = static_cast<int>(std::round(center.x + ux));
    int y = static_cast<int>(std::round(center.y + uy));

    return { x, y };
}

// Функция для расчёта координат узлов
void RouteTreeUI::calculateNodePositions(
    const RouteTree::Vertex* node,
    int& currentX,
    int depth,
    std::vector<NodePosition>& positions
)
{
    if (!node) return;

    int subtreeStartX = currentX;

    // Рассчитываем позиции для детей
    for (const auto& child : node->getChildren())
    {
        calculateNodePositions(child.get(), currentX, depth + 1, positions);
    }

    // Рассчитываем позицию текущего узла
    if (node->getChildren().empty())
    {
        // Лист: используем текущий X
        positions.push_back({ node, {currentX, depth * yOffset + coordRoot.y} });
        currentX += xOffset; // Смещаем X вправо
    }
    else
    {
        // Узел с детьми: занимаем среднее положение среди детей
        int centerX = (subtreeStartX + currentX - xOffset) / 2;
        positions.push_back({ node, {centerX, depth * yOffset + coordRoot.y} });
    }
}

// Рисование дерева на основе координат
void RouteTreeUI::drawTree(HDC hdc)
{
    const RouteTree::Vertex* root = routeTree.getRoot();
    if (!root) return;

    std::vector<NodePosition> positions;
    int currentX = coordRoot.x; // Начальная координата X
    calculateNodePositions(root, currentX, 0, positions);

    // Рисуем все узлы и линии
    for (const auto& pos : positions)
    {
        const auto& node = pos.node;
        const auto& coords = pos.pos;

        // Рисуем узел
        Ellipse(
            hdc,
            coords.x - nodeRadius,
            coords.y - nodeRadius,
            coords.x + nodeRadius,
            coords.y + nodeRadius
        );

        std::string address = node->getRouter().getStringAddress();
        TextOut(hdc, coords.x + nodeRadius, coords.y - nodeRadius * 2, address.c_str(), address.size());

        // Рисуем линии к детям
        for (const auto& child : node->getChildren())
        {
            auto it = std::find_if(
                positions.begin(),
                positions.end(),
                [&child](const NodePosition& p) { return p.node == child.get(); }
            );
            if (it != positions.end())
            {
                // MoveToEx(hdc, coords.x, coords.y, NULL);
                //LineTo(hdc, it->pos.x, it->pos.y);

                auto center1 = findPointOnCircle(coords, nodeRadius, it->pos);
                auto center2 = findPointOnCircle(it->pos, nodeRadius, coords);

                MoveToEx(hdc, center1.x, center1.y, NULL);
                LineTo(hdc, center2.x, center2.y);
            }
        }
    }
}
