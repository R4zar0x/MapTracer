#include <windows.h>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include "RouteTree.h"

#define ID_EDIT_TARGETS 7

class RouteTreeUI 
{
private:
    std::vector<std::string> targetURL;
    std::vector<std::string> targetIP;

    RouteTree routeTree;
    POINT coordRoot = { 450, 30 };

    int fontSize = 15;
    int nodeRadius = 12;

    int yOffset = 50;
    int xOffset = 130;

public:
    RouteTreeUI();
    RouteTreeUI(const RouteTree& tree);

    void draw(HWND hWnd);

    void moveOn(POINT& shift);

    void addTarget(std::vector<Router> routers, std::string target);

    RouteTree* getRouteTreePtr();

private:
    struct NodePosition 
    {
        const RouteTree::Vertex* node;
        POINT pos;
    };

    POINT findPointOnCircle(POINT center, int r, POINT target);

    // Функция для расчёта координат узлов
    void calculateNodePositions(
        const RouteTree::Vertex* node,
        int& currentX,
        int depth,
        std::vector<NodePosition>& positions
    );

    // Рисование дерева на основе координат
    void drawTree(HDC hdc);
};
