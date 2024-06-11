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

    /**
     * 重置定时器
     * @param interval  间隔时间
     * @param from_now 是否从当前时间开始
     * @return bool
     */
    bool Timer::reset(uint64_t interval, bool from_now) {
        // 如果间隔时间没有变化，并且不重新计算时间，则返回false
        if (interval == m_interval && !from_now) {
            return false;
        }
        TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
        if (!m_cb) {
            return false;
        }
        auto it = m_manager->m_timers.find(shared_from_this());
        if (it == m_manager->m_timers.end()) {
            return false;
        }
        // 删除原定时器更改新定时器
        m_interval = interval;
        m_manager->m_timers.erase(it);
        // 重新添加
        uint64_t start_time = from_now ? hh::GetCurrentMS() : m_next_time - m_interval;
        m_next_time = start_time + m_interval;
        m_manager->addTimer(shared_from_this());
        return true;
    }

    /**
     * 取消定时器
     * @return bool
     */
    bool Timer::cancel() {
        TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
        if (m_cb) {
            m_cb = nullptr;
            auto it = m_manager->m_timers.find(shared_from_this());
            if (it != m_manager->m_timers.end()) {
                m_manager->m_timers.erase(it);
                return true;
            }
        }
        return false;
    }

    /**
     * 刷新定时器
     * @return bool
     */
    bool Timer::refresh() {
        TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
        if (!m_cb) {
            return false;
        }
        auto it = m_manager->m_timers.find(shared_from_this());
        if (it == m_manager->m_timers.end()) {
            return false;
        }
        m_manager->m_timers.erase(it);
        m_next_time = hh::GetCurrentMS() + m_interval;
        m_manager->addTimer(shared_from_this());
        return true;
    }

    hh::TimerManager::TimerManager() {
         m_previouseTime = hh::GetCurrentMS();
    }

    hh::TimerManager::~TimerManager() {

    }

    /**
     * 添加定时器
     * @param interval
     * @param cb
     * @param recurring
     * @return hh::Timer::ptr 定时器指针
     */
    hh::Timer::ptr hh::TimerManager::addTimer(uint64_t interval,
                                              std::function<void()> cb,
                                              bool recurring) {
        Timer::ptr timer(new Timer(interval, cb, recurring, this));
        {
             TimerManager::RWMutexType::WriteLock lock(m_mutex);
             addTimer(timer);
        }
        // 判断是否最小需要onTimerInsertedAtFront(唤醒)
        return timer;
    }

    static void onTimer(std::weak_ptr<void> weak_cond, std::function<void()> cb) {
        std::shared_ptr<void> tmp = weak_cond.lock();
        if (tmp) {
            // 存在条件
            cb();
        }
    }

    /**
     * 添加条件定时器
     * @param interval
     * @param cb
     * @param weak_cond
     * @param recurring
     * @return hh::Timer::ptr 定时器指针
     */
    hh::Timer::ptr
    hh::TimerManager::addConditionTimer(uint64_t interval,
                                        std::function<void()> cb,
                                        std::weak_ptr<void> weak_cond,
                                        bool recurring) {
        return addTimer(interval, std::bind(onTimer, weak_cond, cb), recurring);
    }

    /**
     * 获取下一个定时器
     * @return uint64_t 毫秒级
     */
    uint64_t hh::TimerManager::getNextTimer() {
        RWMutexType::ReadLock lock(m_mutex);
        m_tickled = false;
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

    /**
     * 移除定时器
     * @param timer
     */
    void TimerManager::removeTimer(Timer::ptr &timer) {
        RWMutexType::WriteLock lock(m_mutex);
        m_timers.erase(timer);
    }

    /**
     * 获取触发定时器任务
     * @param cbs
     */
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
        //是否回拨
        bool rollover = detectClockRollover(now_ms);
        if(!rollover &&  now_ms<(*m_timers.begin())->m_next_time){
            //没有回拨 && 第一个定时器时间大于当前时间 没有过期任务
            return;
        }
        Timer::ptr now_timer(new Timer(now_ms));
        // 回拨是一定不会有超时
        auto it =rollover ?m_timers.end() : std::lower_bound(m_timers.begin(), m_timers.end(), now_timer);
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

    /**
     * 添加定时器
     */
    void TimerManager::addTimer(Timer::ptr timer) {
        bool is_first = false;
        auto it = m_timers.insert(timer).first;
        is_first = (it == m_timers.begin()) && !m_tickled;
        if (is_first) {
            m_tickled = true;
        }
        if(is_first){
            onTimerInsertedAtFront();
        }
    }
    /**
     * 检测时钟回拨
     * @param now_ms 当前时间
     */
    bool TimerManager::detectClockRollover(uint64_t now_ms) {
        bool rollover = false;
        if(m_previouseTime > now_ms
            && (m_previouseTime-60*60*1000)>now_ms){
            //认为时钟回拨了
            rollover = true;
        }
        // 更新最后一次时间
        m_previouseTime = now_ms;
        return rollover;
    }

}
