#ifndef KODIAK_ENVIRONMENT_HPP
#define KODIAK_ENVIRONMENT_HPP

#include "types.hpp"
#include "Value.hpp"

#include <memory>
#include <vector>
#include <Expressions/Boolean/Certainty.hpp>

using std::unique_ptr;

namespace kodiak {

    using BBox = std::vector<Certainty>;

    extern const BBox EmptyBBox;

    struct Environment {
        Environment() : bbox(EmptyBBox), box(EmptyBox), namedBox(EmptyNamedBox) {}
        Environment(const BBox &bbox, const Box &box, const NamedBox &namedBox) : bbox(bbox), box(box), namedBox(namedBox) {}
        Environment(const Environment &env) : bbox(env.bbox), box(env.box), namedBox(env.namedBox) {}
        BBox bbox;
        Box box;
        NamedBox namedBox;

        Interval& operator[](int n) {
            return this->box[n];
        }

        void resize(int nvars) {
            this->box.resize(nvars);
        }

        nat size() {
            return this->box.size();
        }

        bool empty() {
            return this->box.empty();
        }

        bool operator== (const Environment &another) const {
            return this->box == another.box
                    && this->bbox == another.bbox;
        }

        friend std::ostream& operator<< (std::ostream& stream, const Environment& env) {
            return stream << env.box;
        }
    };

    extern const Environment &EmptyEnvironment;
}
#endif //KODIAK_ENVIRONMENT_HPP
