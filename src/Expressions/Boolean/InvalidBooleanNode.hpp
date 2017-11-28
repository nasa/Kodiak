#ifndef KODIAK_INVALIDBOOLEANNODE_HPP
#define KODIAK_INVALIDBOOLEANNODE_HPP

#include "Expressions/Boolean/Node.hpp"

namespace kodiak {
    namespace BooleanExpressions {

        class InvalidBooleanNode : public Node {
        public:
        private:
            virtual Certainty doEvaluate(const Environment &, const bool, const real) const override;

            virtual void doPrint(std::ostream & = std::cout) const override;

            virtual bool equals(const Node &) const override;

            virtual unique_ptr<Node> doClone() const override;
        };
    }
}

#endif
