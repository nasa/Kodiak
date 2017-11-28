#ifndef KODIAK_ANDNODE_HPP
#define KODIAK_ANDNODE_HPP

#include "Real.hpp"
#include "Expressions/Boolean/Node.hpp"

namespace kodiak {
    namespace BooleanExpressions {

        class AndNode : public Node {
        public:
            AndNode(const Node &lhs, const Node &rhs) : lhs(lhs.clone()), rhs(rhs.clone()) {}

            virtual std::unique_ptr<Node> getLHS() const { return this->lhs->clone(); }

            virtual std::unique_ptr<Node> getRHS() const { return this->rhs->clone(); }

        private:
            virtual Certainty doEvaluate(const Environment &, const bool, const real) const override;

            virtual void doPrint(std::ostream & = std::cout) const override;

            virtual bool equals(const Node &) const override;

            virtual unique_ptr<Node> doClone() const override;

            const std::unique_ptr<Node> lhs;
            const std::unique_ptr<Node> rhs;
        };

        std::unique_ptr<Node> operator&&(std::unique_ptr<Node> const &lhs, std::unique_ptr<Node> const &rhs);
        std::unique_ptr<Node> operator&&(Node const &lhs, Node const &rhs);
    }
}

#endif
