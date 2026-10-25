namespace URay
{

template <typename... Args>
void Ray<Args...>::Register(const void* owner, Callback callback)
{
    callbacks[owner].push_back(callback);
}

template <typename... Args>
void Ray<Args...>::UnregisterAll(const void* owner)
{
    callbacks.erase(owner);
}

template <typename... Args>
void Ray<Args...>::Emit(Args... args) const
{
    for (auto& [owner, callbackArray] : callbacks)
    {
        for (auto& cb : callbackArray)
        {
            cb(args...);
        }
    }
}

} // namespace URay
