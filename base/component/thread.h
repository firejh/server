/*
 * Author: JiangHeng
 *
 * Created: 2016-10-31 18:16 +0800
 *
 * Modified: 2016-10-31 18:16 +0800
 *
 * Description: 
 */
#ifndef WATER_COMPONET_THREAD_H
#define WATER_COMPONET_THREAD_H

namespace base{
namespace component{

template <typename ThreadFunRet, typename... ThreadFunArgs>
class Thread;

template <typename ThreadFunRet, typename... ThreadFunArgs>
class Thread<ThreadFunRet (ThreadFunArgs...)>
{
public:
    Thread();
    Thread(const std::functional<ThreadFunRet (ThreadFunArgs...)>&& fun);

private:
};

}}


#endif
