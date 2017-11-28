#ifndef KODIAK_NOTNODE_HPP
#define KODIAK_NOTNODE_HPP

#include "Expressions/Boolean/Node.hpp"

namespace kodiak {
    namespace BooleanExpressions {

        class NotNode : public Node {
        public:
            NotNode(const Node &expr) : operand_(expr.clone()) {}

            NotNode(const std::unique_ptr<Node> &expr) : operand_(expr->clone()) {}

            NotNode(std::unique_ptr<Node> &&expr) : operand_(std::move(expr)) {}

        private:
            virtual Certainty doEvaluate(const Environment &, const bool, const real) const override;

            virtual void doPrint(std::ostream & = std::cout) const override;

            virtual bool equals(const Node &) const override;

            virtual unique_ptr<Node> doClone() const override;

            const std::unique_ptr<Node> operand_;
        };

        std::unique_ptr<Node> operator!(const std::unique_ptr<Node>&);
        std::unique_ptr<Node> operator!(const Node &);
    }
}

#endif
