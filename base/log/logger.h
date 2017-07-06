/*
 * Author: JiangHeng
 *
 * Created: 2017-01-07 16:43 +0800
 *
 * Modified: 2017-01-07 16:43 +0800
 *
 * Description: 日志接口
 */
#ifndef BASE_LOG_LOGGER_H
#define BASE_LOG_LOGGER_H

#include "file_writer_manager.h"
#include "component/format.h"

#include <map>

enum class LogLevel : uint8_t
{
    LL_DEBUG,//调试用
    LL_TRACE,//一些行为记录
    LL_ERROR,//错误
    LL_MAX
};

class Logger
{
public:
    //初始化日志文件，主程序先调用
    static void addLog(const std::string& filename)
    {
        base::log::FileWriterManager::me().addLogFile(filename);
    }
    //日志开启
    static void startLog()
    {
        base::log::FileWriterManager::me().start();
    }
    //日志结束
    static void stopLog()
    {
        base::log::FileWriterManager::me().stop();
    }

    //通用写日志
    template<typename... Args>
    static void log(const std::string& filename, LogLevel level, const std::string& formatStr, const Args&... args)
    {
        std::string fullFormatStr = formatTime() + " " + getLevelStr(level) + ": " + formatStr + "\n";
        const std::string text = base::component::format(fullFormatStr, args...);
        base::log::FileWriterManager::me().append(filename, text.data(), text.size());
    }


    //通用定好等级的日志
    template<typename... Args>
    static void trace(const std::string& filename, const Args&... args)
    {
        log(filename, LogLevel::LL_TRACE, std::forward<const Args>(args)...);
    }
    template<typename... Args>
    static void debug(const std::string& filename, const Args&... args)
    {
        log(filename, LogLevel::LL_DEBUG, std::forward<const Args>(args)...);
    }
    template<typename... Args>
    static void error(const std::string& filename, const Args&... args)
    {
        log(filename, LogLevel::LL_ERROR, std::forward<const Args>(args)...);
    }

    //添加所有日志文件
    static void addAllLog(const std::string& filename);

public:
    //基础库日志名称
    static std::string m_baseFilename;
    static std::string m_diamondFilename;
    //...


private:
    static std::string formatTime();
    static const char* getLevelStr(LogLevel l);
};

#define BASE_LOG "_base"

//使用者可以也应该定义一些特定模块的日志，使格式更统一

//库通用，库日志的格式太多。不规定格式，使用者自定义即可
#define LOG_DEBUG(args...) Logger::debug(Logger::m_baseFilename, args); 
#define LOG_TRACE(args...) Logger::trace(Logger::m_baseFilename, args); 
#define LOG_ERROR(args...) Logger::error(Logger::m_baseFilename, args); 

//逻辑debug日志，这里需要记录的格式也较多，用户自己定义，格式混乱没关系，都是自己调试使用，这样有助于调试
#define LOG_DEBUG_DIAMOND(args...) Logger::debug(Logger::m_diamondFilename, args);
#define LOG_ERROR_DIAMOND(args...) Logger::error(Logger::m_diamondFilename, args);

//登录记录日志，需要有特定格式，记录数据尽量少

//金币记录日志，要有特定格式，记录数据尽量少


#endif
