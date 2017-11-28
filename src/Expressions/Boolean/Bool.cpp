#include "Expressions/Boolean/Bool.hpp"

namespace kodiak {
    namespace BooleanExpressions {

        extern const Bool True{ConstantNode(TRUE)};
        extern const Bool False{ConstantNode(FALSE)};
        extern const Bool Possibly{ConstantNode(POSSIBLY)};
        extern const Bool EPSTrue{ConstantNode(TRUE_WITHIN_EPS)};

        Bool Cnst(const Certainty c) {
            return Bool(std::make_unique<ConstantNode>(c));
        }

        Bool operator!(const Bool &arg) {
            return Bool(std::make_unique<NotNode>(arg.getNode()));
        }

        Bool operator&&(Bool const &lhs, Bool const &rhs) {
            return Bool(std::make_unique<AndNode>(lhs.getNode(), rhs.getNode()));
        }

        Bool operator||(Bool const &lhs, Bool const &rhs) {
            return Bool(std::make_unique<OrNode>(lhs.getNode(), rhs.getNode()));
        }

        Bool operator|=(Bool const &lhs, Bool const &rhs) {
            return Bool(std::make_unique<ImplicationNode>(lhs.getNode(), rhs.getNode()));
        }

        Bool operator<(Real const &lhs, Real const &rhs) {
            return Bool(std::make_unique<RelationNode>(lhs, rhs, LT));
        }

        Bool operator<=(Real const &lhs, Real const &rhs) {
            return Bool(std::make_unique<RelationNode>(lhs, rhs, LE));
        }

        Bool operator>(Real const &lhs, Real const &rhs) {
            return Bool(std::make_unique<RelationNode>(lhs, rhs, GT));
        }

        Bool operator>=(Real const &lhs, Real const &rhs) {
            return Bool(std::make_unique<RelationNode>(lhs, rhs, GE));
        }

        Bool operator==(Real const &lhs, Real const &rhs) {
            return Bool(std::make_unique<RelationNode>(lhs, rhs, EQ));
        }

        std::ostream &operator<<(std::ostream &os, const Bool &expr) {
            expr.print(os);
            return os;
        }
    }
}

