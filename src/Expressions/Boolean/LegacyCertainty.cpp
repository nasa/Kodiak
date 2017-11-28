//
// Created by Marco Antonio Feliú Gabaldón on 11/04/2017.
//

#include "Expressions/Boolean/LegacyCertainty.hpp"
#include <string>

using namespace kodiak;
using std::string;
using std::literals::string_literals::operator""s;

LegacyCertainty kodiak::weakest(LegacyCertainty l, LegacyCertainty r) {
    if (l == CERTAINLY_FALSE or r == CERTAINLY_FALSE)
        return CERTAINLY_FALSE;
    if (l == POSSIBLY_TRUE or r == POSSIBLY_TRUE)
        return POSSIBLY_TRUE;
    if (l == POSSIBLY_TRUE_WITHIN_EPS or r == POSSIBLY_TRUE_WITHIN_EPS) {
        return POSSIBLY_TRUE_WITHIN_EPS;
    }
    return CERTAINLY_TRUE;
}

std::ostream& kodiak::operator<<(std::ostream& cout, const LegacyCertainty c){
    const string CERTAINLY_TRUE_STRING           = "Certainly True"s;;
    const string CERTAINLY_FALSE_STRING          = "Certainly False"s;;
    const string POSSIBLY_TRUE_STRING            = "Possibly True"s;;
    const string POSSIBLY_TRUE_WITHIN_EPS_STRING = "Possibly True within EPS"s;;
    const string ERROR_STRING = "ERROR: Unknown Certainty"s;;
    switch (c) {
        case CERTAINLY_TRUE :
            return cout << CERTAINLY_TRUE_STRING;
        case CERTAINLY_FALSE :
            return cout << CERTAINLY_FALSE_STRING;
        case POSSIBLY_TRUE :
            return cout << POSSIBLY_TRUE_STRING;
        case POSSIBLY_TRUE_WITHIN_EPS:
            return cout << POSSIBLY_TRUE_WITHIN_EPS_STRING;
        default:
            return cout << ERROR_STRING << " "<< c;
    }
}

LegacyCertainty CertaintyMapper::fromInt(int i) {
    switch (i) {
        case -2:
            return POSSIBLY_TRUE_WITHIN_EPS;
        case -1:
            return POSSIBLY_TRUE;
        case 0:
            return CERTAINLY_FALSE;
        default:
            return CERTAINLY_TRUE;
    }
}

int CertaintyMapper::toInt(LegacyCertainty i) {
    switch (i) {
        case POSSIBLY_TRUE_WITHIN_EPS:
            return -2;
        case POSSIBLY_TRUE:
            return -1;
        case CERTAINLY_FALSE:
            return 0;
        case CERTAINLY_TRUE:
            return 1;
    }
    return -1000000;
}