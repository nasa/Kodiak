#include "Certainty.hpp"

using namespace kodiak;

std::ostream &kodiak::operator<<(std::ostream &out, const Certainty c) {

    CertaintyClass cc{c};
    cc.print(out);
    return out;
}
