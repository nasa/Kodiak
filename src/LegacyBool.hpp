#ifndef KODIAK_LEGACYBOOL_HPP
#define KODIAK_LEGACYBOOL_HPP

#include "Real.hpp"

namespace kodiak {

    class RelExpr {
    public:
        RelExpr(const Real &, const RelType, const bool = false,
                const Substitution & = EmptySubstitution, const nat nvars = 0);

        Real ope() const {
            return ope_;
        }

        RelType op() const {
            return op_;
        }

        int eval(Box &box, const bool bp = false, const real eps = 0) {
            NamedBox constbox;
            return eval(box, constbox, bp, eps);
        }

        int eval(Box &, NamedBox &, const bool = false, const real = 0);

        Real derivativeForVariable(const nat) const;

        void print(std::ostream &) const;

    private:
        Real ope_;
        RelType op_;
        Realn partialDerivativesPerVariable_;
    };

    std::ostream &operator<<(std::ostream &, const kodiak::RelExpr &);

    std::ostream &operator<<(std::ostream &, const kodiak::RelType);

}

#endif //KODIAK_LEGACYBOOL_HPP
