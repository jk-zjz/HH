//
// Created by 35148 on 2024/6/10.
//

#ifndef HH_TIMER_H
#define HH_TIMER_H


#include <memory>
#include <set>
#include <functional>
#include "thread.h"
namespace hh {
    class TimerManager;

    class Timer : public std::enable_shared_from_this<Timer> {

        friend class TimerManager;

    public:
        typedef std::shared_ptr<Timer> ptr;

        Timer(uint64_t interval, std::function<void()> cb,
              bool recurring, TimerManager *manager);
        Timer(uint64_t Max):m_next_time(Max){};
        //重置
        bool reset(uint64_t interval, bool from_now);
        //取消
        bool cancel();
        //刷新
        bool refresh();
    private:
        // 毫秒级
        uint64_t m_next_time = 0;   // 下一次触发时间
        uint64_t m_interval = 0;    // 触发间隔
        bool m_repeat = 0;          // 是否重复
        std::function<void()> m_cb; // 回调函数
        TimerManager *m_manager = nullptr;  // 管理器
    private:
        // 比较函数
        struct Compare {
            bool operator()(const std::shared_ptr<Timer>& lhs, const std::shared_ptr<Timer>& rhs) const;
        };

    };

    class TimerManager {
        friend class Timer;
    public:
        typedef RWMutex RWMutexType;

        TimerManager();

        virtual ~TimerManager();

        // 添加定时器
        Timer::ptr addTimer(uint64_t interval, std::function<void()> cb, bool recurring = false);

        // 条件定时器
        Timer::ptr addConditionTimer(uint64_t interval,
                                     std::function<void()> cb,
                                     std::weak_ptr<void> weak_cond,
                                     bool recurring = false);
        // 获取最小定时器的执行时间
        uint64_t getNextTimer();
        // 获取过期的定时器任务
        void listExpiredCb(std::vector<std::function<void()>> &cbs);
        bool hasTimer();
    protected:
        // 添加定时器 并且 为最小 唤醒
        virtual void onTimerInsertedAtFront() = 0;
        void addTimer(Timer::ptr timer,RWMutexType::WriteLock &lock);
    private:
        bool detectClockRollover(uint64_t now_ms);
    private:
        RWMutexType m_mutex;
        // 定时器集合
        std::set<Timer::ptr, Timer::Compare> m_timers;  // 定时器集合
        bool m_tickled = false;                         // 是否被唤醒
        uint64_t m_previouseTime = 0;                   // 上一次触发时间
    };
}
#endif //HH_TIMER_H
