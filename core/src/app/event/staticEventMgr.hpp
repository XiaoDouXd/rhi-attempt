#pragma once

#include <concepts>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <stack>
#include <type_traits>
#include <unordered_map>

namespace XD
{
    /// @brief 事件类型基类
    template<class EType, typename... ArgTypes>
    class EventTypeBase
    {
        friend class StaticEventMgr;
    public:
        using __cc_eType = EType;
        using __cc_fType = std::function<void(ArgTypes...)>;
        using __cc_isEventType = std::true_type;
    };

    class StaticEventMgr
    {
        friend class App;
    private:
        struct EventAsyncHandler;
        struct EventHandler
        {
        public:
            EventHandler(std::ptrdiff_t objId, std::unique_ptr<std::_Function_base>&& cb)
            :objId(objId), cb(std::move(cb)), waiting(std::queue<std::list<EventAsyncHandler>::const_iterator>()) {}
            std::ptrdiff_t objId    = 0;
            std::unique_ptr<std::_Function_base> cb = nullptr;
            std::queue<std::list<EventAsyncHandler>::const_iterator> waiting;
        };

        struct EventAsyncHandler
        {
        public:
            EventAsyncHandler(EventHandler* event, std::function<void()> invoke)
            :event(event), invoke(invoke) {}
            EventHandler* event = nullptr;
            std::function<void()> invoke;
        };

        class EventMgrData
        {
        public:
            /// @brief 静态事件 <事件id, <事件监听成员, EventHandler>>
            std::unordered_map<std::size_t, std::map<std::ptrdiff_t, EventHandler>> staticEvents;
            /// @brief 异步事件的等待队列
            std::list<EventAsyncHandler>                                            waitingQueue;
        };
        static std::unique_ptr<EventMgrData> _inst;

    public:
        /// @brief 注册事件 每个 obj 只能注册一次相同事件 多余的注册会被略过
        /// @tparam EType 注册的事件类型
        /// @param obj 监听器的 id (一般用对象内存地址描述)
        /// @param cb 事件的回调
        /// @return 注册到事件的哈希值包 (可以使用这个哈希值注销事件)
        template<class EType>
        requires EType::__cc_isEventType::value && std::is_same<typename EType::__cc_eType, EType>::value
        static std::optional<std::size_t> registerEvent(std::ptrdiff_t obj, EType::__cc_fType cb)
        {
            std::size_t hashCode = typeid(EType).hash_code();
            auto& eDic = _inst->staticEvents;
            if (eDic.find(hashCode) == eDic.end())
                eDic.insert({hashCode, std::map<std::ptrdiff_t, EventHandler>()});
            auto& lDic = eDic[hashCode];
            if (lDic.find(obj) != lDic.end()) return std::nullopt;
            lDic.insert(std::pair(obj, EventHandler(
                    obj,
                    std::unique_ptr<std::_Function_base>(reinterpret_cast<std::_Function_base*>(new EType::__cc_fType(cb)))
                )));
            return std::make_optional<std::size_t>(hashCode);
        }

        /// @brief 注销事件
        /// @tparam EType 注销的事件类型
        /// @param obj 监听器的 id (一般用对象内存地址描述)
        template<class EType>
        requires EType::__cc_isEventType::value && std::is_same<typename EType::__cc_eType, EType>::value
        static std::optional<std::size_t> unregisterEvent(std::ptrdiff_t obj)
        {
            std::size_t hashCode = typeid(EType).hash_code();
            auto& eDic = _inst->staticEvents;
            if (eDic.find(hashCode) == eDic.end()) return std::nullopt;
            auto& lDic = eDic[hashCode];
            if (lDic.find(obj) == lDic.end()) return std::nullopt;
            auto eH = lDic.find(obj);

            while (!eH->second.waiting.empty())
            {
                _inst->waitingQueue.erase(eH->second.waiting.front());
                eH->second.waiting.pop();
            }

            lDic.erase(eH);
            return std::make_optional<std::size_t>(hashCode);
        }

        /// @brief 注销事件
        /// @param hashCode 事件的哈希值
        /// @param obj 监听器的 id (一般用对象内存地址描述)
        static void unregisterEvent(const std::size_t& hashCode, std::ptrdiff_t obj);

        /// @brief 注销事件
        /// @param hashCodeOpt 事件的哈希值包
        /// @param obj 监听器的 id (一般用对象内存地址描述)
        static void unregisterEvent(const std::optional<std::size_t>& hashCodeOpt, std::ptrdiff_t obj);

        /// @brief 某事件的所有监听
        /// @tparam EType 事件类型
        template<class EType>
        requires EType::__cc_isEventType::value && std::is_same<typename EType::__cc_eType, EType>::value
        static void clearEvent()
        {
            std::size_t hashCode = typeid(EType).hash_code();
            auto& eDic = _inst->staticEvents;
            if (eDic.find(hashCode) == eDic.end()) return;
            for (auto& lDic : eDic[hashCode])
            {
                while (!lDic.second.waiting.empty())
                {
                    _inst->waitingQueue.erase(lDic.second.waiting.front());
                    lDic.second.waiting.pop();
                }
            }
            eDic.erase(hashCode);
        }

        /// @brief 广播
        /// @tparam EType 事件类型
        /// @tparam ...ArgTypes 参数包 (定义事件类型时所指定的参数)
        /// @param ...args 要传递参数
        template<class EType, typename... ArgTypes>
        requires EType::__cc_isEventType::value && std::is_same<typename EType::__cc_eType, EType>::value
        && std::is_same<typename EType::__cc_fType, std::function<void(ArgTypes...)>>::value
        static void broadcast(ArgTypes... args)
        {
            std::size_t hashCode = typeid(EType).hash_code();
            auto& eDic = _inst->staticEvents;
            if (eDic.find(hashCode) == eDic.end()) return;
            for (auto& lDic : eDic[hashCode])
            {
                auto cb = reinterpret_cast<typename EType::__cc_fType*>(lDic.second.cb.get());
                (*cb)(std::forward<ArgTypes>(args)...);
            }
        }

        /// @brief 异步地广播
        /// @tparam EType 事件类型
        /// @tparam ...ArgTypes 参数包 (定义事件类型时所指定的参数)
        /// @param ...args 要传递的参数
        template<class EType, typename... ArgTypes>
        requires EType::__cc_isEventType::value && std::is_same<typename EType::__cc_eType, EType>::value
        && std::is_same<typename EType::__cc_fType, std::function<void(ArgTypes...)>>::value
        static void broadcastAsync(ArgTypes... args)
        {
            std::size_t hashCode = typeid(EType).hash_code();
            auto& eDic = _inst->staticEvents;
            if (eDic.find(hashCode) == eDic.end()) return;
            for (auto& lDic : eDic[hashCode])
            {
                auto cb = reinterpret_cast<typename EType::__cc_fType*>(lDic.second.cb.get());
                _inst->waitingQueue.emplace_back(
                    EventAsyncHandler(
                        &lDic.second,
                        [cb, args...](){(*cb)(args...);}
                    ));
                lDic.second.waiting.push(--(_inst->waitingQueue.end()));
            }
        }

    private:
        static void init();
        static void update();
    };
}