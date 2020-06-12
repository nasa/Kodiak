#ifndef KODIAK_MINMAXSYSTEM
#define KODIAK_MINMAXSYSTEM

#include "BranchAndBoundDF.hpp"
#include "Environment.hpp"
#include "MinMax.hpp"
#include "System.hpp"

namespace kodiak {

    class MinMaxSystem : public System, public BranchAndBoundDF<Certainties, MinMax, Environment> {
    public:

        // If bp is true, Bernstein polynomials are used whenever possible. 
        // Evaluation of these polynomials is expensive but the enclosures 
        // are more precise.
        MinMaxSystem(const std::string = "");

        // min_or_max     : MINMAX, MIN, or MAX

        const MinMax &answer() const {
            return minmax_;
        }

        void minmax(const Real &, const MinMaxType = MINMAX);

        void min(const Real &e) {
            minmax(e, MIN);
        }

        void max(const Real &e) {
            minmax(e, MAX);
        }
        void print(std::ostream & = std::cout) const;
        void benchmark(std::ostream & = std::cout);

    protected:
        void split(const DirVar &, Environment &, const Interval &, const real);
        void evaluate(MinMax &, Certainties &, Environment &);
        void combine(MinMax &, const DirVar &, const MinMax &);
        void combine(MinMax &, const DirVar &, const MinMax &,
                const MinMax &);
        void accumulate(const MinMax &);
        bool prune(const MinMax &);
        bool local_exit(const MinMax &);
        // set_varselect:
        // 0 : round robin
        // 1 : select variable of maximum sos-weight on the function to be min/max
        // 2 : select variable of maximum sos-weight on the system of constraints
        void select(DirVar &, Certainties &, Environment &);
        bool isSound(const MinMax &, const Certainties &, const Environment &);
        Box temp_; // Temporary box for internal computations
        DirVar dirvar_; // Selected DirVar object at each recursive step
        Real expr_; // Expression
        Realn dexpr_; // Partial derivatives of expression
        MinMax acc_; // Accumulated value for pruning
        // min_or_max_ = 0 : compute both min and max. This is the default value.
        // min_or_max_ < 0 : compute min
        // min_or_max_ > 0 : compute max
        Ints min_or_max_; // Stack of min_or_max objective
        MinMax minmax_; // MinMax computed by branch and bound algorithm
    };

    Interval min(const Real &, const NamedBox &, const NamedBox &, const int = -1, const nat = 0);

    Interval min(const Real &, const NamedBox &, const int = -1, const nat = 0);

    Interval max(const Real &, const NamedBox &, const NamedBox &, const int = -1, const nat = 0);

    Interval max(const Real &, const NamedBox &, const int = -1, const nat = 0);

    Interval outclosure(const Real &, const NamedBox &, const NamedBox &, const int = -1, const nat = 0);

    Interval outclosure(const Real &, const NamedBox &, const int = -1, const nat = 0);

    Interval inclosure(const Real &, const NamedBox &, const NamedBox &, const int = -1, const nat = 0);

    Interval inclosure(const Real &, const NamedBox &, const int = -1, const nat = 0);

}

#endif // KODIAK_MINMAX_SYSTEM
