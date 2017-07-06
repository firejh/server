#include "process_id.h"

#include "process_config.h"
#include "component/format.h"



namespace base{
namespace process{

std::vector<std::string> ProcessIdentity::s_type2Name = {"none"};
std::map<std::string, ProcessType> ProcessIdentity::s_name2Type;

std::string ProcessIdentity::typeToString(ProcessType type)
{
    uint32_t index = static_cast<uint32_t>(type);
    if(index >= s_type2Name.size())
        return "none";

    return s_type2Name[index];
}

ProcessType ProcessIdentity::stringToType(const std::string& str)
{
    auto it = s_name2Type.find(str);
    if(it == s_name2Type.end())
        return INVALID_PROCESS_TYPE;

    return it->second;
}

ProcessIdentity::ProcessIdentity(const std::string& typeStr, int8_t num)
{
    m_type = stringToType(typeStr);
    m_num = num; 
}

ProcessIdentity::ProcessIdentity(ProcessIdentityValue value)
{
    m_type = (value & 0xffff0000) >> 16;
    m_num = value & 0xffff;
}

std::string ProcessIdentity::toString() const
{
    std::string ret;
    /*
    component::appendToString(&ret, m_num);
    ret.append("-");
    */
    ret.append(typeToString(m_type));
    ret.append("-");
    component::appendToString(&ret, m_num);
    return ret;
}

void ProcessIdentity::clear()
{
    m_type = 0;
    m_num = 0;
}

bool ProcessIdentity::isValid() const
{
    return typeToString(m_type) != "none" && m_num != 0;
}

ProcessIdentityValue ProcessIdentity::value() const
{
    return (ProcessIdentityValue(m_type) << 16) | m_num;
}

void ProcessIdentity::setValue(ProcessIdentityValue value)
{
    m_type   = (value & 0xffff0000) >> 16;
    m_num    = value & 0xffff;
}

ProcessType& ProcessIdentity::type() 
{
    return m_type;
}

ProcessType ProcessIdentity::type() const
{
    return m_type;
}

ProcessNum& ProcessIdentity::num()
{
    return m_num;
}

ProcessNum ProcessIdentity::num() const
{
    return m_num;
}


bool operator==(const ProcessIdentity& pid1, const ProcessIdentity& pid2)
{
    return pid1.value() == pid2.value();
}

bool operator!=(const ProcessIdentity& pid1, const ProcessIdentity& pid2)
{
    return pid1.value() != pid2.value();
}

bool operator<(const ProcessIdentity& pid1, const ProcessIdentity& pid2)
{
    return pid1.value() < pid2.value();
}


}}

