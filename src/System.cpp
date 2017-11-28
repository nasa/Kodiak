#include "System.hpp"

using namespace kodiak;

nat System::globalCounterOfSystemIDs = 0;

void Certainties::print(std::ostream &os) const {
    os << cert_ << " : " << certs_;
}

std::ostream &kodiak::operator<<(std::ostream &os, const Certainties &certs) {
    certs.print(os);
    return os;
}

System::System(const std::string id) :
isConjunctive_(true), defaultEnclosureMethodTrueBernsteinFalseInterval_(false), absoluteToleranceForStoppingBranchAndBound_(0.1), granularityAkaDefaultResolutionForVariablesAsPercentageOfInitialBox_(0), varselect_(0) {
    if (id == "") {
        std::ostringstream os;
        os << "F" << globalCounterOfSystemIDs++;
        ID = os.str();
    } else
        ID = id;
}

// Set default tolerance

void System::set_tolerance(const real tolerance) {
    if (tolerance >= 0)
        absoluteToleranceForStoppingBranchAndBound_ = tolerance;
    else {
        std::ostringstream os;
        os << "Kodiak (set_tolerance): tolerance " << tolerance << " must be non-negative";
        throw Growl(os.str());
    }
}

// Set granularity

void System::set_granularity(const real granularity) {
    if (granularity >= 0 && granularity <= 1)
        granularityAkaDefaultResolutionForVariablesAsPercentageOfInitialBox_ = granularity;
    else {
        std::ostringstream os;
        os << "Kodiak (set_granularity): granularity " << granularity << " must be in [0,1]";
        throw Growl(os.str());
    }
}

// Set resolution for variable name

void System::set_resolution(const std::string name, const real resolution) {
    nat v = variablesEnclosures_.var(name);
    if (v < variableResolutionsAkaSmallestRangeConsideredForEachVariable_.size()) {
        if (resolution >= 0)
            variableResolutionsAkaSmallestRangeConsideredForEachVariable_[v] = resolution;
        else {
            std::ostringstream os;
            os << "Kodiak (set_resolution): variable resolution " << resolution << " must be non-negative";
            throw Growl(os.str());
        }
    } else {
        std::ostringstream os;
        os << "Kodiak (set_resolution): variable \"" << name << "\" doesn't exist in named box";
        throw Growl(os.str());
    }
}

// Set resolution for variable v

void System::set_resolution(const nat v, const real resolution) {
    if (v < variableResolutionsAkaSmallestRangeConsideredForEachVariable_.size()) {
        if (resolution >= 0)
            variableResolutionsAkaSmallestRangeConsideredForEachVariable_[v] = resolution;
        else {
            std::ostringstream os;
            os << "Kodiak (set_resolution): variable resolution " << resolution << " must be non-negative";
            throw Growl(os.str());
        }
    } else {
        std::ostringstream os;
        os << "Kodiak (set_resolution): variable index " << v << " doesn't exist in named box";
        throw Growl(os.str());
    }
}

// Set resolutions for all variable

void System::set_resolutions(const real resolution) {
    if (resolution >= 0) {
        for (nat v = 0; v < variableResolutionsAkaSmallestRangeConsideredForEachVariable_.size(); ++v)
            variableResolutionsAkaSmallestRangeConsideredForEachVariable_[v] = resolution;
    } else {
        std::ostringstream os;
        os << "Kodiak (set_resolution): variable resolution " << resolution << " must be non-negative";
        throw Growl(os.str());
    }
}

// Return resolution of variable name

real System::resolution(const std::string name) const {
    nat v = variablesEnclosures_.var(name);
    if (v < variableResolutionsAkaSmallestRangeConsideredForEachVariable_.size())
        return variableResolutionsAkaSmallestRangeConsideredForEachVariable_[v];
    std::ostringstream os;
    os << "Kodiak (resolution): variable \"" << name << "\" doesn't exist in named box";
    throw Growl(os.str());
}

// Return resolution of n-th variable

real System::resolution(const nat v) const {
    if (v < variableResolutionsAkaSmallestRangeConsideredForEachVariable_.size())
        return variableResolutionsAkaSmallestRangeConsideredForEachVariable_[v];
    std::ostringstream os;
    os << "Kodiak (resolution): variable index " << v << " doesn't exist in named box";
    throw Growl(os.str());
}

void System::vars(const NamedBox &varbox) {
    nat r = variableResolutionsAkaSmallestRangeConsideredForEachVariable_.size();
    variableResolutionsAkaSmallestRangeConsideredForEachVariable_.insert(variableResolutionsAkaSmallestRangeConsideredForEachVariable_.end(), varbox.size(), 0);
    for (nat v = 0; v < varbox.size(); ++v) {
        Interval X = varbox.val(v);
        real resolution = (X.sup() - X.inf()) * granularityAkaDefaultResolutionForVariablesAsPercentageOfInitialBox_;
        variableResolutionsAkaSmallestRangeConsideredForEachVariable_[r + v] = resolution;
    }
    variablesEnclosures_.insert(varbox);
}

nat System::var(const std::string id, const Interval &lb, const Interval &ub) {
    real resolution = (ub.sup() - lb.inf()) * granularityAkaDefaultResolutionForVariablesAsPercentageOfInitialBox_;
    variableResolutionsAkaSmallestRangeConsideredForEachVariable_.push_back(resolution);
    return variablesEnclosures_.push(id, lb, ub);
}

void System::def(const std::string id, const Real &e, const bool bp) {
    Real r = e.index(variablesEnclosures_, constantsEnclosures_);
    r.checkIndexed(globalDefinitions_);
    globalDefinitions_.push_back(make_pair(id, bp ? polynomial(r) : r));
    globalDefinitionsEnclosures_.push(id);
    for (nat v = 0; v < variablesEnclosures_.size(); ++v) {
        Real dx = (bp ? polynomial(r.deriv(v)) : r.deriv(v)).subs(defdx_);
        std::ostringstream os;
        os << "d" << id << "/d" << var_name(v);
        if (dx.isConst())
            defdx_[os.str()] = dx;
        else {
            globalDefinitions_.push_back(make_pair(os.str(), dx));
            globalDefinitionsEnclosures_.push(os.str());
        }
    }
}

void System::defs(const NamedBox defbox, const std::vector< std::pair<std::string, Real> > defs) {
    Names names = defbox.names();
    Box values = defbox.box();
    for (nat i = 0; i < defbox.size(); ++i) {
        def(names[i], defs[i].second, false);

    }
}

// Index equation and add it to rels_. The Boolean parameter specifies the type of evaluation 
// for this equation, true meaning Bernstein enclosure.

void System::rel0(const Real &e, const RelType rel, const bool bp) {
    Real r = e.index(variablesEnclosures_, constantsEnclosures_);
    r.checkIndexed(globalDefinitions_);
    relationalFormulas_.push_back(RelExpr(r, rel, bp, defdx_, r.closed() ? 0 : nvars()));
    enclosureMethodForEachFormulaTrueBernsteinFalseInterval_.push_back(bp);
}

void System::evalGlobalDefinitions(Box &box) {
    for (nat f = 0; f < numberOfGlobalDefinitions(); ++f) {
        Interval X = globalDefinitions_[f].second.eval(box, globalDefinitionsEnclosures_, defaultEnclosureMethodTrueBernsteinFalseInterval_);
        globalDefinitionsEnclosures_.set(f, X);
    }
}

int System::evalSystem(Box &box) {
    evalGlobalDefinitions();
    int cert = 1;
    for (nat f = 0; f < numberOfRelationalFormulas(); ++f) {
        int c = evalNthFormulaOnBox(f,box);
        // if f-th formula is certainly not true, conjunction of formulas is certainly not true
        if (c == 0) return 0;
        if (c < 0) cert = (cert > 0 ? c : std::max(cert, c));
    }
    return cert;
}

int System::evalSystem(Box &box, Certainties &certs, const DirVars &dirvars) {
    evalGlobalDefinitions();
    if (certs.get() > 0) return certs.get();
    int cert = 1;
    for (nat f = 0; f < numberOfRelationalFormulas(); ++f) {
        // if f-th formula is certainly true, don't check it again
        if (certs.get(f) > 0) continue;
        if (!dirvars.empty() && relationalFormulas_[f].ope().closed() &&
                relationalFormulas_[f].ope().isFresh(dirvars.back().var) &&
                where(dirvars, dirvars.back().var) != EXTERIOR) {
            // top variable doesn't appear in f-th formula, don't check it again
            cert = (cert > 0 ? certs.get(f) : std::max(cert, certs.get(f)));
            continue;
        }
        certs.set(f, evalNthFormulaOnBox(f,box));
        // if f-th formula is certainly not true, conjunction of formulas is certainly not true
        if (certs.get(f) == 0)
            return certs.set(0);
        if (certs.get(f) < 0)
            cert = (cert > 0 ? certs.get(f) : std::max(cert, certs.get(f)));
    }
    return certs.set(cert);
}

void System::sosSystem(Interval &X, Certainties &certs, const Box &box, const nat v) {
    X = Interval(0);
    if (certs.get() == -1) {
        for (nat f = 0; f < numberOfRelationalFormulas(); ++f) {
            if (certs.get(f) == -1 && (relationalFormulas_[f].ope().open() || relationalFormulas_[f].ope().hasVar(v)))
                X += sqr(relationalFormulas_[f].ope().eval(box, globalDefinitionsEnclosures_));
        }
    }
}

void System::printSystem(std::ostream &os, const nat debug) const {
    os << "System: " << ID << std::endl;
    if (debug > 0) {
        os << "Tolerance: " << absoluteToleranceForStoppingBranchAndBound_ << std::endl;
        os << "Granularity: " << std::floor(100 * (granularityAkaDefaultResolutionForVariablesAsPercentageOfInitialBox_)) << "%" << std::endl;
        os << "Default Enclosure Method: " << (defaultEnclosureMethodTrueBernsteinFalseInterval_ ? "BP" : "IA") << std::endl;
    }
    os << "Vars: ";
    variablesEnclosures_.print(os);
    os << std::endl;
    if (debug > 0) {
        nat v;
        for (v = 0; v < variablesEnclosures_.size(); ++v)
            if (variableResolutionsAkaSmallestRangeConsideredForEachVariable_[v] != 0) break;
        if (v < variablesEnclosures_.size()) {
            os << "Resolutions: " << std::endl;
            os << "  " << variablesEnclosures_.name(v) << ": " << variableResolutionsAkaSmallestRangeConsideredForEachVariable_[v];
            for (++v; v < variablesEnclosures_.size(); ++v) {
                if (variableResolutionsAkaSmallestRangeConsideredForEachVariable_[v] != 0)
                    os << ", " << variablesEnclosures_.name(v) << ": " << variableResolutionsAkaSmallestRangeConsideredForEachVariable_[v];
            }
            os << std::endl;
        }
    }
    if (!constantsEnclosures_.empty()) {
        os << "Consts: ";
        constantsEnclosures_.print(os);
        os << std::endl;
    }
    if (!globalDefinitions_.empty()) {
        os << "Definitions: " << std::endl;
        for (nat f = 0; f < numberOfGlobalDefinitions(); ++f) {
            os << "  " << globalDefinitions_[f].first << " = " << globalDefinitions_[f]. second << std::endl;
        }
    }
    if (!relationalFormulas_.empty()) {
        os << "Constraints: " << std::endl;
        for (nat f = 0; f < numberOfRelationalFormulas(); ++f)
            os << "  " << ID << "_" << f << ": " << relationalFormulas_[f] << std::endl;
    }
    if (debug > 0) {
        os << "Partial derivatives: " << std::endl;
        for (nat f = 0; f < numberOfRelationalFormulas(); ++f) {
            for (nat v = 0; v < nvars(); ++v)
                os << "  d" << ID << "_" << f << "/d" << variablesEnclosures_.name(v)
                << " = " << relationalFormulas_[f].derivativeForVariable(v) << std::endl;
        }
    }
}

