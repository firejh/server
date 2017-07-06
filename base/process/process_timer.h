/*
 * Author: JiangHeng
 *
 * Created: 2017-01-09 17:48 +0800
 *
 * Modified: 2017-01-09 17:48 +0800
 *
 * Description: 
 */

#ifndef WATER_PROCESS_PROCESSTIMER_H
#define WATER_PROCESS_PROCESSTIMER_H

#include "component/datetime.h"
#include "component/event.h"
#include "component/spinlock.h"
#include "component/timer.h"

#include "process_thread.h"

#include <functional>

namespace base{
namespace process{

class ProcessTimer : public ProcessThread
{
public:
    using EventHandler = std::function<void (const component::TimePoint&)>;

    ProcessTimer();
    ~ProcessTimer() = default;

    bool exec() override;

    bool isSuspend() const;
    void suspend();
    void resume();

    int64_t precision() const;

    //注册一个触发间隔
    void regEventHandler(std::chrono::milliseconds interval, const EventHandler& handle);
public:
    component::Event<void (ProcessTimer*)> e_stop;

private:
    component::Timer m_timer;
    volatile bool m_suspend; //只有进程的主线程会修改这个值，就不用atomic了
};

}}

#endif
