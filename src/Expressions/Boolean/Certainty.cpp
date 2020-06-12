#include "Certainty.hpp"

using namespace kodiak;

std::ostream &kodiak::operator<<(std::ostream &out, const Certainty c) {

    CertaintyClass cc{c};
    cc.print(out);
    return out;
}

Certainty kodiak::operator&&(Certainty const lhs, Certainty const rhs) {
    if (lhs == FALSE || rhs == FALSE) {
        return FALSE;
    } else if (lhs == POSSIBLY || rhs == POSSIBLY) {
        return POSSIBLY;
    } else if (lhs == TRUE_WITHIN_EPS || rhs == TRUE_WITHIN_EPS) {
        return TRUE_WITHIN_EPS;
    } else {
        return TRUE;
    }
}
