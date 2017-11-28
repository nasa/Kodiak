#ifndef KODIAK_VARIABLENODE_HPP
#define KODIAK_VARIABLENODE_HPP

#include "Expressions/Boolean/Node.hpp"

namespace kodiak {
    namespace BooleanExpressions {

        class VariableNode : public Node {
        public:
            VariableNode(const nat index, const std::string name = "") : index_(index) {
                if (name.empty()) {
                    std::ostringstream autoName;
                    autoName << "name{" << index << "}";
                    this->name_ = autoName.str();
                } else {
                    this->name_ = name;
                }
            }

            std::string getName() const {
                return this->name_;
            }

        private:
            virtual Certainty doEvaluate(const Environment &env, const bool, const real) const override {
                return env.bbox[index_];
            }

            virtual void doPrint(std::ostream &os = std::cout) const override {
                os << "boolVar{" << this->index_ << "," << this->name_ << "}";
            }

            virtual bool equals(const Node &) const override;

            virtual unique_ptr<Node> doClone() const override;

            nat index_;
            std::string name_;
        };
    }
}

#endif
