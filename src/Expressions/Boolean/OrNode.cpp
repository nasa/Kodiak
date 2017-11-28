#include "OrNode.hpp"

using namespace kodiak;
using namespace kodiak::BooleanExpressions;

Certainty OrNode::doEvaluate(const Environment &environment, const bool useBernstein, const real eps) const {
    Certainty leftCertainty  = this->lhs->eval(environment, useBernstein, eps);
    Certainty rightCertainty = POSSIBLY;
    switch (leftCertainty) {
        case TRUE:
            return TRUE;
        case FALSE:
            rightCertainty = this->rhs->eval(environment, useBernstein, eps);
            return rightCertainty;
        case POSSIBLY:
            rightCertainty = this->rhs->eval(environment, useBernstein, eps);
            if (rightCertainty == TRUE)
                return TRUE;
            else if (rightCertainty == TRUE_WITHIN_EPS)
                return TRUE_WITHIN_EPS;
            else
                return POSSIBLY;
        case TRUE_WITHIN_EPS:
            rightCertainty = this->rhs->eval(environment, useBernstein, eps);
            if (rightCertainty == TRUE)
                return TRUE;
            else
                return TRUE_WITHIN_EPS;
        default:
            throw Growl("Kodiak (OrOperator::eval): case not implemented");
    }
}

void OrNode::doPrint(std::ostream &cout) const {
    this->lhs->print(cout);
    cout << " || ";
    this->rhs->print(cout);
}

bool OrNode::equals(const Node &other) const {
    if (typeid(*this) != typeid(other))
        return false;

    const OrNode &otherCasted = dynamic_cast<OrNode const &>(other);
    return *this->lhs == *otherCasted.lhs && *this->rhs == *otherCasted.rhs;
}

unique_ptr<kodiak::BooleanExpressions::Node> OrNode::doClone() const {
    return std::make_unique<OrNode>(*this->lhs->clone(), *this->rhs->clone());
}

namespace kodiak {
    namespace BooleanExpressions {

        std::unique_ptr<kodiak::BooleanExpressions::Node>
        operator||(std::unique_ptr<Node> const &lhs, std::unique_ptr<Node> const &rhs) {
            return std::make_unique<OrNode>(*lhs, *rhs);
        }

        std::unique_ptr<kodiak::BooleanExpressions::Node>
        operator||(Node const &lhs, Node const &rhs) {
            return std::make_unique<OrNode>(lhs, rhs);
        }
    }
}
