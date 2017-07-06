#ifndef BASE_PROTOCOL_RAWMSG_H
#define BASE_PROTOCOL_RAWMSG_H

#include "process/tcp_message.h"
#include "component/datetime.h"


#include <memory>
#include <unordered_map>

namespace base{
namespace rawmsg{

typedef process::TcpMsgCode TcpMsgCode;
typedef process::TcpMsg TcpMsg;
typedef component::TimePoint TimePoint;

typedef std::function<void (const uint8_t*, uint32_t, uint64_t, const TimePoint&)> RawmsgHandler;

class RawmsgManager
{
public:
    //被Process::tcpPacketHandle 调用的函数
    void dealTcpMsg(const TcpMsg* recv, uint32_t recvSize, uint64_t senderId, TimePoint now); 

    //定时检测注册是否过期，暂时无用
    void timerExec(TimePoint now);
    
    //注册处理msg的handler
    void regHandler(TcpMsgCode code, RawmsgHandler handler);

    static TcpMsgCode makeCode(uint32_t code1, uint32_t code2);

private:

    std::unordered_map<TcpMsgCode, RawmsgHandler> m_handlers; 

public:
    static RawmsgManager& me();

};

}}

/*
#ifndef RAWMSG_CODE_PUBLIC
#define RAWMSG_CODE_PUBLIC(msgName) (PublicRaw::msgName)
#endif

#ifndef RAWMSG_CODE_PRIVATE
#define RAWMSG_CODE_PRIVATE(msgName) (PrivateRaw::msgName)
#endif
*/


#define REG_RAWMSG_PUBLIC(raw, handler) \
base::rawmsg::RawmsgManager::me().regHandler(raw, handler);

#define REG_RAWMSG_PRIVATE(raw, handler) \
base::rawmsg::RawmsgManager::me().regHandler(raw, handler);

#define MAKE_CODE(code1, code2) base::rawmsg::RawmsgManager::makeCode((uint32_t)code1,(uint32_t)code2)

#endif
