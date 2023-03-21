
#include "timeMgr.h"

#include <list>
#include <memory>
#include <queue>

namespace XD::Util::TimeMgr
{
    class TimeCallback
    {
    public:
        clock_t                 endTime;
        std::function<void()>   cb;
        TimeCallback(std::function<void()> cb, double endTime) :
        endTime(endTime), cb(cb) {}
    };
    class TimeCallbackCompare
    {
    public:
        bool operator()(const TimeCallback& a, const TimeCallback& b)
        { return a.endTime > b.endTime; }
    };

    struct Data
    {
    public:
        std::chrono::high_resolution_clock::time_point startPoint;
        std::priority_queue<
            TimeCallback,
            std::vector<TimeCallback>,
            TimeCallbackCompare> callbacks;
    };
    static std::unique_ptr<Data> _inst = nullptr;

    void init()
    {
        if (_inst) return;
        _inst = std::make_unique<Data>();
    }

    bool inited() { return (bool)_inst; }

    clock_t now()
    {
        return clock();
    }

    std::chrono::high_resolution_clock::time_point nowTimePoint()
    {
        return std::chrono::high_resolution_clock::now();
    }

    void delay(std::function<void()> cb, clock_t delay)
    {
        if (_inst->callbacks.empty()) return;
        _inst->callbacks.push(TimeCallback(cb, delay + now()));
    }

    void update()
    {
        if (_inst->callbacks.empty()) return;
        static bool run;
        do
        {
            run = now() >= _inst->callbacks.top().endTime;
            if (run)
            {
                _inst->callbacks.top().cb();
                _inst->callbacks.pop();
            }
        } while (run);
    }

    void destroy()
    {
        _inst.reset();
    }
} // namespace XD::Util::TimeMgr
