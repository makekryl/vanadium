#pragma once

#include <string>

#include "vanadium/runtime/rt_reflect.h"

namespace vanadium::rt {
void StringifyObject(std::string& buf, const vrt_val_t& v);
}
