#include "VariableNode.hpp"

using namespace kodiak::BooleanExpressions;

bool VariableNode::equals(const Node &other) const {
    if (typeid(*this) != typeid(other))
        return false;

    return this->index_ == dynamic_cast<const VariableNode &>(other).index_;
}

unique_ptr<Node> VariableNode::doClone() const {
    return std::make_unique<VariableNode>(this->index_, this->name_);
}
