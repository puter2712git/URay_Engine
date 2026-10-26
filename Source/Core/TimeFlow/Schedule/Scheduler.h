#pragma once

#include <Core/Type/TypeAlias.h>

#include <functional>
#include <unordered_map>

namespace URay
{

struct Task
{
    uint64 id = 0;
    double delay = 0;
    std::function<void()> callback;
};

class Scheduler
{
public:
    using TaskId = uint64;

public:
    void Update(double deltaTime);

    TaskId ScheduleOnce(double delay, std::function<void()> callback);
    void Cancel(TaskId id);

private:
    std::unordered_map<TaskId, Task> tasks;

    TaskId nextId = 0;
};

} // namespace URay
