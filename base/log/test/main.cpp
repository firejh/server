#include "../logger.h"

#include <iostream>
#include "stdlib.h"
#include "unistd.h"

int main()
{
    Logger::addLog("./log/test1");
    Logger::addLog("./log/test2");
    Logger::addLog("./log/test3");
    Logger::startLog();
    std::cout << "@@@@@@@@1" << std::endl;
    Logger::log("./log/test1", LogLevel::LL_DEBUG, "测试一下{}", 123);
    std::cout << "@@@@@@@@2" << std::endl;
    Logger::log("./log/test1", LogLevel::LL_DEBUG, "测试一下{}", 123);
    Logger::log("./log/test2", LogLevel::LL_DEBUG, "测试一下{}", 123);
    Logger::log("./log/test2", LogLevel::LL_DEBUG, "测试一下{}", 123);
    Logger::log("./log/test3", LogLevel::LL_DEBUG, "测试一下{}", 123);
    Logger::log("./log/test3", LogLevel::LL_DEBUG, "测试一下{}", 123);
    Logger::log("./log/test3", LogLevel::LL_DEBUG, "测试一下{}", 123);
    std::cout << "@@@@@@@@" << std::endl;


    sleep(3);
    std::cout << "@@@@@@@@3" << std::endl;
    Logger::stopLog();
    std::cout << "@@@@@@@@" << std::endl;

    std::cout << "stop  " << std::endl;
}
