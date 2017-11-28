#include <vector>

#include "Node.hpp"

using namespace kodiak;

const Substitution kodiak::EmptySubstitution;

// --- Real  ---

Real::Real(const Real &e) : node_(e.node_) {
    if (!e.notAReal())
        ++node_->use_;
}

Real::Real(Node *node) {
    if (node != NULL)
        ++node->use_;
    node_ = node;
}

Real kodiak::val(const Interval &i) {
    return Real(new RealVal_Node(i));
}

const VarBag &Real::vars() const {
    if (!notAReal())
        return node_->variableIndexes_;
    return EmptyVarBag;
}

VarBag &Real::vars() {
    if (!notAReal())
        return node_->variableIndexes_;
    throw Growl("Kodiak (vars): set of variables of NotAReal is undefined");
}

const NameSet &Real::locals() const {
    if (!notAReal())
        return node_->localVariables_;
    return EmptyNameSet;
}

const NameSet &Real::consts() const {
    if (!notAReal())
        return node_->globalConstants_;
    return EmptyNameSet;
}

NameSet &Real::consts() {
    if (!notAReal())
        return node_->globalConstants_;
    throw Growl("Kodiak (consts): set of constants of NotAReal is undefined");
}

RealType Real::type() const {
    if (!notAReal())
        return node_->realExpressionType_;
    throw Growl("Kodiak (type): type of NotAReal is undefined");
}

nat Real::numberOfVariables() const {
    if (!notAReal())
        return node_->numOfVariables;
    return 0;
}

Real &Real::operator=(const Real &e) {
    if (this != &e) {
        if (!e.notAReal())
            ++e.node_->use_;
        if (!notAReal()) {
            --node_->use_;
            if (node_->use_ == 0)
                delete node_;
        }
        node_ = e.node_;
    }
    return *this;
}

Real kodiak::operator+(const Real &e1, const Real &&e2) {
    return e1 + e2;
}
Real kodiak::operator+(const Real &&e1, const Real &e2) {
    return e1 + e2;
}
Real kodiak::operator+(const Real &&e1, const Real &&e2) {
    return e1 + e2;
}

Real kodiak::operator+(const Real &e1, const Real &e2) {
    if (e1.notAReal() || e2.isZero())
        return e1;
    if (e2.notAReal() || e1.isZero())
        return e2;
    if (e1.isVal() && e2.isVal())
        return val(e1.val() + e2.val());
    if (e2.isOperator(NEG))
        return e1 - e2.ope1();
    if (e1.isOperator(NEG) && !e2.isOperator(NEG))
        return e2 - e1.ope1();
    if (e1.isConst(true) && e2.isConst(true))
        return Real(new RealBinary_Node(ADD, e1, e2));
    if (e1.type() == POLYNOMIAL && e2.type() == POLYNOMIAL)
        return mk_poly(poly_add(e1, e2));
    if (e1.type() == POLYNOMIAL && e2.isMonomExpr())
        return mk_poly(poly_add(e1, poly_monomexpr(e2)));
    if (e1.isMonomExpr() && e2.type() == POLYNOMIAL)
        return mk_poly(poly_add(poly_monomexpr(e1), e2));
    if (e1.isMonomExpr() && e2.isMonomExpr())
        return mk_poly(poly_add(poly_monomexpr(e1), poly_monomexpr(e2)));
    return Real(new RealBinary_Node(ADD, e1, e2));
}

Real kodiak::operator+(const Interval &i1, const Real &e2) {
    return val(i1) + e2;
}

Real kodiak::operator+(const Real &e1, const Interval &i2) {
    return e1 + val(i2);
}

Real kodiak::operator-(const Real &e1, const Real &e2) {
    if (e1.notAReal() || e2.isZero())
        return e1;
    if (e2.notAReal())
        return e2;
    if (e1.isZero())
        return -e2;
    if (e1.isVal() && e2.isVal())
        return val(e1.val() - e2.val());
    if (e2.isOperator(NEG))
        return e1 + e2.ope1();
    if (e1.isConst(true) && e2.isConst(true))
        return Real(new RealBinary_Node(SUB, e1, e2));
    if (e1.type() == POLYNOMIAL && e2.type() == POLYNOMIAL)
        return mk_poly(poly_add(e1, e2, -1));
    if (e1.type() == POLYNOMIAL && e2.isMonomExpr())
        return mk_poly(poly_add(e1, poly_monomexpr(e2, -1)));
    if (e1.isMonomExpr() && e2.type() == POLYNOMIAL)
        return mk_poly(poly_add(poly_monomexpr(e1), e2, -1));
    if (e1.isMonomExpr() && e2.isMonomExpr())
        return mk_poly(poly_add(poly_monomexpr(e1), poly_monomexpr(e2, -1)));
    return Real(new RealBinary_Node(SUB, e1, e2));
}

Real kodiak::operator-(const Interval &i1, const Real &e2) {
    return val(i1) - e2;
}

Real kodiak::operator-(const Real &e1, const Interval &i2) {
    return e1 - val(i2);
}

Real kodiak::operator*(const Real &e1, const Real &e2) {
    if (e1.notAReal() || e2.isOne())
        return e1;
    if (e2.notAReal() || e1.isOne())
        return e2;
    if (e1.isZero() || e2.isZero())
        return val(Interval::ZERO());
    if (e1.isNegOne())
        return -e2;
    if (e1.isOperator(NEG) && e2.isOperator(NEG))
        return e1.ope1() * e2.ope1();
    if (e1.isOperator(NEG))
        return -(e1.ope1() * e2);
    if (e2.isOperator(NEG))
        return -(e1 * e2.ope1());
    if (e1.isVal() && e2.isVal())
        return val(e1.val() * e2.val());
    if (e2.isVal() || (e2.isConst(true) && !e1.isConst(true)))
        return e2 * e1;
    if (e2.isOperator(MULT) && e2.ope1().isConst(true))
        return (e1 * e2.ope1()) * e2.ope2();
    if (e1.isOperator(MULT) && !e1.ope2().isConst(true))
        return e1.ope1() * (e1.ope2() * e2);
    if (e1.isVar(true) && e2.isVar(true)) {
        if (e1.var() == e2.var()) {
            return var(e1.var(), e1.name()) ^ (e1.expn() + e2.expn());
        }
        if (e1.var() >= e2.var())
            return e2 * e1;
    }
    if (e1.isVar(true) && e2.isOperator(MULT) && e2.ope1().isVar(true)) {
        if (e1.var() == e2.ope1().var())
            return (var(e1.var(), e1.name()) ^ (e1.expn() + e2.ope1().expn())) * e2.ope2();
        if (e1.var() >= e2.ope1().var())
            return e2.ope1() * (e1 * e2.ope2());
    }
    if (e1.isOperator(DIV))
        return (e1.ope1() * e2) / e1.ope2();
    if (e2.isOperator(DIV))
        return (e1 * e2.ope1()) / e2.ope2();
    return Real(new RealBinary_Node(MULT, e1, e2));
}

Real kodiak::operator*(const Interval &i1, const Real &e2) {
    return val(i1) * e2;
}

Real kodiak::operator*(const Real &e1, const Interval &i2) {
    return val(i2) * e1;
}

Real kodiak::operator/(const Real &e1, const Real &e2) {
    if (e1.notAReal() || e2.isOne())
        return e1;
    if (e2.notAReal())
        return e2;
    if (e2.isVal()) {
        if (e2.val().contains(0))
            throw Growl("Kodiak (div): division by an interval that contains zero");
        if (e1.isVal())
            return val(e1.val() / e2.val());
        else
            return val(Interval::ONE() / e2.val()) * e1;
    }
    if (e1.isZero())
        return val(Interval::ZERO());
    if (e1.isOperator(DIV))
        return e1.ope1() / (e1.ope2() * e2);
    if (e2.isOperator(DIV))
        return (e1 * e2.ope2()) / e2.ope1();
    if (e1.isVar(true) && e2.isVar(true) && e1.var() == e2.var())
        return var(e1.var(), e1.name())^(e1.expn() - e2.expn());
    // added code for rationals
    // only potentially create a rational if there is a variable which
    // appears in both the numerator and denominator
    if (e1.vars().is_intersection_nonempty(e2.vars())) {
        if (e1.type() == POLYNOMIAL && e2.type() == POLYNOMIAL)
            return rational(poly_div(e1, e2));
        if (e1.type() == POLYNOMIAL && e2.isMonomExpr())
            return rational(poly_div(e1, poly_monomexpr(e2)));
        if (e1.isMonomExpr() && e2.type() == POLYNOMIAL)
            return rational(poly_div(poly_monomexpr(e1), e2));
        if (e1.isMonomExpr() && e2.isMonomExpr())
            return rational(poly_div(poly_monomexpr(e1), poly_monomexpr(e2)));
    }
    // end of added code
    return Real(new RealBinary_Node(DIV, e1, e2));
}

Real kodiak::operator/(const Interval &i1, const Real &e2) {
    return val(i1) / e2;
}

Real kodiak::operator/(const Real &e1, const Interval &i2) {
    return e1 / val(i2);
}

Real kodiak::operator-(const Real &e) {
    if (e.notAReal() || e.isZero())
        return e;
    if (e.isOperator(NEG))
        return e.ope1();
    if (e.isVal())
        return val(-e.val());
    return Real(new RealUnary_Node(NEG, e));
}

Real kodiak::operator^ (const Real &e, const int n) {
    if (e.notAReal()) return e;
    if (n == 0)
        return val(Interval::ONE());
    if (e.isZero() || n == 1)
        return e;
    if (e.isVal()) {
        return val(power(e.val(), n));
    }
    if (e.isOperator(NEG)) {
        if (n % 2 == 0)
            return e.ope1()^n;
        else
            return -(e.ope1()^n);
    }
    if (e.isPower())
        return e.ope1() ^ (n * e.expn());
    if (e.isOperator(MULT) && (e.ope1().isVal() || e.isMonomExpr()))
        return (e.ope1()^n) * (e.ope2()^n);
    if (e.isOperator(DIV) && e.ope1().isVal())
        return (e.ope1()^n) / (e.ope2()^n);
    if (n < 0)
        return 1 / (e ^ (-n));
    return Real(new RealPower_Node(e, n));
}

Real kodiak::operator^ (const Interval &i, const int n) {
    return val(i)^n;
}

const Real kodiak::NotAReal;

// Symbolic expression representing a determinant of n x n matrix

Real kodiak::det(const nat n, const std::string s) {
    Permutation p(n);
    Real sum = val(0);
    while (p.next()) {
        Real prod = val(1);
        for (nat i = 0; i < n; ++i) {
            std::ostringstream os;
            os << s << "[" << i << "," << p[i] << "]";
            prod = prod * cnst(os.str());
        }
        sum = sum + p.sign() * prod;
    }
    return sum;
}

// Symbolic expression representing the minor determinant of nxn matrix, where row r, column c are removed

Real kodiak::minor_det(const nat n, const nat r, const nat c, const std::string s) {
    Permutation p(n - 1);
    Real sum = val(0);
    while (p.next()) {
        Real prod = val(1);
        for (nat i = 0; i < n - 1; ++i) {
            std::ostringstream os;
            nat k = (i < r ? i : i + 1);
            nat l = (p[i] < c ? p[i] : p[i] + 1);
            os << s << "[" << k << "," << l << "]";
            prod = prod * cnst(os.str());
        }
        sum = sum + p.sign() * prod;
    }
    if (r < n && c < n && ((r + c) % 2 != 0))
        return -sum;
    return sum;
}

// Symbolic coefficients of characteristic polynomial of degree n (coefficient k corresponds to degree k)

void kodiak::characteristic_coeffs(Realn &coeffs, const nat n, const Substitution &jacobian, const std::string s) {
    Substitution sigma;
    for (nat i = 0; i < n; ++i) {
        for (nat j = 0; j < n; ++j) {
            std::ostringstream os;
            os << s << "[" << i << "," << j << "]";
            if (i == j)
                sigma[os.str()] = var(0) - cnst(os.str());
            else
                sigma[os.str()] = -cnst(os.str());
        }
    }
    Real e = det(n, s);
    Real p = polynomial(e.subs(sigma));
    Monomials monoms = p.monoms();
    coeffs.assign(n + 1, val(0));
    for (nat i = 0; i < monoms.size(); i++)
        coeffs[monoms[i].expo(0)] = monoms[i].coeff().subs(jacobian);
}

// (i,j)-th coefficient of symbolic Hurwitz matrix of dimension n x n

Real kodiak::hurwitz_mat(const nat n, const nat i, const nat j) {
    nat k = 2 * (i + 1);
    if (j >= k || (k - j - 1) > n)
        return val(0);
    return var(n - k + j + 1);
}

// Hurwitz substitution for coefficients of a charateristic polynomial

void kodiak::hurwitz_subs(Substitution &sigma, const nat n, const std::string s) {
    // assert(n > 0);
    for (nat i = 0; i < n; ++i) {
        for (nat j = 0; j < n; ++j) {
            std::ostringstream os;
            os << s << "[" << i << "," << j << "]";
            sigma[os.str()] = hurwitz_mat(n, i, j);
        }
    }
}

// Symbolic hurwitz determinant i-th for coefficients of a charateristic polynomial of degree n
// For 0 <= i <= n, var(i) is the coefficient of the power x^i of the characteristic polynomials
// var(n+1) is the (i-1)-hurtwitz determinant

Real kodiak::hurwitz_det(const Substitution &sigma, const nat n, const nat i, const std::string s) {
    // assert(n > 0 && i < n);
    if (i == 0)
        return var(n - 1);
    Real d = val(0);
    for (nat j = 0; j < i; ++j)
        d = d + hurwitz_mat(n, i, j) * (minor_det(i + 1, i, j, s).subs(sigma));
    std::ostringstream os;
    os << s << i - 1;
    d = d + hurwitz_mat(n, i, i) * var(n + 1, os.str());
    return d;
}

Real kodiak::var(const nat v, const std::string name) {
    return Real(new RealVar_Node(v, name));
}

Real kodiak::var(const std::string name) {
    return Real(new RealName_Node(name));
}

Real kodiak::cnst(const std::string name) {
    return Real(new RealName_Node(name, false));
}

// Polynomial expressions are translated into polynomials

Real kodiak::polynomial(const Real &e) {
    if (e.vars().linear() || e.isPolynomial())
        return e;
    else if (e.isPolyExpr())
        return mk_poly(poly_polyexpr(e));
    else if (e.isOperator(NEG))
        return -polynomial(e.ope1());
    else if (e.isOperator(ABS))
        return Abs(polynomial(e.ope1()));
    else if (e.isOperator(SQ))
        return Sq(polynomial(e.ope1()));
    else if (e.isOperator(SQRT))
        return Sqrt(polynomial(e.ope1()));
    else if (e.isOperator(EXP))
        return Exp(polynomial(e.ope1()));
    else if (e.isOperator(LN))
        return Ln(polynomial(e.ope1()));
    else if (e.isOperator(SIN))
        return Sin(polynomial(e.ope1()));
    else if (e.isOperator(COS))
        return Cos(polynomial(e.ope1()));
    else if (e.isOperator(TAN))
        return Tan(polynomial(e.ope1()));
    else if (e.isOperator(ASIN))
        return Asin(polynomial(e.ope1()));
    else if (e.isOperator(ACOS))
        return Acos(polynomial(e.ope1()));
    else if (e.isOperator(ATAN))
        return Atan(polynomial(e.ope1()));
    else if (e.isOperator(ADD))
        return polynomial(e.ope1()) + polynomial(e.ope2());
    else if (e.isOperator(SUB))
        return polynomial(e.ope1()) - polynomial(e.ope2());
    else if (e.isOperator(MULT))
        return polynomial(e.ope1()) * polynomial(e.ope2());
    else if (e.isOperator(DIV))
        return polynomial(e.ope1()) / polynomial(e.ope2());
    else if (e.isPower())
        return polynomial(e.ope1())^e.expn();
    else if (e.isLetin())
        return let(e.name(), polynomial(e.ope1()), polynomial(e.ope2()));
    else if (e.isIfnz())
        return ifnz(polynomial(e.ope1()),
            polynomial(e.ope2()), polynomial(e.ope3()));
    return e;
}

Real kodiak::Abs(const Real &&e) {
    if (e.notAReal())
        return e;
    if (e.isVal()) {
        if (e.isZero())
            return val(Interval::ZERO());
        if (e.isOne())
            return val(Interval::ONE());
        return val(abs(e.val()));
    }
    return Real(new RealUnary_Node(ABS, e));
}

Real kodiak::Abs(const Real &e) {
    if (e.notAReal())
        return e;
    if (e.isVal()) {
        if (e.isZero())
            return val(Interval::ZERO());
        if (e.isOne())
            return val(Interval::ONE());
        return val(abs(e.val()));
    }
    return Real(new RealUnary_Node(ABS, e));
}

Real kodiak::Abs(const Interval &i) {
    return Abs(val(i));
}

Real kodiak::Sq(const Real &e) {
    if (e.notAReal())
        return e;
    if (e.isVal()) {
        if (e.isZero())
            return val(Interval::ZERO());
        if (e.isOne())
            return val(Interval::ONE());
        return val(sqr(e.val()));
    }
    if (e.isMonomExpr())
        return e ^ 2;
    return Real(new RealUnary_Node(SQ, e));
}

Real kodiak::Sq(const Interval &i) {
    return Sq(val(i));
}

Real kodiak::Sqrt(const Real &e) {
    if (e.notAReal())
        return e;
    if (e.isVal()) {
        if (e.val().plt(0))
            throw Growl("Kodiak (sqrt): sqrt expects a nonnegative interval");
        if (e.isZero())
            return val(Interval::ZERO());
        if (e.isOne())
            return val(Interval::ONE());
        return val(sqrt(e.val()));
    }
    if (e.isOperator(SQ))
        return Abs(e.ope1());
    if (e.isOperator(POW) && e.expn() == 2)
        return Abs(e.ope1());
    return Real(new RealUnary_Node(SQRT, e));
}

Real kodiak::Sqrt(const Interval &i) {
    return Sqrt(val(i));
}

Real kodiak::Ln(const Real &e) {
    if (e.notAReal())
        return e;
    if (e.isVal()) {
        if (e.val().ple(0))
            throw Growl("Kodiak (ln): ln expects a positive interval");
        if (e.isOne())
            return val(Interval::ZERO());
        return val(log(e.val()));
    }
    return Real(new RealUnary_Node(LN, e));
}

Real kodiak::Ln(const Interval &i) {
    return Ln(val(i));
}

Real kodiak::Exp(const Real &e) {
    if (e.notAReal())
        return e;
    if (e.isVal()) {
        if (e.isZero())
            return val(Interval::ONE());
        return val(exp(e.val()));
    }
    return Real(new RealUnary_Node(EXP, e));
}

Real kodiak::Exp(const Interval &i) {
    return Exp(val(i));
}

Real kodiak::Sin(const Real &e) {
    if (e.notAReal())
        return e;
    if (e.isVal()) {
        if (e.isZero())
            return val(Interval::ZERO());
        return val(sin(e.val()));
    }
    return Real(new RealUnary_Node(SIN, e));
}

Real kodiak::Sin(const Interval &i) {
    return Sin(val(i));
}

Real kodiak::Cos(const Real &e) {
    if (e.notAReal())
        return e;
    if (e.isVal()) {
        if (e.isZero())
            return val(Interval::ONE());
        return val(cos(e.val()));
    }
    return Real(new RealUnary_Node(COS, e));
}

Real kodiak::Cos(const Interval &i) {
    return Cos(val(i));
}

Real kodiak::Tan(const Real &e) {
    if (e.notAReal())
        return e;
    if (e.isVal()) {
        if (e.isZero())
            return val(Interval::ZERO());
        if (!(e.val().cgt(-Interval::PI() / Interval(2)) &&
                e.val().clt(Interval::PI() / Interval(2))))
            throw Growl("Kodiak (tan): tan expects a proper interval in [-pi/2,pi/2]");
        return val(tan(e.val()));
    }
    return Real(new RealUnary_Node(TAN, e));
}

Real kodiak::Tan(const Interval &i) {
    return Tan(val(i));
}

Real kodiak::Asin(const Real &e) {
    if (e.notAReal())
        return e;
    if (e.isVal()) {
        if (!e.val().subset(Interval(-1, 1)))
            throw Growl("Kodiak (asin): asin expects an interval in [-1,1]");
        if (e.isZero())
            return val(Interval::ZERO());
        return val(asin(e.val()));
    }
    return Real(new RealUnary_Node(ASIN, e));
}

Real kodiak::Asin(const Interval &i) {
    return Asin(val(i));
}

Real kodiak::Acos(const Real &e) {
    if (e.notAReal())
        return e;
    if (e.isVal()) {
        if (!e.val().subset(Interval(-1, 1)))
            throw Growl("Kodiak (acos): acos expects an interval in [-1,1]");
        if (e.isOne())
            return val(Interval::ZERO());
        return val(acos(e.val()));
    }
    return Real(new RealUnary_Node(ACOS, e));
}

Real kodiak::Acos(const Interval &i) {
    return Acos(val(i));
}

Real kodiak::Atan(const Real &e) {
    if (e.notAReal())
        return e;
    if (e.isVal()) {
        if (e.isZero())
            return val(Interval::ZERO());
        return val(atan(e.val()));
    }
    return Real(new RealUnary_Node(ATAN, e));
    ;
}

Real kodiak::Atan(const Interval &i) {
    return Atan(val(i));
}

Real kodiak::DFExp(const Real &e) {
    if (e.notAReal())
        return e;
    return Real(new RealUnary_Node(DOUBLE_FEXP, e));
}

Real kodiak::DFExp(const Interval &i) {
    return DFExp(val(i));
}

Real kodiak::DUlp(const Real &e) {
    if (e.notAReal())
        return e;
    return Real(new RealUnary_Node(DOUBLE_ULP, e));
}

Real kodiak::DUlp(const Interval &i) {
    return DUlp(val(i));
}

Real kodiak::SUlp(const Real &e) {
    if (e.notAReal())
        return e;
    return Real(new RealUnary_Node(SINGLE_ULP, e));
}

Real kodiak::SUlp(const Interval &i) {
    return SUlp(val(i));
}

Real kodiak::let(const std::string name, const Real &let, const Real &in) {
    if (in.notAReal())
        return in;
    if (in.consts().find(name) != in.consts().end()) {
        std::ostringstream os;
        os << "Kodiak (let): name \"" << name << "\" occurs as variable and constant name";
        throw Growl(os.str());
    }
    if (in.locals().find(name) == in.locals().end())
        return in;
    if (let.notAReal())
        return let;
    if (let.isVal()) {
        Substitution sigma;
        sigma[name] = let;
        return in.subs(sigma);
    }
    return Real(new RealLetin_Node(name, let, in));
    ;
}

Real kodiak::let(const std::string name, const Interval &let, const Real &in) {
    return kodiak::let(name, val(let), in);
}

Real kodiak::ifnz(const Real &cond, const Real &lt, const Real &gt) {
    if (cond.notAReal())
        return cond;
    if (cond.isVal()) {
        if (cond.val().clt(0))
            return lt;
        if (cond.val().cgt(0))
            return gt;
        throw Growl("Kodiak (ifnz): ifnz cannot be evaluated when the first argument is an interval containing zero");
    }
    return Real(new RealIfnz_Node(cond, lt, gt));
}

Real kodiak::Max(std::initializer_list<const Real> listOfReals) {
    if (listOfReals.size() <= 0) {
        throw Growl("Kodiak (Max): needs at least one argument");
    }

    std::vector<Real> vectorOfReals;
    for (auto r : listOfReals) {
        if (r.notAReal()) {
            throw Growl("Kodiak (Max): one of the arguments is not a Real");
        }
        vectorOfReals.push_back(r);
    }
    return Real(new Max_Node(vectorOfReals));
}

Real kodiak::Max(std::vector<Real> &listOfReals) {
    if (listOfReals.size() <= 0) {
        throw Growl("Kodiak (Max): needs at least one argument");
    }
//
    std::vector<Real> vectorOfReals;
    for (auto r : listOfReals) {
        if (r.notAReal()) {
            throw Growl("Kodiak (Max): one of the arguments is not a Real");
        }
        vectorOfReals.push_back(r);
    }
    return Real(new Max_Node(vectorOfReals));
}

Real kodiak::Min(std::initializer_list<const Real> listOfReals) {
    if (listOfReals.size() <= 0) {
        throw Growl("Kodiak (Min): needs at least one argument");
    }

    std::vector<Real> vectorOfReals;
    for (auto r : listOfReals) {
        if (r.notAReal()) {
            throw Growl("Kodiak (Min): one of the arguments is not a Real");
        }
        vectorOfReals.push_back(r);
    }
    return Real(new Min_Node(vectorOfReals));
}

Real kodiak::Min(std::vector<Real> &listOfReals) {
    if (listOfReals.size() <= 0) {
        throw Growl("Kodiak (Min): needs at least one argument");
    }

    std::vector<Real> vectorOfReals;
    for (auto r : listOfReals) {
        if (r.notAReal()) {
            throw Growl("Kodiak (Min): one of the arguments is not a Real");
        }
        vectorOfReals.push_back(r);
    }
    return Real(new Min_Node(vectorOfReals));
}

Real kodiak::Error(const Real &value, const Real &error) {
    return Real(new Error_Node(value,error));
}

Real kodiak::Error(const Interval &value, const Interval &error) {
    return Error(val(value), val(error));
}

Real kodiak::Floor(const Real &e) {
    return Real(new Floor_Node(e));
}

Real kodiak::Floor(const Interval &i) {
    return Floor(val(i));
}

bool Real::isVal() const {
    return !notAReal() && node_->isVal();
}

// If pow is true, check is if var or a power of var

bool Real::isVar(bool pow) const {
    return !notAReal() &&
            (node_->isVar() || (pow && node_->isPower() && ope1().isVar()));
}

bool Real::isName() const {
    return !notAReal() && node_->isName();
}

bool Real::isConst(const bool rec) const {
    return !notAReal() &&
            (node_->isVal() || node_->isName() || (rec && vars().empty()));
}

bool Real::isUnary() const {
    return !notAReal() && node_->isUnary();
}

bool Real::isBinary() const {
    return !notAReal() && node_->isBinary();
}

bool Real::isPower() const {
    return !notAReal() && node_->isPower();
}

bool Real::isOperator(const OpType o) const {
    return (isUnary() || isBinary() || isPower()) && op() == o;
}

bool Real::isLetin() const {
    return !notAReal() && node_->isLetin();
}

bool Real::isIfnz() const {
    return !notAReal() && node_->isIfnz();
}

bool Real::isError() const {
    return !notAReal() && node_->isError();
}

bool Real::isFloor() const {
    return !notAReal() && node_->isFloor();
}

bool Real::isMax() const {
    return !notAReal() && node_->isMax();
}

bool Real::isMin() const {
    return !notAReal() && node_->isMin();
}

bool Real::isPolynomial() const {
    return !notAReal() && node_->isPolynomial();
}

bool Real::isRational() const {
    return !notAReal() && node_->isRational();
}

bool Real::isAtom() const {
    return !notAReal() &&
            node_->realExpressionType_ == ATOM;
}

bool Real::isMonomExpr() const {
    return !notAReal() &&
            node_->realExpressionType_ <= MONOMEXPR;
}

bool Real::isPolyExpr() const {
    return !notAReal() &&
            node_->realExpressionType_ <= POLYEXPR;
}

bool Real::isZero() const {
    return isVal() &&
            ((RealVal_Node *) node_)->val_.isPoint() &&
            ((RealVal_Node *) node_)->val_.inf() == 0;
}

bool Real::isOne() const {
    return isVal() &&
            ((RealVal_Node *) node_)->val_.isPoint() &&
            ((RealVal_Node *) node_)->val_.inf() == 1;
}

bool Real::isNegOne() const {
    return isVal() &&
            ((RealVal_Node *) node_)->val_.isPoint() &&
            ((RealVal_Node *) node_)->val_.inf() == -1;
}

OpType Real::op() const {
    if (isUnary())
        return ((RealUnary_Node *) node_)->op_;
    if (isBinary())
        return ((RealBinary_Node *) node_)->op_;
    if (isPower())
        return POW;
    std::ostringstream os;
    os << "Kodiak (op): operator of " << *this << "is undefined";
    throw Growl(os.str());
}

const Real &Real::ope1() const {
    if (isPower())
        return ((RealPower_Node *) node_)->ope_;
    if (isUnary())
        return ((RealUnary_Node *) node_)->ope_;
    if (isBinary())
        return ((RealBinary_Node *) node_)->ope1_;
    if (isLetin())
        return ((RealLetin_Node *) node_)->let_;
    if (isIfnz())
        return ((RealIfnz_Node *) node_)->cond_;
    std::ostringstream os;
    os << "Kodiak (ope1): firt operand of " << *this << " is undefined";
    throw Growl(os.str());
}

const Real &Real::ope2() const {
    if (isBinary())
        return ((RealBinary_Node *) node_)->ope2_;
    if (isLetin())
        return ((RealLetin_Node *) node_)->in_;
    if (isIfnz())
        return ((RealIfnz_Node *) node_)->lt_;
    std::ostringstream os;
    os << "Kodiak (ope2): second operand of " << *this << " is undefined";
    throw Growl(os.str());
}

const Real &Real::ope3() const {
    if (isIfnz())
        return ((RealIfnz_Node *) node_)->gt_;
    std::ostringstream os;
    os << "Kodiak (ope3): third operand of " << *this << " is undefined";
    throw Growl(os.str());
}

const Interval &Real::val() const {
    if (isVal())
        return ((RealVal_Node *) node_)->val_;
    std::ostringstream os;
    os << "Kodiak (val): interval value of " << *this << " is undefined";
    throw Growl(os.str());
}

nat Real::var() const {
    if (isVar())
        return ((RealVar_Node *) node_)->var_;
    if (isVar(true))
        return ope1().var();
    std::ostringstream os;
    os << "Kodiak (vars): variable index of " << *this << " is undefined";
    throw Growl(os.str());
}

nat Real::expn() const {
    if (isVar())
        return 1;
    if (isPower())
        return ((RealPower_Node *) node_)->n_;
    std::ostringstream os;
    os << "Kodiak (expn): exponent of " << *this << " is undefined";
    throw Growl(os.str());
}

std::string Real::name() const {
    if (isName())
        return ((RealName_Node *) node_)->name_;
    if (isVar())
        return ((RealVar_Node *) node_)->name();
    if (isVar(true))
        return ope1().name();
    if (isLetin())
        return ((RealLetin_Node *) node_)->name_;
    std::ostringstream os;
    os << "Kodiak (name): name of " << *this << " is undefined";
    throw Growl(os.str());
}

const Monomials &Real::monoms() const {
    if (isPolynomial())
        return ((Polynomial_Node *) node_)->monoms_;
    std::ostringstream os;
    os << "Kodiak (monoms): monomials of " << *this << " is undefined";
    throw Growl(os.str());
}

const Tuple &Real::degree() const {
    if (isPolynomial())
        return ((Polynomial_Node *) node_)->degs_;
    std::ostringstream os;
    os << "Kodiak (degree): degree of " << *this << " is undefined";
    throw Growl(os.str());
}

const Names &Real::names() const {
    if (isPolynomial())
        return ((Polynomial_Node *) node_)->names_;
    std::ostringstream os;
    os << "Kodiak (names): names of " << *this << " is undefined";
    throw Growl(os.str());
}

// Partial derivative with respect to var

Real Real::deriv(const nat v) const {
    if (notAReal())
        return NotAReal;
    else if (open() || hasVar(v)) {
        return node_->deriv(v);
    }
    return kodiak::val(Interval::ZERO());
}

// Index a real expression so names are replaced by indices and constant are replaced by their values

Real Real::index(const NamedBox &varbox, const NamedBox &constbox) const {
    if (!notAReal()) {
        Substitution sigma;
        if (!locals().empty()) {
            for (nat v = 0; v < varbox.size(); ++v)
                sigma[varbox.names()[v]] = kodiak::var(v, varbox.name(v));
        }
        if (!consts().empty()) {
            for (nat c = 0; c < constbox.size(); ++c)
                sigma[constbox.names()[c]] = kodiak::val(constbox.val(c));
        }
        return subs(sigma);
    }
    throw Growl("Kodiak (index): NotAReal cannot be indexed");
}

void Real::checkIndexed(Defs &defs) const {
    if (!consts().empty() || !locals().empty()) {
        std::ostringstream os;
        NameSet c = consts();
        for (NameSet::iterator it = locals().begin(); it != locals().end(); ++it) {
            if (definition(defs, (*it)) >= defs.size())
                c.insert(*it);
        }
        if (!c.empty()) {
            os << "Kodiak: (checkIndexed) Expression " << (*this)
                    << " contains constants " << c << " that need to be indexed";
            throw Growl(os.str());
        }
    }
}

Interval Real::eval(const Box &box, NamedBox &constbox, const bool enclosure) const {
    if (!notAReal())
        return node_->eval(box, constbox, !vars().linear() && enclosure);
    throw Growl("Kodiak (eval): evaluation of NotAReal is undefined");
}

Interval Real::eval(const NamedBox &varbox, NamedBox &constbox, const bool enclosure) const {
    if (!notAReal())
        return index(varbox, constbox).eval(varbox.box(), constbox, enclosure);
    throw Growl("Kodiak (eval): evaluation of NotAReal is undefined");
}

Real Real::subs(const Substitution &sigma, Names &names) const {
    if (!notAReal()) {
        return node_->subs(sigma, names);
    }
    return NotAReal;
}

void Real::print(std::ostream &os) const {
    if (!notAReal())
        node_->print(os);
    else
        os << "?";
}

Real::~Real() {
    if (!notAReal()) {
        --node_->use_;
        if (node_->use_ <= 0)
            delete node_;
    }
}

// --- Monomial ---

Monomial::Monomial(const Real &coeff, const Tuple &expo) {
    coeff_ = coeff;
    locals_ = coeff.locals();
    consts_ = coeff.consts();
    expo_.resize(expo.size());
    for (nat v = 0; v < expo.size(); ++v) {
        expo_[v] = expo[v];
        if (expo_[v] > 0)
            vars_.insert(v);
    }
}

nat Monomial::expo(const nat v) const // exponent of a specific variable
{
    if (v < nvars())
        return expo_[v];
    return 0;
}

bool Monomial::sameDegree(const Monomial &monom) const {
    // Do the two monomials have the same degree in every variable?
    bool same = true;
    for (nat v = 0; same && v < nvars(); ++v) {
        same = expo(v) == monom.expo(v);
    }
    for (nat v = nvars(); same && v < monom.nvars(); ++v)
        same = expo(v) == monom.expo(v);
    return same;
}

bool Monomial::isConst() const {
    for (nat v = 0; v < nvars(); ++v)
        if (expo(v) > 0) return false;
    return true;
}

Interval Monomial::eval(const Box &varbox, NamedBox &constbox) const {
    Interval X = coeff_.eval(varbox, constbox);
    for (nat v = 0; v < nvars(); ++v) {
        if (expo_[v] > 0) X *= power(varbox[v], expo_[v]);
    }
    return X;
}

Real Monomial::realExpr(const Names &names) const {
    Real e = coeff_;
    for (nat v = 0; v < nvars(); ++v) {
        Real vv = v < names.size() ? var(v, names[v]) : var(v);
        if (expo_[v] > 0) e = e * (vv ^ expo_[v]);
    }
    return e;
}

void Monomial::print(std::ostream &os, const Names &names) const {
    bool nio = !coeff_.isOne();
    bool iz = isZero(expo_);
    if (nio || iz)
        os << coeff_;
    if (!iz) {
        for (nat v = 0; v < expo_.size(); ++v) {
            if (expo_[v] > 0) {
                if (nio)
                    os << "*";
                else
                    nio = true;
                os << (Kodiak::debug() || v >= names.size() ?
                        var_name(v) : names[v]);
                if (expo_[v] > 1) os << "^" << expo_[v];
            }
        }
    }
}

Real kodiak::mk_poly(const Real &e) {
    if (e.type() != POLYNOMIAL)
        return e;
    if (e.monoms().empty())
        return val(0);
    if (e.monoms().size() == 1)
        return e.monoms()[0].realExpr(e.names());
    return e;
}

void kodiak::mk_expo(Tuple &expo, Names &names, const Real &e) {
    if (e.isVar(true)) {
        if (e.var() >= expo.size()) {
            nat n = e.var() - expo.size() + 1;
            expo.insert(expo.end(), n, 0);
            names.insert(names.end(), n, "");
        }
        expo[e.var()] = e.expn();
        if (names[e.var()] != "") {
            if (e.name() != "" && names[e.var()] != e.name()) {
                std::ostringstream os;
                os << "Kodiak (mk_expo): variable " << var_name(e.var()) << " has two different names, i.e, "
                        << names[e.var()] << " and " << e.name() << std::endl;
                throw Growl(os.str());
            }
        } else
            names[e.var()] = e.name();
    } else if (e.isOperator(MULT)) {
        mk_expo(expo, names, e.ope1());
        mk_expo(expo, names, e.ope2());
    }
}

Monomial kodiak::mk_monom(Names &names, const Real &e) {
    Tuple expo;
    expo.assign(names.size(), 0);
    if (e.isConst(true)) {
        return Monomial(e, expo);
    } else if (e.isVar(true)) {
        mk_expo(expo, names, e);
        return Monomial(val(1), expo);
    } else if (e.isOperator(NEG)) {
        mk_expo(expo, names, e.ope1());
        return Monomial(val(-1), expo);
    } else if (e.isOperator(MULT)) {
        if (e.ope1().isConst(true)) {
            mk_expo(expo, names, e.ope2());
            return Monomial(e.ope1(), expo);
        }
        mk_expo(expo, names, e);
        return Monomial(val(1), expo);
    }
    return Monomial();
}

Monomial kodiak::mult_monom(const Monomial &monom1, const Monomial &monom2) {
    Tuple expo;
    nat n = std::max(monom1.nvars(), monom2.nvars());
    expo.assign(n, 0);
    for (nat i = 0; i < n; ++i)
        expo[i] = monom1.expo(i) + monom2.expo(i);
    return Monomial(monom1.coeff() * monom2.coeff(), expo);
}

void kodiak::push_monom(Monomials &monoms, const Monomial &monom, const int sign) {
    for (Monomials::iterator it = monoms.begin(); it != monoms.end(); ++it) {
        if (monom.sameDegree(*it)) {
            (*it).add(monom.coeff(), sign);
            if ((*it).coeff().isZero())
                monoms.erase(it);
            return;
        }
    }
    monoms.push_back(monom);
    if (sign < 0)
        monoms.back().neg();
}

void kodiak::add_monoms(Monomials &monoms1, const Monomials &monoms2, const int sign) {
    for (nat i = 0; i < monoms2.size(); ++i)
        push_monom(monoms1, monoms2[i], sign);
}

nat kodiak::definition(const Defs &defs, const std::string id) {
    nat f;
    for (f = 0; f < defs.size(); ++f) {
        if (defs[f].first == id) return f;
    }
    return f;
}

// Other functions

std::ostream &kodiak::operator<<(std::ostream &os, const Real &e) {
    e.print(os);
    return os;
}

std::ostream &kodiak::operator<<(std::ostream &os, const RealType t) {
    switch (t) {
        case ATOM: os << "ATOM";
            break;
        case MONOMEXPR: os << "MONOMEXPR";
            break;
        case POLYNOMIAL: os << "POLYNOMIAL";
            break;
        case POLYEXPR: os << "POLYEXPR";
            break;
        case RATIONAL: os << "RATIONAL";
            break;
        case REAL: os << "REAL";
            break;
    }
    return os;
}

std::ostream &kodiak::operator<<(std::ostream &os, const Monomial &monom) {
    monom.print(os);
    return os;
}

bool Real::operator==(const Real &anotherReal) const {
    if (this->notAReal() || anotherReal.notAReal()) {
        throw Growl("Kodiak (==): trying to compare NotAReal");
    }
    if (this->isName() && anotherReal.isName() && this->name() == anotherReal.name()) {
        return true;
    }
    if (this->node_ == anotherReal.node_) {
        return true;
    }
    return *this->node_ == *anotherReal.node_;
}
