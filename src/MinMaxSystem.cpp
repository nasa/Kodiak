#include "MinMaxSystem.hpp"

using namespace kodiak;

MinMaxSystem::MinMaxSystem(const std::string id) {
    if (id != "") ID = id;
    varselect_ = 1;
    granularityAkaDefaultResolutionForVariablesAsPercentageOfInitialBox_ = 0;
    absoluteToleranceForStoppingBranchAndBound_ = 0.01;
}

bool MinMaxSystem::isSound(const MinMax &minmax, const Certainties &, const Environment &box) {
    Box b = box.box;
    int cert = evalSystem(b);
    return cert == 0 || (!minmax.empty() && minmax.isSound() &&
            (cert <= 0 ||
            ((minmax.min_or_max() > MINMAX || !minmax.min_point().empty()) &&
            (minmax.min_or_max() < MINMAX || !minmax.max_point().empty()))));
}

void MinMaxSystem::minmax(const Real &e, const MinMaxType min_or_max) {
    min_or_max_.clear();
    min_or_max_.push_back(min_or_max - 1);
    temp_.resize(variablesEnclosures_.size());
    expr_ = e.index(variablesEnclosures_, constantsEnclosures_);
    expr_.checkIndexed(globalDefinitions_);
    dexpr_.resize(variablesEnclosures_.size());
    for (nat v = 0; v < variablesEnclosures_.size(); ++v)
        dexpr_[v] = (defaultEnclosureMethodTrueBernsteinFalseInterval_ ? polynomial(expr_.deriv(v)) : expr_.deriv(v)).subs(defdx_);
    if (defaultEnclosureMethodTrueBernsteinFalseInterval_)
        expr_ = polynomial(expr_);
    minmax_.init(min_or_max_.back());
    Certainties certs;
    certs.init(numberOfRelationalFormulas());
    Environment env {EmptyBBox, variablesEnclosures_.box(), EmptyNamedBox};
    branchAndBound(minmax_, certs, env);
}

// dirvar.splitting = 0: normal split
// dirvar.splitting = 1: lower and upper bound splits

void MinMaxSystem::split(const DirVar &dirvar, Environment &env, const Interval &i, const real mid) {
    Box &box = env.box;
    if (dirvar.splitting == 0) {
        if (dirvar.dir)
            box[dirvar.var] = Interval(i.inf(), mid);
        else
            box[dirvar.var] = Interval(mid, i.sup());
    } else if (dirvar.splitting == 1) {
        if (dirvar.dir)
            box[dirvar.var] = i.infimum();
        else
            box[dirvar.var] = i.supremum();
        if (min_or_max_.back() == 0 && dirvar.first())
            min_or_max_.push_back(-1); // Compute min from this point on
        else if (!dirvar.first()) {
            min_or_max_.pop_back();
            min_or_max_.push_back(1); // Compute max from this point on
        }
    }
}

void MinMaxSystem::evaluate(MinMax &answer, Certainties &certs, Environment &box) {
    if (debug() > 1) {
        std::cout << "-- " << splits() << " (" << dirvars().size() << ") --" << std::endl;
        std::cout << "Dirvars: " << dirvars() << std::endl;
    }
    NamedBox defbox;
    dirvar_.init(box.size());
    int cert = evalSystem(box.box, certs, dirvars());
    defbox = globalDefinitionsEnclosures_;
    if (debug() > 1)
        std::cout << "Environment: " << box.box << ", Certs: " << certs << std::endl;
    if (cert == 0) // Constraints are not satisfied
        return;
    Point mid_point;
    mid_point.resize(box.size());
    for (nat v = 0; v < box.size(); ++v) {
        real mid = box[v].mid();
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
    nat var_min = box.size();
    nat var_max = box.size();
    real var_min_point = 0;
    real var_max_point = 0;
    real max_diff = 0;
    Interval d_it;
    for (VarBag::iterator it = expr_.vars().begin(); it != expr_.vars().end(); ++it) {
        nat v = it->first;
        if (!box[v].isPoint() && box[v].diam() > variableResolutionsAkaSmallestRangeConsideredForEachVariable_[v]) {
            Interval save_it = temp_[v];
            temp_[v] = box[v].infimum();
            int inf_cert = evalSystem(temp_);
            Interval lb_it = expr_.eval(temp_, globalDefinitionsEnclosures_);
            if (inf_cert > 0) { // Infpoint satisfies constraints
                if (answer.min_point_.empty() || lb_it.sup() < answer.ub_of_min_) {
                    answer.ub_of_min_ = lb_it.sup();
                    var_min = v;
                    var_min_point = box[v].inf();
                    if (answer.min_point_.empty())
                        answer.min_point_ = mid_point; //.assign(mid_point.begin(),mid_point.end());
                }
                if (answer.max_point_.empty() || lb_it.inf() > answer.lb_of_max_) {
                    answer.lb_of_max_ = lb_it.inf();
                    var_max = v;
                    var_max_point = box[v].inf();
                    if (answer.max_point_.empty())
                        answer.max_point_ = mid_point; //.assign(mid_point.begin(),mid_point.end());
                }
            }
            temp_[v] = box[v].supremum();
            int sup_cert = evalSystem(temp_);
            Interval ub_it = expr_.eval(temp_, globalDefinitionsEnclosures_);
            if (sup_cert > 0) { // Suppoint satisfies constraints
                if (answer.min_point_.empty() || ub_it.sup() < answer.ub_of_min_) {
                    answer.ub_of_min_ = ub_it.sup();
                    var_min = v;
                    var_min_point = box[v].sup();
                    if (answer.min_point_.empty())
                        answer.min_point_ = mid_point; //.assign(mid_point.begin(),mid_point.end());
                }
                if (answer.max_point_.empty() || ub_it.inf() > answer.lb_of_max_) {
                    answer.lb_of_max_ = ub_it.inf();
                    var_max = v;
                    var_max_point = box[v].sup();
                    if (answer.max_point_.empty())
                        answer.max_point_ = mid_point; //.assign(mid_point.begin(),mid_point.end());
                }
            }
            if (cert > 0) {
                try {
                    d_it = dexpr_[v].eval(box.box, defbox, defaultEnclosureMethodTrueBernsteinFalseInterval_);
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
                                    (min_or_max_.back() != 0 && w == LEFT_INTERIOR && dirvar_.dir) ||
                                    (min_or_max_.back() != 0 && w == RIGHT_INTERIOR && !dirvar_.dir)) {
                                // The solution is necessarily found outside this box
                                if (ge) {
                                    answer.mm_ = Interval(lb_it.inf(),ub_it.sup());
                                    answer.lb_of_max_ = ub_it.inf();
                                    answer.ub_of_min_ = lb_it.sup();
                                } else {
                                    answer.mm_ = Interval(ub_it.inf(),lb_it.sup());
                                    answer.lb_of_max_ = lb_it.inf();
                                    answer.ub_of_min_ = ub_it.sup();
                                }
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
                } catch (Growl const & growl) {
                    if (Kodiak::debug()) {
                        std::cout << "[GrowlException@MinMaxSystem::evaluate]" << growl.what() << std::endl;
                    }
                }
            }
            if (varselect_ > 0) {
                temp_[v] = box[v];
                evalGlobalDefinitions(temp_);
                if (cert > 0 || varselect_ == 1)
                    // Narkawitz/Munoz's strategy for variable selection
                    d_it = expr_.eval(temp_, globalDefinitionsEnclosures_);
                else
                    sosSystem(d_it, certs, temp_, v);
                real max_diff_it = d_it.diam();
                if (dirvar_.var == box.size() || max_diff_it > max_diff) {
                    dirvar_.var = v;
                    max_diff = max_diff_it;
                    dirvar_.dir = (lb_it.inf() <= ub_it.sup() && min_or_max_.back() <= 0);
                }
            }
            temp_[v] = save_it;
        }
    }
    answer.mm_ = expr_.eval(box.box, defbox, defaultEnclosureMethodTrueBernsteinFalseInterval_);
    if (var_min < box.size()) {
        answer.min_point_[var_min] = var_min_point;
    }
    if (var_max < box.size()) {
        answer.max_point_[var_max] = var_max_point;
    }
}

void MinMaxSystem::combine(MinMax &answer, const DirVar &dirvar,
        const MinMax &ans1) {
    if (dirvar.splitting == 1) {
        if (min_or_max_.back() <= 0)
            answer.mm_ = Interval(ans1.mm_.inf(), answer.mm_.sup());
        else if (!ans1.mm_.empty())
            answer.mm_ = Interval(answer.mm_.inf(), ans1.mm_.sup());
    }
    if (!ans1.min_point_.empty()) {
        if (answer.min_point_.empty() || ans1.ub_of_min_ < answer.ub_of_min_) {
            answer.ub_of_min_ = ans1.ub_of_min_;
            answer.min_point_ = ans1.min_point_;
        }
    }
    if (!ans1.max_point_.empty()) {
        if (answer.max_point_.empty() || ans1.lb_of_max_ > answer.lb_of_max_) {
            answer.lb_of_max_ = ans1.lb_of_max_;
            answer.max_point_ = ans1.max_point_;
        }
    }
}

void MinMaxSystem::combine(MinMax &answer, const DirVar &dirvar,
        const MinMax &ans1, const MinMax &ans2) {
    if (dirvar.splitting == 0) {
        if (ans1.empty())
            answer.mm_ = ans2.mm_;
        else if (ans2.empty())
            answer.mm_ = ans1.mm_;
        else
            answer.mm_ = ans1.mm_.hull(ans2.mm_);
    } else if (dirvar.splitting == 1) {
        min_or_max_.pop_back();
        if (ans1.empty())
            answer.mm_ = ans2.mm_;
        else if (ans2.empty())
            answer.mm_ = ans1.mm_;
        else {
            if (min_or_max_.back() <= 0)
                answer.mm_ = Interval(ans1.mm_.inf(), ans2.mm_.sup());
            else
                answer.mm_ = Interval(ans2.mm_.inf(), ans1.mm_.sup());
        }
    }
    if (!ans1.min_point_.empty() &&
            (ans2.min_point_.empty() || ans1.ub_of_min_ < ans2.ub_of_min_)) {
        answer.ub_of_min_ = ans1.ub_of_min_;
        answer.min_point_ = ans1.min_point_;
    } else if (!ans2.min_point_.empty()) {
        answer.ub_of_min_ = ans2.ub_of_min_;
        answer.min_point_ = ans2.min_point_;
    }
    if (!ans1.max_point_.empty() &&
            (ans2.max_point_.empty() || ans1.lb_of_max_ > ans2.lb_of_max_)) {
        answer.lb_of_max_ = ans1.lb_of_max_;
        answer.max_point_ = ans1.max_point_;
    } else if (!ans2.max_point_.empty()) {
        answer.lb_of_max_ = ans2.lb_of_max_;
        answer.max_point_ = ans2.max_point_;
    }
}

void MinMaxSystem::accumulate(const MinMax &answer) {
    if (!answer.min_point_.empty() &&
            (acc_.min_point_.empty() || answer.ub_of_min_ < acc_.ub_of_min_)) {
        acc_.min_point_ = answer.min_point_;
        acc_.ub_of_min_ = answer.ub_of_min_;
    }
    if (!answer.max_point_.empty() &&
            (acc_.max_point_.empty() || answer.lb_of_max_ > acc_.lb_of_max_)) {
        acc_.max_point_ = answer.max_point_;
        acc_.lb_of_max_ = answer.lb_of_max_;
    }
}

bool MinMaxSystem::prune(const MinMax &answer) {
    bool b =
            (min_or_max_.back() > 0 || (!answer.empty() && !acc_.min_point_.empty() &&
            acc_.ub_of_min_ <= answer.mm_.inf())) &&
            (min_or_max_.back() < 0 || (!answer.empty() && !acc_.max_point_.empty() &&
            answer.mm_.sup() <= acc_.lb_of_max_));
    return b;
}

bool MinMaxSystem::local_exit(const MinMax &answer) {
    const bool isOptimizedBranch = answer.empty();
    if (isOptimizedBranch) return true;

#ifdef DEBUG
    assert(answer.ub_of_min_ >= answer.mm_.inf());
    assert(answer.mm_.sup() >= answer.lb_of_max_);
#endif
    const int &currentTarget = min_or_max_.back();
    const bool isMaximizing = currentTarget > 0;
    const bool lowerClosurePrecissionReached =
            !answer.min_point_.empty() &&
            answer.ub_of_min_ > answer.mm_.inf() &&
            answer.ub_of_min_ - answer.mm_.inf() <= absoluteToleranceForStoppingBranchAndBound_;
    if (!isMaximizing && !lowerClosurePrecissionReached)
        return false;
    const bool isMinimizing = currentTarget < 0;
    const bool upperClosurePrecissionReached =
            !answer.max_point_.empty() &&
            answer.mm_.sup() > answer.lb_of_max_ &&
            answer.mm_.sup() - answer.lb_of_max_ <= absoluteToleranceForStoppingBranchAndBound_;
    return (isMinimizing || upperClosurePrecissionReached);
}

void MinMaxSystem::select(DirVar &dirvar, Certainties &certs, Environment &box) {
    if (varselect_ == 0)
        round_robin(dirvar, certs, box);
    else
        dirvar = dirvar_;
}

void MinMaxSystem::print(std::ostream &os) const {
    os << "** Inputs **" << std::endl;
    printSystem(os, debug());
    if (debug() > 0) {
        for (nat v = 0; v < initialBox().box.size(); ++v) {
            os << "  d" << id() << "/d" << variablesEnclosures_.name(v) << " = "
                    << dexpr_[v] << std::endl;
        }
    }
    os << "Function:" << std::endl;
    os << "  " << id() << " = " << expr_ << std::endl;
    os << "** Outputs **" << std::endl;
    minmax_.print(os);
    print_info(os);
    os << std::endl;

    os << "Enclosure type: "
       << (this->defaultEnclosureMethodTrueBernsteinFalseInterval_ ? "bernstein" : "interval")
       << std::endl;

    os << "Max. depth: "
       << this->maxdepth()
       << std::endl;

    os << "Abs. tolerance: "
       << this->tolerance()
       << std::endl;
}

void MinMaxSystem::benchmark(std::ostream &os) {
    os << std::setprecision(10);
    os << this->id();
    os << ",";
    const MinMax &minMax = this->answer();
    if (minMax.isSound()) {
        os << minMax.ub_of_max();
    } else {
        os << "UNSOUND{" << minMax.ub_of_max() << "}";
    }
    os << std::endl;
}

Interval kodiak::min(const Real &e, const NamedBox &varbox, const NamedBox &constbox,
        const int precision, const nat maxdepth) {
    MinMaxSystem s;
    s.set_maxdepth(maxdepth);
    s.set_precision(precision);
    s.vars(varbox);
    Real expr = e.index(varbox, constbox);
    expr.checkIndexed();
    s.min(expr);
    return s.answer().min();
}

Interval min(const Real &e, const NamedBox &varbox, const int precision = -1, const nat maxdepth = 0) {
    return min(e, varbox, kodiak::EmptyNamedBox, precision, maxdepth);
}

Interval kodiak::max(const Real &e, const NamedBox &varbox, const NamedBox &constbox,
        const int precision, const nat maxdepth) {
    MinMaxSystem s;
    s.set_maxdepth(maxdepth);
    s.set_precision(precision);
    s.vars(varbox);
    Real expr = e.index(varbox, constbox);
    expr.checkIndexed();
    s.max(expr);
    return s.answer().max();
}

Interval max(const Real &e, const NamedBox &varbox, const int precision = -1, const nat maxdepth = 0) {
    return max(e, varbox, kodiak::EmptyNamedBox, precision, maxdepth);
}

Interval kodiak::outclosure(const Real &e, const NamedBox &varbox, const NamedBox &constbox,
        const int precision, const nat maxdepth) {
    MinMaxSystem s;
    s.set_maxdepth(maxdepth);
    s.set_precision(precision);
    s.vars(varbox);
    Real expr = e.index(varbox, constbox);
    expr.checkIndexed();
    s.minmax(expr);
    return s.answer().outclosure();
}

Interval outclosure(const Real &e, const NamedBox &varbox, const int precision = -1, const nat maxdepth = 0) {
    return outclosure(e, varbox, kodiak::EmptyNamedBox, precision, maxdepth);
}

Interval kodiak::inclosure(const Real &e, const NamedBox &varbox, const NamedBox &constbox,
        const int precision, const nat maxdepth) {
    MinMaxSystem s;
    s.set_maxdepth(maxdepth);
    s.set_precision(precision);
    s.vars(varbox);
    Real expr = e.index(varbox, constbox);
    expr.checkIndexed();
    s.minmax(expr);
    return s.answer().inclosure();
}

Interval inclosure(const Real &e, const NamedBox &varbox, const int precision = -1, const nat maxdepth = 0) {
    return inclosure(e, varbox, kodiak::EmptyNamedBox, precision, maxdepth);
}
