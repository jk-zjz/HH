//
// Created by 35148 on 2024/6/10.
//
#include "timer.h"
#include <algorithm>

namespace hh {
    bool hh::Timer::Compare::operator()(const hh::Timer::ptr &lhs, const hh::Timer::ptr &rhs) {
        // 优先级队列，按照时间升序排列
        if (lhs->m_next_time < rhs->m_next_time) {
            return true;
        }
        // 如果时间相同，则比较指针
        if (lhs->m_next_time == rhs->m_next_time && lhs.get() < rhs.get()) {
            return true;
        }
        return false;
    }

    hh::Timer::Timer(uint64_t interval, bool repeat,
                     std::function<void()> cb, bool recurring,
                     hh::TimerManager *manager) :
            m_interval(interval),
            m_repeat(repeat),
            m_cb(cb),
            m_manager(manager) {
        // 获取当前时间 毫秒级
        m_next_time = hh::GetCurrentMS() + interval;
    }

    hh::TimerManager::TimerManager() {

    }

    hh::TimerManager::~TimerManager() {

    }

    hh::Timer::ptr hh::TimerManager::addTimer(uint64_t interval,
                                              std::function<void()> cb,
                                              bool recurring) {
        Timer::ptr timer(new Timer(interval, cb, recurring, this));
        bool need_tickle = false;
        {
            // 插入set容器中
            RWMutexType::WriteLock lock(m_mutex);
            auto it = m_timers.insert(timer).first;
            need_tickle = (it == m_timers.begin());
        }
        // 判断是否最小需要onTimerInsertedAtFront(唤醒)
        if (need_tickle) {
            onTimerInsertedAtFront();
        }
        return timer;
    }

    static void onTimer(std::weak_ptr<void> weak_cond, std::function<void()> cb) {
        std::shared_ptr<void> tmp = weak_cond.lock();
        if (tmp) {
            // 存在条件
            cb();
        }
    }

    hh::Timer::ptr
    hh::TimerManager::addConditionTimer(uint64_t interval,
                                        std::function<void()> cb,
                                        std::weak_ptr<void> weak_cond,
                                        bool recurring) {
        return addTimer(interval, std::bind(onTimer, weak_cond, cb), recurring);
    }

    uint64_t hh::TimerManager::getNextTimer() {
        RWMutexType::ReadLock lock(m_mutex);
        if (m_timers.empty()) {
            return ~0ull;
        }
        // 取出最小的timer
        auto it = *m_timers.begin();
        if (it->m_next_time > hh::GetCurrentMS()) {
            return it->m_next_time - hh::GetCurrentMS();
        }
        return 0;
    }

    void TimerManager::removeTimer(Timer::ptr &timer) {
        RWMutexType::WriteLock lock(m_mutex);
        m_timers.erase(timer);
    }

    void TimerManager::listExpiredCb(std::vector<std::function<void()>> &cbs) {
        uint64_t now_ms = hh::GetCurrentMS();
        std::vector<Timer::ptr> tmp;
        {
            RWMutexType::ReadLock lock(m_mutex);
            if (m_timers.empty()) {
                return;
            }
        }
        RWMutexType::WriteLock lock(m_mutex);
        Timer::ptr now_timer(new Timer(now_ms));
        auto it = std::lower_bound(m_timers.begin(), m_timers.end(), now_timer);
        while (it != m_timers.end() && (*it)->m_next_time < now_ms) {
            it++;
        }
        tmp.insert(tmp.begin(), m_timers.begin(), it);
        m_timers.erase(m_timers.begin(), it);
        cbs.resize(tmp.size());
        for (size_t i = 0; i < tmp.size(); i++) {
            cbs[i] = tmp[i]->m_cb;
            if (tmp[i]->m_repeat) {
                tmp[i]->m_next_time = now_ms + tmp[i]->m_interval;
                m_timers.insert(tmp[i]);
            } else {
                tmp[i]->m_cb = nullptr;
            }
        }

    }

}

