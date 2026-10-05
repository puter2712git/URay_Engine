#include "Scheduler.h"

namespace URay
{

void Scheduler::Update(double deltaTime)
{
    std::vector<TaskId> finishedTaskIds;

    for (auto& [id, task] : tasks)
    {
        task.delay -= deltaTime;

        if (task.delay > 0)
            continue;

        finishedTaskIds.push_back(id);
    }

    for (const auto& id : finishedTaskIds)
    {
        auto it = tasks.find(id);
        if (it == tasks.end())
            continue;

        std::function<void()> callback = std::move(it->second.callback);
        tasks.erase(id);

        if (callback)
            callback();
    }
}

Scheduler::TaskId Scheduler::ScheduleOnce(double delay, std::function<void()> callback)
{
    const TaskId id = nextId++;

    Task newTask = {};
    newTask.id = id;
    newTask.delay = delay;
    newTask.callback = callback;

    tasks.emplace(id, newTask);

    return id;
}

void Scheduler::Cancel(TaskId id)
{
    tasks.erase(id);
}

} // namespace URay
