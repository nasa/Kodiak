#ifndef KODIAK_RELATIONNODE_HPP
#define KODIAK_RELATIONNODE_HPP

#include "LegacyBool.hpp"
#include "Expressions/Boolean/Node.hpp"

namespace kodiak {
    namespace BooleanExpressions {

        class RelationNode : public Node {
        public:
            RelationNode(const Real &left, const Real &right, const RelType relation) :
                    relation_(relation),
                    operand_(left - right),
                    delegate_(std::make_unique<RelExpr>(operand_, relation_)) {}

        private:
            virtual Certainty doEvaluate(const Environment &, const bool, const real) const override;

            virtual void doPrint(std::ostream & = std::cout) const override;

            virtual bool equals(const Node &) const override;

            virtual std::unique_ptr<Node> doClone() const override;

            const RelType relation_;
            const Real operand_;
            const std::unique_ptr<RelExpr> delegate_;
        };
    }
}

#endif
