#include "RouteTree.h"

// Router::Vertex 

RouteTree::Vertex::Vertex(const Router& router, Vertex* parent = nullptr)
    : router(router), parent(parent)
{}

RouteTree::Vertex::Vertex(const Vertex& other, Vertex* parent)
    : router(other.router), parent(parent) {
    for (const auto& child : other.children) {
        children.push_back(std::make_unique<Vertex>(*child, this));
    }
}

const Router& RouteTree::Vertex::getRouter() const
{
    return router;
}
const std::vector<std::unique_ptr<RouteTree::Vertex>>& RouteTree::Vertex::getChildren() const
{
    return children;
}

void RouteTree::Vertex::addChild(std::unique_ptr<Vertex> child)
{
    children.push_back(std::move(child));
}

std::vector<Router> RouteTree::Vertex::getPath() const
{
    std::vector<Router> path;
    const Vertex* current = this;

    while (current != nullptr)
    {
        path.push_back(current->router);
        current = current->parent;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

// RouterTree

RouteTree::RouteTree() :
    root(std::make_unique<Vertex>(Router("localhost")))
{
    // Добавляем корневой маршрутизатор в индекс
    routerIndex[root->getRouter().getStringAddress()] = root.get();

}

RouteTree::RouteTree(const RouteTree& other) : root(nullptr) 
{
    if (other.root) 
    {
        root = std::make_unique<Vertex>(*other.root, nullptr); // Копируем дерево
        rebuildRouterIndex();
    }
}

RouteTree& RouteTree::operator=(const RouteTree& other) 
{
    if (this == &other) return *this; // Защита от самоприсваивания

    root = nullptr;
    if (other.root) {
        root = std::make_unique<Vertex>(*other.root, nullptr); // Копируем дерево
        rebuildRouterIndex();
    }
    return *this;
}

void RouteTree::addRoute(const std::vector<Router>& route)
{
    if (route.empty())
        return;

    Vertex* current = root.get();

    for (const Router& router : route)
    {
        // Проверяем, существует ли уже такой маршрутизатор
        auto existingRouter = routerIndex.find(router.getStringAddress());

        if (existingRouter != routerIndex.end())
        {
            // Если маршрутизатор существует, проверяем, является ли он прямым потомком
            bool isChild = false;
            for (const auto& child : current->getChildren())
            {
                if (child->getRouter().getStringAddress() == router.getStringAddress())
                {
                    current = child.get();
                    isChild = true;
                    break;
                }
            }

            if (!isChild)
            {
                // Если не является прямым потомком, создаем новую ветвь
                auto newVertex = std::make_unique<Vertex>(router, current);
                Vertex* newVertexPtr = newVertex.get();

                current->addChild(std::move(newVertex));
                current = newVertexPtr;

                routerIndex[router.getStringAddress()] = current;
            }
        }
        else
        {
            // Создаем новую вершину
            auto newVertex = std::make_unique<Vertex>(router, current);
            Vertex* newVertexPtr = newVertex.get();

            current->addChild(std::move(newVertex));
            current = newVertexPtr;

            routerIndex[router.getStringAddress()] = current;
        }
    }
}

void RouteTree::print() const
{
    printVertex(root.get(), 0);
}

std::vector<std::vector<Router>> RouteTree::findRoutesThrough(const std::string& routerAddress) const
{
    std::vector<std::vector<Router>> routes;
    auto it = routerIndex.find(routerAddress);

    if (it != routerIndex.end())
    {
        const Vertex* vertex = it->second;
        collectRoutes(vertex, routes);
    }

    return routes;
}

const RouteTree::Vertex* RouteTree::getRoot() const
{
    return root.get();
}

void RouteTree::printVertex(const Vertex* vertex, int depth) const
{
    std::string indent(depth * 2, ' ');
    std::cout << indent << vertex->getRouter().getStringAddress() << "\n";

    for (const auto& child : vertex->getChildren())
    {
        printVertex(child.get(), depth + 1);
    }
}

void RouteTree::collectRoutes(const Vertex* vertex, std::vector<std::vector<Router>>& routes) const
{
    std::vector<Router> currentPath = vertex->getPath();

    if (vertex->getChildren().empty())
    {
        routes.push_back(currentPath);
    }
    else
    {
        for (const auto& child : vertex->getChildren())
            collectRoutes(child.get(), routes);
    }
}

void RouteTree::rebuildRouterIndex() 
{
    routerIndex.clear();
    if (root) rebuildRouterIndexRecursive(root.get());
}

void RouteTree::rebuildRouterIndexRecursive(Vertex* node) 
{
    routerIndex[node->getRouter().getStringAddress()] = node;

    for (const auto& child : node->getChildren()) 
    {
        rebuildRouterIndexRecursive(child.get());
    }
}