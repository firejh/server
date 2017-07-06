#include "rawmsg_manager.h"
#include "log/logger.h"

namespace base{
namespace rawmsg{

RawmsgManager& RawmsgManager::me()
{
    static RawmsgManager me;
    return me;
}

void RawmsgManager::timerExec(TimePoint now)
{}

void RawmsgManager::regHandler(TcpMsgCode code, RawmsgHandler handler)
{
    m_handlers[code] = handler;
}

void RawmsgManager::dealTcpMsg(const TcpMsg* recv, uint32_t recvSize, uint64_t senderId, TimePoint now)
{
    if(recvSize < sizeof(base::process::TcpMsg))
    {   
        LOG_ERROR("handle msg error, msgSize < sizeof(TcpMsg), recvSize={}, senderId={}", recvSize, senderId);
        return;
    }

    TcpMsgCode msgCode     = recv->code;
    const uint8_t* msgData = recv->data;
    uint32_t msgDataSize   = recvSize - sizeof(base::process::TcpMsg);
    uint64_t theSenderId   = senderId;

    //如果是信封需要拆开
    if(base::process::isEnvelopeMsgCode(recv->code))
    {
        auto envelope = reinterpret_cast<const base::process::Envelope*>(recv);
        if(recvSize < sizeof(base::process::Envelope))
        {   
            LOG_ERROR("handle msg error, recvSize < sizeof(Envelope), code={}", recv->code);
            return;
        }
        msgCode = envelope->msg.code;
        msgData = envelope->msg.data;
        msgDataSize  = recvSize - sizeof(base::process::Envelope);
        theSenderId  = envelope->sourceId;
    }

    auto iterToHandler = m_handlers.find(msgCode);
    if(iterToHandler == m_handlers.end())
    {   
        LOG_ERROR("handle msg error, missing rawmsg handler, code = {}", msgCode);
        return;
    }   

    iterToHandler->second(msgData, msgDataSize, theSenderId, now);
    return;
}

TcpMsgCode RawmsgManager::makeCode(uint32_t code1, uint32_t code2)
{
    return (code1 | code2);
}


}}
