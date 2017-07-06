#include "connection_checker.h"

namespace base{
namespace process{

bool ConnectionChecker::ConnectionChecker::exec()
{
    checkConn();
    return true;
}


}}
