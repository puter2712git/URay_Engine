namespace URay
{

template <typename... Args>
RayHandle EventRay<Args...>::Register(Callback callback)
{
    const RayHandle newHandle = nextHandle++;
    callbacks.insert({ newHandle, std::move(callback) });
    return newHandle;
}

template <typename... Args>
void EventRay<Args...>::Unregister(RayHandle handle)
{
    callbacks.erase(handle);
}

template <typename... Args>
void EventRay<Args...>::Emit(Args... args)
{
    for (auto& [handle, callback] : callbacks)
    {
        callback(args...);
    }
}

} // namespace URay