#ifndef KODIAK_BOOLEANCONSTANTNODE_HPP
#define KODIAK_BOOLEANCONSTANTNODE_HPP

#include "Expressions/Boolean/Node.hpp"

namespace kodiak {
    namespace BooleanExpressions {

        class ConstantNode : public Node {
        public:
            ConstantNode(const Certainty);

            Certainty getValue() const;

        private:

            virtual Certainty doEvaluate(const Environment &, const bool, const real) const override;

            virtual void doPrint(std::ostream & = std::cout) const override;

            virtual bool equals(const Node &) const override;

            virtual unique_ptr<Node> doClone() const override;

            const Certainty value;
        };
    }
}

#endif
