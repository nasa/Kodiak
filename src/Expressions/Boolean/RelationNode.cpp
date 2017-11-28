#include "RelationNode.hpp"

namespace kodiak {
    namespace BooleanExpressions {

        Certainty RelationNode::doEvaluate(const Environment &env, const bool i, const real eps) const {
            Box anotherBox{env.box};
            NamedBox anotherNamedBox{env.namedBox};
            int result = this->delegate_->eval(anotherBox, anotherNamedBox, i, eps);
            switch (result) {
                case -2:
                    return TRUE_WITHIN_EPS;
                case -1:
                    return POSSIBLY;
                case 0:
                    return FALSE;
                default:
                    return TRUE;
            }
        }

        void RelationNode::doPrint(std::ostream &ostream) const {
            this->delegate_->print(ostream);
        }

        bool RelationNode::equals(const Node &other) const {
            if (typeid(*this) != typeid(other))
                return false;

            std::ostringstream thisString;
            std::ostringstream otherString;
            this->print(thisString);
            other.print(otherString);
            return thisString.str() == otherString.str();
        }

        std::unique_ptr<Node> RelationNode::doClone() const {
            return std::make_unique<RelationNode>(
                    this->operand_,
                    val(0),
                    this->relation_
            );
        }
    }
}
