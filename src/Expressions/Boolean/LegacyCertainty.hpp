//
// Created by Marco Antonio Feliú Gabaldón on 11/04/2017.
//

#ifndef KODIAK_LEGACYCERTAINTY_HPP
#define KODIAK_LEGACYCERTAINTY_HPP

#include <ostream>

namespace kodiak {

    enum LegacyCertainty {
        CERTAINLY_TRUE,
        CERTAINLY_FALSE,
        POSSIBLY_TRUE,
        POSSIBLY_TRUE_WITHIN_EPS
    };

    std::ostream& operator<<(std::ostream&, const LegacyCertainty);

    class CertaintyMapper {
    public:
        static LegacyCertainty fromInt(int);

        static int toInt(LegacyCertainty);
    };

    LegacyCertainty weakest(LegacyCertainty, LegacyCertainty);
}

#endif //KODIAK_LEGACYCERTAINTY_HPP
