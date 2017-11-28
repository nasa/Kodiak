#include "InvalidBooleanNode.hpp"

using namespace kodiak;
using namespace kodiak::BooleanExpressions;

Certainty InvalidBooleanNode::doEvaluate(const Environment &env, const bool i, const real eps) const {
    return POSSIBLY;
}

void InvalidBooleanNode::doPrint(std::ostream &cout) const {
    cout << "INVALID";
}

bool InvalidBooleanNode::equals(const Node &other) const {
    if (typeid(*this) != typeid(other))
        return false;

    return true;
}

unique_ptr<kodiak::BooleanExpressions::Node> InvalidBooleanNode::doClone() const {
    return std::make_unique<InvalidBooleanNode>();
}
