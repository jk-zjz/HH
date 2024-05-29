//
// Created by 35148 on 2024/5/29.
//
#include "scheduler.h"

#include <utility>
#include "log.h"
namespace hh{
    static Logger::ptr g_logger = HH_LOG_NAME("system");
    Scheduler::Scheduler(uint32_t threads, bool use_caller, std::string name):
    m_name(std::move(name)){

    }

    Scheduler::~Scheduler() {

    }

    void Scheduler::start() {

    }

    void Scheduler::stop() {

    }

    Scheduler *Scheduler::GetThis() {
        return nullptr;
    }

    Fiber *Scheduler::GetFiber() {
        return nullptr;
    }
}