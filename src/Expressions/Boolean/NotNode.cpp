#include "NotNode.hpp"

using namespace kodiak;
using namespace kodiak::BooleanExpressions;

Certainty NotNode::doEvaluate(const Environment &env,
                              const bool useBernstein,
                              const real eps) const {
    Certainty operandResult = operand_->eval(env, useBernstein, eps);
    switch (operandResult) {
        case TRUE:
            return FALSE;
        case FALSE:
            return TRUE;
        case POSSIBLY:
            return POSSIBLY;
        case TRUE_WITHIN_EPS:
            return POSSIBLY;
    }
}

void NotNode::doPrint(std::ostream &ostream) const {
    ostream << "! ";
    operand_->print(ostream);
}

bool NotNode::equals(const Node &other) const {
    if (typeid(*this) != typeid(other))
        return false;

    return *this->operand_ == *dynamic_cast<const NotNode &>(other).operand_;
}

unique_ptr<Node> NotNode::doClone() const {
    return std::make_unique<NotNode>(this->operand_->clone());
}

std::unique_ptr<Node> kodiak::BooleanExpressions::operator!(const std::unique_ptr<Node> &arg) {
    return std::make_unique<NotNode>(arg);
}

std::unique_ptr<Node> kodiak::BooleanExpressions::operator!(const Node &arg) {
    return std::make_unique<NotNode>(arg);
}
