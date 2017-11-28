#ifndef _BOOL_
#define _BOOL_

#include "types.hpp"
#include "Environment.hpp"
#include "Expressions/Boolean/Certainty.hpp"
#include "Expressions/Boolean/Node.hpp"
#include "Expressions/Boolean/ConstantNode.hpp"
#include "Expressions/Boolean/NotNode.hpp"
#include "Expressions/Boolean/AndNode.hpp"
#include "Expressions/Boolean/OrNode.hpp"
#include "Expressions/Boolean/ImplicationNode.hpp"
#include "Expressions/Boolean/RelationNode.hpp"

namespace kodiak {
    namespace BooleanExpressions {

        class Bool {
        public:
            Bool(const unique_ptr<Node> &aNode) : node_(aNode->clone()) {}

            Bool(unique_ptr<Node> &&aNode) : node_(std::move(aNode)) {}

            Bool(const Node &aNode) : node_(aNode.clone()) {}

            Bool(const Node &&aNode) : node_(aNode.clone()) {}

            Bool(const Bool &src) : node_(src.node_->clone()) {}

            Certainty eval(const Environment &env, const bool bernsteinEnclosure, const real eps) const {
                return this->node_->eval(env, bernsteinEnclosure, eps);
            }

            void print(std::ostream &os) const {
                this->node_->print(os);
            }

            bool operator==(const Bool &another) const {
                return *this->node_ == *another.node_;
            }

            Node const &getNode() const {
                return *this->node_;
            }

        private:
            unique_ptr<Node> node_;
        };

        extern Bool const True;
        extern Bool const False;
        extern Bool const Possibly;
        extern Bool const EPSTrue;

        Bool Cnst(Certainty const);

        Bool operator!(Bool const &);

        Bool operator&&(Bool const &, Bool const &);

        Bool operator||(Bool const &, Bool const &);

        Bool operator|=(Bool const &, Bool const &);

        Bool operator<(Real const &, Real const &);

        Bool operator<=(Real const &, Real const &);

        Bool operator>(Real const &, Real const &);

        Bool operator>=(Real const &, Real const &);

        Bool operator==(Real const &, Real const &);

        std::ostream &operator<<(std::ostream &os, const Bool &expr);
    }
}

#endif
