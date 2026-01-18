#pragma once

#include <string_view>
#include <utility>
#include <vector>

using asn1p_wsyntx_t = struct asn1p_wsyntx_s;

namespace vanadium::asn1::ast {

// TODO: emit errors

using ClassObjectRow = std::pair<std::string_view, std::string_view>;
using ClassObjectList = std::vector<ClassObjectRow>;

ClassObjectList ParseClassObject(std::string_view buf, const asn1p_wsyntx_t* syntax);

}  // namespace vanadium::asn1::ast
