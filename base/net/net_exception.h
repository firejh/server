/*
 * Author: JiangHeng
 *
 * Created: 2016-11-01 14:59 +0800
 *
 * Modified: 2016-11-01 14:59 +0800
 *
 * Description: 
 */
#ifndef WATER_NET_EXCEPTION_H
#define WATER_NET_EXCEPTION_H

#include "../component/exception.h"

namespace base{
namespace net{

    DEFINE_EXCEPTION(NetException, component::ExceptionBase)

}}
#endif
