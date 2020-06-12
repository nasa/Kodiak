#ifndef KODIAK_CERTAINTY_HPP
#define KODIAK_CERTAINTY_HPP

#include "Value.hpp"

namespace kodiak {

    enum Certainty {
        TRUE,
        FALSE,
        POSSIBLY,
        TRUE_WITHIN_EPS,
    };

    class CertaintyClass : public Value {
    public:
        CertaintyClass(Certainty c) : certainty_(c) {}

        Certainty getCertainty() { return this->certainty_; }

        virtual void print(std::ostream &cout) const override {
            switch (certainty_) {
                case kodiak::TRUE:
                    cout << "CERTAINLY_TRUE";
                    break;
                case kodiak::FALSE:
                    cout << "CERTAINLY_FALSE";
                    break;
                case kodiak::POSSIBLY:
                    cout << "POSSIBLY_TRUE";
                    break;
                case kodiak::TRUE_WITHIN_EPS:
                    cout << "TRUE_WITHIN_EPS";
                    break;
            }
        };

        static CertaintyClass TRUE() { return CertaintyClass(kodiak::TRUE); };

        static CertaintyClass TRUE_WITHIN_EPS() { return CertaintyClass(kodiak::TRUE_WITHIN_EPS); };

        static CertaintyClass FALSE() { return CertaintyClass(kodiak::FALSE); };

        static CertaintyClass POSSIBLY() { return CertaintyClass(kodiak::POSSIBLY); };

        static int certainty2Int(Certainty c) {
            switch (c) {
                case kodiak::TRUE:
                    return 1;
                case kodiak::FALSE:
                    return 0;
                case kodiak::POSSIBLY:
                    return -1;
                case kodiak::TRUE_WITHIN_EPS:
                    return -2;
            }
        }
    private:
        const Certainty certainty_;
    };

    std::ostream &operator<<(std::ostream &out, const Certainty c);

    Certainty operator&&(Certainty const, Certainty const);
}

#endif //KODIAK_CERTAINTY_HPP
