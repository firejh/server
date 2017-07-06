/*
 * Author: JiangHeng
 *
 * Created: 2016-10-31 18:15 +0800
 *
 * Modified: 2016-10-31 18:15 +0800
 *
 * Description: 
 */
#ifndef WATER_COMPONET_SPINLOCK_HPP
#define WATER_COMPONET_SPINLOCK_HPP

#include "class_helper.h"
#include <atomic>

namespace base{
namespace component{

class Spinlock
{
    NON_COPYABLE(Spinlock);
public:
    Spinlock();
    ~Spinlock() = default;

    void lock();
    void unlock();
    bool try_lock();

    bool locked();

private:
    std::atomic<uint32_t> m_flag;
};


}}


#endif
