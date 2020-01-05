//
// Created by dominik on 29.12.19.
//

#include <http-server/Building/BuildingSystem.h>
#include <http-server/Building/HttpAdapter.h>

HttpAdapter BuildingSystem::getHttpAdapter()
{
    return HttpAdapter(*this);
}

BuildingSystem::BuildingSystem()
{
    root.reset(new BuildingComposite(0, "System root"));
}

std::pair<std::shared_ptr<BuildingComponent>, std::queue<std::shared_lock<std::shared_mutex>>>
BuildingSystem::find(std::list<int> path)
{
    auto node = root;
    std::queue<std::shared_lock<std::shared_mutex>> locked_mutexes;
    while (path.front()!=0)
    {
        locked_mutexes.push(node->get_read_lock());
        int child_id = path.front();
        node = node->get_child(child_id);
        if (node == nullptr) throw HttpException(StatusCode::Gone, "Resource is gone.");
        path.pop_front();
    }
    return {node, std::move(locked_mutexes)};
}

std::string BuildingSystem::get_info(std::list<int> path)
{
    auto result = find(path);
    auto& node = result.first;
    auto& mutexes = result.second;
    auto node_lock = node->get_read_lock();
    return node->get_structure_json();
}

std::string BuildingSystem::get_equipment(std::list<int> path)
{
    auto result = find(path);
    auto& node = result.first;
    auto& mutexes = result.second;
    auto node_lock = node->get_read_lock();
    return node->get_equipment_json();
}

void BuildingSystem::add(std::list<int> path, std::shared_ptr<BuildingComponent> child)
{
    auto result = find(path);
    auto& node = result.first;
    auto& mutexes = result.second;
    auto node_lock = node->get_write_lock();
    node->add_child(child);
}

void BuildingSystem::add(std::list<int> path, std::shared_ptr<Equipment> eq)
{
    auto result= find(path);
    auto& node = result.first;
    auto& mutexes = result.second;
    auto node_lock = node->get_write_lock();
    node->add_equipment(eq);
}

void BuildingSystem::remove(std::list<int> path)
{
    if (!path.empty())
    {
        int removed_id = path.back();
        path.pop_back();
        auto result = find(path);
        auto& node = result.first;
        auto& mutexes = result.second;
        auto node_lock = node->get_write_lock();
        node->delete_child(removed_id);
    }
    else
    {
        root.reset(new BuildingComposite(0, "System root"));
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-goto"
void BuildingSystem::move(int eq_id, std::list<int> source_path, std::list<int> dest_path)
{
    std::pair<std::shared_ptr<BuildingComponent>, std::queue<std::shared_lock<std::shared_mutex>>> result;

    std::shared_ptr<BuildingComponent> source;
    std::queue<std::shared_lock<std::shared_mutex>> source_mutexes;
    std::unique_lock<std::shared_mutex> source_lock;

    std::shared_ptr<BuildingComponent> destination;
    std::queue<std::shared_lock<std::shared_mutex>> destination_mutexes;
    std::unique_lock<std::shared_mutex> destination_lock;

    bool swap_order = dest_path < source_path;
    if(swap_order) goto swap_first;
    swap_second:
    result = find(source_path);
    source = std::move(result.first);
    source_mutexes = std::move(result.second);
    source_lock = source->get_write_lock();
    if(swap_order) goto swap_end;

    swap_first:
    result = find(dest_path);
    destination = result.first;
    destination_mutexes = std::move(result.second);
    destination_lock = destination->get_write_lock();
    if(swap_order) goto swap_second;

    swap_end:
    auto equipment = source->get_equipment(eq_id);
    destination->add_equipment(equipment);
    source->delete_equipment(eq_id);
}
#pragma clang diagnostic pop
