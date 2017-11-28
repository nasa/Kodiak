#ifndef KODIAK_VALUE_HPP
#define KODIAK_VALUE_HPP

#include <iostream>

namespace kodiak {

    class Value {
    public:
        virtual void print(std::ostream &) const = 0;
    };
}

#endif //KODIAK_VALUE_HPP
