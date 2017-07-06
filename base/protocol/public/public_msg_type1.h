#ifndef PROTOCOL_PUBLIC_MSG_TYPE1_H
#define PROTOCOL_PUBLIC_MSG_TYPE1_H

namespace publicraw{

enum class MsgType1Code : uint16_t
{
    error   = 0,
    process = 1,    //进程关联通信模块
};

}
#endif
