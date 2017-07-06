/*
 * Author: JiangHeng
 *
 * Created: 2016-10-31 18:15 +0800
 *
 * Modified: 2016-10-31 18:15 +0800
 *
 * Description: 代码块退出时自动执行一些代码的机制，可理解为java的finally block
 */
#ifndef WATER_COMPONET_SCOPE_GUARD_HPP
#define WATER_COMPONET_SCOPE_GUARD_HPP

#include <functional>

namespace base{
namespace component{

class ScopeGuard final
{
public:
    explicit ScopeGuard(std::function<void()> onExitScope_);
    ~ScopeGuard();
    void dismiss();

private:
    std::function<void()> onExitScope;
    bool dismissed;

private: // noncopyable
    ScopeGuard(ScopeGuard const&) = delete;
    ScopeGuard& operator = (ScopeGuard const&) = delete;
};

}}

#define CODE_CON(code1, code2) code1##code2
#define MACRO_CON(code1, code2) CODE_CON(code1, code2)


#define ON_EXIT_SCOPE(callback) base::component::ScopeGuard MACRO_CON(onExit, __LINE__)(callback)
#define ON_EXIT_SCOPE_DO(statements) base::component::ScopeGuard MACRO_CON(onExit, __LINE__)([&]{statements;})

#endif 
