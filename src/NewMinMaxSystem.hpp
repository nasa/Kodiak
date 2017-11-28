#ifndef KODIAK_NEWMINMAXSYSTEM_HPP
#define KODIAK_NEWMINMAXSYSTEM_HPP

#include "MinMaxSystem.hpp"
#include "Expressions/Boolean/Node.hpp"
#include "Expressions/Boolean/ConstantNode.hpp"

namespace kodiak {

    using namespace kodiak::BooleanExpressions;

    class NewMinMaxSystem : public MinMaxSystem {
    public:

        NewMinMaxSystem(const std::string id = "") :
                MinMaxSystem(id),
                booleanExpression_(std::make_unique<ConstantNode>(TRUE)) {}

        nat numberOfRelationalFormulas() const {
            return this->booleanExpression_ != nullptr ? 1 : 0;
        }

        void setBooleanExpression(const kodiak::BooleanExpressions::Node &expr) {
            // TODO: Index expression
            this->booleanExpression_ = expr.clone();
        }

        void setBooleanExpression(const std::unique_ptr<kodiak::BooleanExpressions::Node> expr) {
            // TODO: Index expression
            this->booleanExpression_ = expr->clone();
        }

        const kodiak::BooleanExpressions::Node &getBooleanExpression() const {
            return *(this->booleanExpression_);
        }

        void evaluate(MinMax &answer, Certainties &certs, Environment &env) {
            if (debug() > 1) {
                std::cout << "-- " << splits() << " (" << dirvars().size() << ") --" << std::endl;
                std::cout << "Dirvars: " << dirvars() << std::endl;
            }
            NamedBox defbox;
            dirvar_.init(env.size());
            int cert = evalSystem(env, certs, dirvars());
            defbox = globalDefinitionsEnclosures_;
            if (debug() > 1)
                std::cout << "Environment: " << env.box << ", Certs: " << certs << std::endl;
            if (cert == 0) // Constraints are not satisfied
                return;
            Point mid_point;
            mid_point.resize(env.size());
            for (nat v = 0; v < env.size(); ++v) {
                real mid = env[v].mid();
                temp_[v] = Interval(mid, mid);
                mid_point[v] = mid;
            }
            int mid_cert;
            if (cert > 0) {
                mid_cert = cert;
                evalGlobalDefinitions(temp_);
            } else
                mid_cert = evalSystem(temp_);
            if (mid_cert > 0) { // Midpoint satisfies constraints
                Interval Mid = expr_.eval(temp_, globalDefinitionsEnclosures_);
                answer.min_point_ = mid_point; //.assign(mid_point.begin(),mid_point.end());
                answer.max_point_ = mid_point; //.assign(mid_point.begin(),mid_point.end());
                answer.ub_of_min_ = Mid.sup();
                answer.lb_of_max_ = Mid.inf();
            }
            nat var_min = env.size();
            real var_min_point;
            nat var_max = env.size();
            real var_max_point;
            real max_diff;
            Interval d_it;
            for (VarBag::iterator it = expr_.vars().begin(); it != expr_.vars().end(); ++it) {
                nat v = it->first;
                if (!env[v].isPoint() &&
                    env[v].diam() > variableResolutionsAkaSmallestRangeConsideredForEachVariable_[v]) {
                    Interval save_it = temp_[v];
                    temp_[v] = env[v].infimum();
                    int inf_cert = evalSystem(temp_);
                    Interval lb_it = expr_.eval(temp_, globalDefinitionsEnclosures_);
                    if (inf_cert > 0) { // Infpoint satisfies constraints
                        if (answer.min_point_.empty() || lb_it.sup() < answer.ub_of_min_) {
                            answer.ub_of_min_ = lb_it.sup();
                            var_min = v;
                            var_min_point = env[v].inf();
                            if (answer.min_point_.empty())
                                answer.min_point_ = mid_point; //.assign(mid_point.begin(),mid_point.end());
                        }
                        if (answer.max_point_.empty() || lb_it.inf() > answer.lb_of_max_) {
                            answer.lb_of_max_ = lb_it.inf();
                            var_max = v;
                            var_max_point = env[v].inf();
                            if (answer.max_point_.empty())
                                answer.max_point_ = mid_point; //.assign(mid_point.begin(),mid_point.end());
                        }
                    }
                    temp_[v] = env[v].supremum();
                    int sup_cert = evalSystem(temp_);
                    Interval ub_it = expr_.eval(temp_, globalDefinitionsEnclosures_);
                    if (sup_cert > 0) { // Suppoint satisfies constraints
                        if (answer.min_point_.empty() || ub_it.sup() < answer.ub_of_min_) {
                            answer.ub_of_min_ = ub_it.sup();
                            var_min = v;
                            var_min_point = env[v].sup();
                            if (answer.min_point_.empty())
                                answer.min_point_ = mid_point; //.assign(mid_point.begin(),mid_point.end());
                        }
                        if (answer.max_point_.empty() || ub_it.inf() > answer.lb_of_max_) {
                            answer.lb_of_max_ = ub_it.inf();
                            var_max = v;
                            var_max_point = env[v].sup();
                            if (answer.max_point_.empty())
                                answer.max_point_ = mid_point; //.assign(mid_point.begin(),mid_point.end());
                        }
                    }
                    if (cert > 0) {
                        try {
                            d_it = dexpr_[v].eval(env.box, defbox, defaultEnclosureMethodTrueBernsteinFalseInterval_);
                            bool ge = d_it.cge(0);
                            bool le = d_it.cle(0);
                            if (ge || le) {
                                dirvar_.dir = true;
                                if ((ge && min_or_max_.back() > 0) ||
                                    (le && min_or_max_.back() < 0))
                                    dirvar_.dir = false;
                                // dir represents the lower (false) or upper (right) bound.
                                if (!d_it.contains(0)) {
                                    Where w = where(dirvars(), v);
                                    if (w == INTERIOR ||
                                        min_or_max_.back() != 0 && w == LEFT_INTERIOR && dirvar_.dir ||
                                        min_or_max_.back() != 0 && w == RIGHT_INTERIOR && !dirvar_.dir) {
                                        // The solution is necessarily found outside this env
                                        return;
                                    }
                                }
                                dirvar_.var = v;
                                dirvar_.splitting = 1;
                                if (min_or_max_.back() != 0)
                                    dirvar_.onlyone = true; // Only compute one side
                                else if (le)
                                    dirvar_.dir = false; // Begin with the min
                                break;
                            }
                        } catch (Growl growl) {
                        }
                    }
                    if (varselect_ > 0) {
                        temp_[v] = env[v];
                        evalGlobalDefinitions(temp_);
                        if (cert > 0 || varselect_ == 1)
                            // Narkawitz/Munoz's strategy for variable selection
                            d_it = expr_.eval(temp_, globalDefinitionsEnclosures_);
                        else
                            sosSystem(d_it, certs, temp_, v);
                        real max_diff_it = d_it.diam();
                        if (dirvar_.var == env.size() || max_diff_it > max_diff) {
                            dirvar_.var = v;
                            max_diff = max_diff_it;
                            dirvar_.dir = (lb_it.inf() <= ub_it.sup() && min_or_max_.back() <= 0);
                        }
                    }
                    temp_[v] = save_it;
                }
            }
            answer.mm_ = expr_.eval(env.box, defbox, defaultEnclosureMethodTrueBernsteinFalseInterval_);
            if (var_min < env.size()) {
                answer.min_point_[var_min] = var_min_point;
            }
            if (var_max < env.size()) {
                answer.max_point_[var_max] = var_max_point;
            }
        }

        int evalSystem(Box &box) override {
            Environment env{EmptyBBox, box, EmptyNamedBox};
            evalGlobalDefinitions();
            return CertaintyClass::certainty2Int(
                    this->booleanExpression_->eval(env, this->defaultEnclosureMethodTrueBernsteinFalseInterval_,
                                                   this->absoluteToleranceForStoppingBranchAndBound_)
            );
        }

        int evalSystem(Environment &env, Certainties &certs, const DirVars &dirvars) {
            return this->evalSystem(env.box, certs, dirvars);
        }

        int evalSystem(Box &box, Certainties &certs, const DirVars &dirvars) {
            evalGlobalDefinitions();
            Environment env{EmptyBBox, box, EmptyNamedBox};
            if (certs.get() > 0) return certs.get();
            int cert = 1;
            for (nat f = 0; f < 1; ++f) {
                // if f-th formula is certainly true, don't check it again
                if (certs.get(f) > 0) continue;
                int evaluation = CertaintyClass::certainty2Int(
                        this->booleanExpression_->eval(env, this->defaultEnclosureMethodTrueBernsteinFalseInterval_,
                                                       this->absoluteToleranceForStoppingBranchAndBound_)
                );
                certs.set(f, evaluation);
                // if f-th formula is certainly not true, conjunction of formulas is certainly not true
                if (certs.get(f) == 0)
                    return certs.set(0);
                if (certs.get(f) < 0)
                    cert = (cert > 0 ? certs.get(f) : std::max(cert, certs.get(f)));
            }
            return certs.set(cert);
        }

    private:
        std::unique_ptr<kodiak::BooleanExpressions::Node> booleanExpression_;
    };

}

#endif //KODIAK_NEWMINMAXSYSTEM_HPP
