#include "ConstantNode.hpp"

using namespace kodiak;
using namespace kodiak::BooleanExpressions;

ConstantNode::ConstantNode(const Certainty c) : value(c) {
}

Certainty ConstantNode::getValue() const {
    return this->value;
}

void ConstantNode::doPrint(std::ostream &cout) const {
    switch (this->value) {
        case TRUE:            cout << "TRUE";            break;
        case FALSE:           cout << "FALSE";           break;
        case POSSIBLY:        cout << "POSSIBLY";        break;
        case TRUE_WITHIN_EPS: cout << "TRUE WITHIN EPS"; break;
    }
}

Certainty ConstantNode::doEvaluate(const Environment &, const bool, const real) const {
    return this->value;
}

bool ConstantNode::equals(const Node &other) const {

    if (typeid(*this) != typeid(other))
        return false;

    return this->value == dynamic_cast<const ConstantNode&>(other).value;
}

unique_ptr<Node> ConstantNode::doClone() const {
    return std::make_unique<ConstantNode>(this->value);
}

