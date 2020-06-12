#ifndef KODIAK_BOOLEANEXPRESSIONS_NODE_HPP
#define KODIAK_BOOLEANEXPRESSIONS_NODE_HPP

#include "Environment.hpp"
#include "Expressions/Boolean/Certainty.hpp"
#include "types.hpp"

namespace kodiak {

    namespace BooleanExpressions {

        class Node {
        public:
            virtual ~Node() = default;

            Certainty eval(const Environment &env, const bool useBernstein, const real eps) const {
                return this->doEvaluate(env, useBernstein, eps);
            }

            void print(std::ostream &os) const {
                return this->doPrint(os);
            }

            bool operator==(const Node &another) const {
                return equals(another);
            }

            std::unique_ptr<Node> clone() const {
                return doClone();
            }

            bool isNaB() const {
                return doIsNaB();
            }

        private:
            virtual Certainty doEvaluate(const Environment &, const bool, const real) const = 0;

            virtual void doPrint(std::ostream &) const = 0;

            virtual bool equals(const Node &) const = 0;

            virtual std::unique_ptr<Node> doClone() const = 0;

            virtual bool doIsNaB() const {
                return false;
            }
        };

        std::ostream& operator<<(std::ostream &os, const Node &node);
    }
}

#endif
