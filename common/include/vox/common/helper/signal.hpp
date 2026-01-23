#pragma once

using CallbackID = u32;

template<typename Owner, typename ...Args>
class Signal {
friend Owner;
public:
    using Callback = std::function<void(Args...)>;

    CallbackID connect(Callback callback) {
        CallbackID id = ++m_current_id;
        m_slots[id] = callback;
        return id;
    }
    
    void disconnect(CallbackID id) {
        m_slots.erase(id);
    }
    
private:
    void emit(Args ...args) {
        for(const auto &[id, callback] : m_slots) {
            callback(args...);
        }
    }
    
private:
    std::map<CallbackID, Callback> m_slots;
    std::atomic<CallbackID> m_current_id{0};
};