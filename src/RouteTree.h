#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>

#include "Router.h"

class RouteTree
{
public:
    class Vertex
    {
    public:
        Vertex(const Router& router, Vertex* parent);
        Vertex(const Vertex& other, Vertex* parent);

        const Router& getRouter() const;
        const std::vector<std::unique_ptr<Vertex>>& getChildren() const;

        void addChild(std::unique_ptr<Vertex> child);

        // Получить полный путь от корня до текущей вершины
        std::vector<Router> getPath() const;

    private:
        Router router;
        Vertex* parent;
        std::vector<std::unique_ptr<Vertex>> children;
    };

    RouteTree();
    RouteTree(const RouteTree& other);

    RouteTree& operator=(const RouteTree& other);

    // Добавить новый маршрут
    void addRoute(const std::vector<Router>& route);

    // Вывод дерева
    void print() const;

    // Поиск всех маршрутов, проходящих через заданный маршрутизатор
    std::vector<std::vector<Router>> findRoutesThrough(const std::string& routerAddress) const;

    const RouteTree::Vertex* getRoot() const;

private:
    std::unique_ptr<Vertex> root;
    std::unordered_map<std::string, Vertex*> routerIndex;

    void printVertex(const Vertex* vertex, int depth) const;
    void collectRoutes(const Vertex* vertex, std::vector<std::vector<Router>>& routes) const;
    void rebuildRouterIndex();
    void rebuildRouterIndexRecursive(Vertex* node);
};
