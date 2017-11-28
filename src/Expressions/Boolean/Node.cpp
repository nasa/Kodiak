#include "Expressions/Boolean/Node.hpp"

namespace kodiak {
    namespace BooleanExpressions {

        std::ostream &operator<<(std::ostream &os, const Node &node) {
            node.print(os);
            return os;
        }
    }
}

