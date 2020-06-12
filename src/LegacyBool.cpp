#include "LegacyBool.hpp"

using namespace kodiak;

RelExpr::RelExpr(const kodiak::Real &r, const kodiak::RelType rel, const bool poly, const Substitution &sigma,
                 const nat nvars) :
op_(rel) {
    ope_ = poly ? polynomial(r) : r;
    partialDerivativesPerVariable_.resize(std::max(nvars, r.numberOfVariables()));
    for (nat v = 0; v < partialDerivativesPerVariable_.size(); ++v) {
        partialDerivativesPerVariable_[v] = (poly ? polynomial(r.deriv(v)) : r.deriv(v)).subs(sigma);
    }
}

/*
 * RelExpr::eval
 *
 * return value:
 * > 0  : relation wrt 0 is certainly true
 * = 0  : relation wrt 0 is certainly false
 * = -1 : relation wrt 0 is possibly true
 * = -2 : relation wrt 0 is possibly true within eps (a positive small number).
 *
 * TODO: Refactor it into an enum type and update Paver::currentBoxCertainty_
 * instance variable
 */
int RelExpr::eval(Box &box, NamedBox &constbox, const bool enclosure, const real eps) {
    Interval i = ope_.eval(box, constbox, enclosure);
    int cly = i.rel0(op_, eps);
    if (cly >= 0) return cly;
    for (nat var = 0; var < ope_.numberOfVariables(); var++) {
        Interval initialVarEnclosure = box[var];
        try {
            Interval derivative = derivativeForVariable(var).eval(box, constbox, enclosure);
            bool isIncreasing = derivative.cge(0);
            bool isDecreasing = derivative.cle(0);
            if ((isIncreasing && (op_ == LE || op_ == LT))
                    || (isDecreasing && (op_ == GE || op_ == GT))) {
                box[var] = initialVarEnclosure.supremum();
                Interval sup = ope_.eval(box, constbox);
                int dly = sup.rel0(op_);
                if (dly > 0) {
                    box[var] = initialVarEnclosure;
                    return dly;
                }
                box[var] = initialVarEnclosure.infimum();
                Interval inf = ope_.eval(box, constbox);
                dly = inf.rel0(op_);
                if (dly == 0) {
                    box[var] = initialVarEnclosure;
                    return dly;
                }
            } else if ((isIncreasing && (op_ == GE || op_ == GT)) ||
                    (isDecreasing && (op_ == LE || op_ == LT))) {
                box[var] = initialVarEnclosure.infimum();
                Interval inf = ope_.eval(box, constbox);
                int dly = inf.rel0(op_);
                if (dly > 0) {
                    box[var] = initialVarEnclosure;
                    return dly;
                }
                box[var] = initialVarEnclosure.supremum();
                Interval sup = ope_.eval(box, constbox);
                dly = sup.rel0(op_);
                if (dly == 0) {
                    box[var] = initialVarEnclosure;
                    return dly;
                }
            } else   if ((isIncreasing || isDecreasing) && op_ == EQ) {
                box[var] = initialVarEnclosure.infimum();
                Interval inf = ope_.eval(box, constbox);
                if ((isIncreasing && inf.cgt(0)) || (isDecreasing && inf.clt(0))) {
                    box[var] = initialVarEnclosure;
                    return 0;
                }
                box[var] = initialVarEnclosure.supremum();
                Interval sup = ope_.eval(box, constbox);
                if ((isIncreasing && sup.clt(0)) || (isDecreasing && sup.cgt(0))) {
                    box[var] = initialVarEnclosure;
                    return 0;
                }
            }
        } catch (Growl growl) {
            if (Kodiak::debug()) {
                std::cout << "[GrowlException@RelExpr::eval]" << growl.what() << std::endl;
            }
        }
        box[var] = initialVarEnclosure;
    }
    return cly;
}

Real RelExpr::derivativeForVariable(const nat v) const {
    if (v < partialDerivativesPerVariable_.size())
        return partialDerivativesPerVariable_[v];
    return val(0);
}

void RelExpr::print(std::ostream &os) const {
    os << ope_ << " " << op_ << " 0";
}

std::ostream &kodiak::operator<<(std::ostream &os, const RelExpr &r) {
    r.print(os);
    return os;
}

std::ostream &kodiak::operator<<(std::ostream &os, const RelType rel) {
    switch (rel) {
        case EQ: os << "=";
            break;
        case LE: os << "<=";
            break;
        case LT: os << "<";
            break;
        case GE: os << ">=";
            break;
        case GT: os << ">";
            break;
    }
    return os;
}