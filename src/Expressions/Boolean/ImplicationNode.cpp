#include "ImplicationNode.hpp"

using namespace kodiak;
using namespace kodiak::BooleanExpressions;

Certainty ImplicationNode::doEvaluate(const Environment &environment, const bool useBernstein, const real eps) const {
    Certainty leftCertainty  = this->lhs->eval(environment, useBernstein, eps);
    Certainty rightCertainty = POSSIBLY;
    switch (leftCertainty) {
        case FALSE:
            return TRUE;
        case TRUE:
            rightCertainty = this->rhs->eval(environment, useBernstein, eps);
            return rightCertainty;
        case TRUE_WITHIN_EPS:
        case POSSIBLY:
            rightCertainty = this->rhs->eval(environment, useBernstein, eps);
            if (rightCertainty == TRUE)
                return TRUE;
            else if (rightCertainty == TRUE_WITHIN_EPS)
                return TRUE_WITHIN_EPS;
            else
                return POSSIBLY;
        default:
            throw Growl("Kodiak (Implication::eval): case not implemented");
    }
}

void ImplicationNode::doPrint(std::ostream &cout) const {
    this->lhs->print(cout);
    cout << " => ";
    this->rhs->print(cout);
}

bool ImplicationNode::equals(const Node &other) const {
    if (typeid(*this) != typeid(other))
        return false;

    const ImplicationNode &otherCasted = dynamic_cast<ImplicationNode const &>(other);
    return *this->lhs == *otherCasted.lhs && *this->rhs == *otherCasted.rhs;
}

unique_ptr<kodiak::BooleanExpressions::Node> ImplicationNode::doClone() const {
    return std::make_unique<ImplicationNode>(*this->lhs->clone(), *this->rhs->clone());
}

namespace kodiak {
    namespace BooleanExpressions {

        std::unique_ptr<kodiak::BooleanExpressions::Node>
        operator|=(std::unique_ptr<Node> const &lhs, std::unique_ptr<Node> const &rhs) {
            return std::make_unique<ImplicationNode>(*lhs, *rhs);
        }

        std::unique_ptr<kodiak::BooleanExpressions::Node>
        operator|=(Node const &lhs, Node const &rhs) {
            return std::make_unique<ImplicationNode>(lhs, rhs);
        }
    }
}
