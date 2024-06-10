//
// Created by 35148 on 2024/6/10.
//

#ifndef HH_TIMER_H
#define HH_TIMER_H

#include "thread.h"
#include <memory>
#include <set>

namespace hh {
    class TimerManager;

    class Timer : public std::enable_shared_from_this<Timer> {
        friend class TimerManager;
    public:
        typedef std::shared_ptr<Timer> ptr;
        Timer(uint64_t interval, bool repeat, std::function<void()> cb,
              bool recurring, TimerManager *manager);
    private:
        uint64_t m_next_time = 0;   // 下一次触发时间
        uint64_t m_interval = 0;    // 触发间隔
        bool m_repeat = 0;          // 是否重复
    private:
        // 比较函数
        struct Compare {
            bool operator()(const Timer::ptr &lhs, const Timer::ptr &rhs) const {
                return lhs->m_next_time < rhs->m_next_time;
            }
        };

        class TimerManager {
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
            bool getRecentTimer();
        private:
            RWMutexType m_mutex;
            // 定时器集合
            std::set<Timer::ptr, Timer::Compare> m_timers;
        };
    };
}
#endif //HH_TIMER_H
