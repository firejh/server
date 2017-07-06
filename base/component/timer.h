/*
 * Author: JiangHeng
 *
 * Created: 2016-10-31 18:16 +0800
 *
 * Modified: 2016-10-31 18:16 +0800
 *
 * Description: 
 */
#include "datetime.h"
#include "event.h"
#include "spinlock.h"

#include <map>
#include <set>
#include <mutex>
#include <functional>

namespace base{
namespace component{

class Timer
{
    //typedef std::chrono::high_resolution_clock TheClock;
    typedef Clock TheClock;
public:
    typedef std::pair<TheClock::duration, Event<void (const TimePoint&)>::RegID> RegID;

    Timer();
    ~Timer() = default;

    void tick();

    int64_t precision() const;

    //注册一个触发间隔
    RegID regEventHandler(std::chrono::milliseconds interval,
                         const std::function<void (const TimePoint&)>& handle);
    void unregEventHandler(RegID);
public:
    Event<void (Timer*)> e_stop;

private:
    struct EventHandlers
    {
        Event<void (const TimePoint&)> event;
        TheClock::time_point lastEmitTime;
    };

    Spinlock m_lock;
    std::map<TheClock::duration, EventHandlers> m_eventHandlers;

    std::set<RegID> m_unregedId;
};

}}
