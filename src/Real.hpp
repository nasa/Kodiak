#ifndef KODIAK_REAL
#define KODIAK_REAL

#include "types.hpp"

namespace kodiak {
    class Node;

    class Monomial;

    enum RealType {
        ATOM, MONOMEXPR, POLYNOMIAL, POLYEXPR, RATIONAL, REAL
    };

    class Real;

    typedef std::map<std::string, Real> Substitution;
    typedef std::vector<Monomial> Monomials;
    typedef std::vector< std::pair<std::string, Real> > Defs;

    extern const Substitution EmptySubstitution;

    class Real {
    public:
        Real(Node *);
        Real(const Real &);

        Real() : node_(NULL) {
        }
        Real& operator=(const Real &);

        bool operator==(const kodiak::Real &) const;

        bool notAReal() const {
            return node_ == NULL;
        }
        bool isVal() const;
        // is expression a variable? if Boolean parameter is set to true, then x^n is considered variable
        bool isVar(const bool = false) const;
        bool isName() const;
        // is expression a constant? if Boolean parameter is set to true, 
        // then an expression tha only involves constants is considered constant
        bool isConst(const bool = false) const;
        bool isUnary() const;
        bool isBinary() const;
        bool isOperator(const OpType) const;
        bool isPower() const;
        bool isLetin() const;
        bool isIfnz() const;
        bool isError() const;
        bool isFloor() const;
        bool isMax() const;
        bool isMin() const;
        bool isPolynomial() const;
        bool isRational() const;
        bool isAtom() const;
        bool isMonomExpr() const;
        bool isPolyExpr() const;
        bool isZero() const;
        bool isOne() const;
        bool isNegOne() const;

        OpType op() const; // Operator of unary, binary, or power expressions
        const Real &ope1() const; // First operand of unary, binary, or power expressions
        const Real &ope2() const; // Second operand of binary expressions
        const Real &ope3() const; // Third operand of ternary expressions
        const Interval &val() const; // Interval value of value expressions
        nat var() const; // Variable index of variable expressions
        nat expn() const; // Exponent of power expressions
        std::string name() const; // Name of named expressions
        const Monomials &monoms() const; // Monomials of POLYNOMIAL expressions
        const Tuple &degree() const; // Degree of POLYNOMIAL expressions
        const Names &names() const; // Names of variables in POLYNOMIAL expressions

        // Partial derivative with respect to given variable
        Real deriv(const nat = 0) const;
        // Index a real expression so names are replaced by indices and constant are replaced by their values
        // If boolean parameter is true, expand polynomial expressions into polynomials
        // (this option is useful when enclosures are computed using Bernstein polynomials)
        Real index(const NamedBox &, const NamedBox & = EmptyNamedBox) const;

        void checkIndexed() const {
            Defs defs;
            checkIndexed(defs);
        }
        void checkIndexed(Defs &) const;

        Interval eval(const Box &box = EmptyBox, const bool enclosure = false) const {
            NamedBox constbox;
            return eval(box, constbox, enclosure);
        }
        Interval eval(const Box &, NamedBox &, const bool = false) const;
        Interval eval(const NamedBox &, NamedBox &, const bool = false) const;

        Interval eval(const NamedBox &varbox, const bool enclosure = false) const {
            NamedBox constbox;
            return eval(varbox, constbox, enclosure);
        }

        Interval enclosure(const Box &box) const {
            return eval(box, true);
        }

        Interval enclosure(const NamedBox &varbox) const {
            return eval(varbox, true);
        }

        Interval enclosure(const NamedBox &varbox, NamedBox &constbox) const {
            return eval(varbox, constbox, true);
        }

        Real copy() const {
            Substitution sigma;
            return subs(sigma);
        }
        Real subs(const Substitution &, Names &) const;

        Real subs(const Substitution &sigma) const {
            Names names;
            return subs(sigma, names);
        }
        void print(std::ostream &) const;
        std::string toString() const;

        const VarBag &vars() const;
        VarBag &vars();

        bool isFresh(const nat v) const {
            return !vars().find(v);
        }

        bool hasVar(const nat v) const {
            return vars().find(v);
        }

        nat numberOfVariables() const;
        const NameSet &locals() const;
        NameSet &locals();
        const NameSet &consts() const;
        NameSet &consts();
        RealType type() const;

        bool closed() const {
            return locals().empty();
        } // Expression is closed, no local variables

        bool open() const {
            return !locals().empty();
        } // Expression is open, there are local variables
        ~Real();

    private:
        Node *node_;

    };

    extern const Real NotAReal;

    // Vector of Real expressions
    typedef std::vector<Real> Realn;

    // Symbolic expression representing a determinant of n x n
    Real det(const nat n, const std::string = "");

    // Symbolic expression representing the minor determinant of nxn matrix, where row r, column c are removed
    Real minor_det(const nat n, const nat, const nat, const std::string = "");

    // Symbolic coefficients of characteristic polynomial of degree n (coefficient k corresponds to degree k)
    void characteristic_coeffs(Realn &, const nat, const Substitution &, const std::string = "");

    // (i,j)-th coefficient of symbolic Hurwitz matrix of dimension n x n
    Real hurwitz_mat(const nat, const nat, const nat);

    // Hurwitz substitution for coefficients of a symbolic charateristic polynomial
    void hurwitz_subs(Substitution &, const nat, const std::string = "");

    // Symbolic hurwitz determinant i-th for coefficients of a charateristic polynomial of degree n
    // For 0 <= i <= n, x(i) is the coefficient of the power x^i of the characteristic polynomials
    // x(n+1) is the (i-1)-hurtwitz determinant 
    Real hurwitz_det(const Substitution &, const nat, const nat, const std::string = "");

    Real operator+(const Real &, const Real &);
    Real operator+(const Real &, const Real &&);
    Real operator+(const Real &&, const Real &);
    Real operator+(const Real &&, const Real &&);
    Real operator+(const Interval &, const Real &);
    Real operator+(const Real &, const Interval &);
    Real operator-(const Real &, const Real &);
    Real operator-(const Interval &, const Real &);
    Real operator-(const Real &, const Interval &);
    Real operator*(const Real &, const Real &);
    Real operator*(const Interval &, const Real &);
    Real operator*(const Real &, const Interval &);
    Real operator/(const Real &, const Real &);
    Real operator/(const Interval &, const Real &);
    Real operator/(const Real &, const Interval &);
    Real operator-(const Real &);
    Real operator^ (const Real &, const int);
    Real operator^ (const Interval &, const int);

    Real val(const Interval &);
    Real var(const nat, const std::string = "");
    Real var(const std::string);
    Real cnst(const std::string);
    Real polynomial(const Real &);
    Real Abs(const Real &);
    Real Abs(const Real &&);
    Real Abs(const Interval &);
    Real Sq(const Real &);
    Real Sq(const Interval &);
    Real Sqrt(const Real &);
    Real Sqrt(const Interval &);
    Real Ln(const Real &);
    Real Ln(const Interval &);
    Real Exp(const Real &);
    Real Exp(const Interval &);
    Real Sin(const Real &);
    Real Sin(const Interval &);
    Real Cos(const Real &);
    Real Cos(const Interval &);
    Real Tan(const Real &);
    Real Tan(const Interval &);
    Real Asin(const Real &);
    Real Asin(const Interval &);
    Real Acos(const Real &);
    Real Acos(const Interval &);
    Real Atan(const Real &);
    Real Atan(const Interval &);
    Real DFExp(const Real &);
    Real DFExp(const Interval &);
    Real DUlp(const Real &);
    Real DUlp(const Interval &);
    Real SUlp(const Real &);
    Real SUlp(const Interval &);
    Real let(const std::string, const Real &, const Real &);
    Real let(const std::string, const Interval &, const Real &);
    Real ifnz(const Real &, const Real &, const Real &);
    Real Max(std::initializer_list<const Real>);
    Real Max(std::vector<Real>&);
    Real Min(std::initializer_list<const Real>);
    Real Min(std::vector<Real>&);
    Real Error(const Real &, const Real &);
    Real Error(const Interval &, const Interval &);
    Real Floor(const Real &);
    Real Floor(const Interval &);

    // A Monomial is a coefficient (of type Real) and a tuple of natural numbers
    // representing the exponents of each variable, e.g., x1^3*x2^2 is represented
    // by the tuple <3,2>. The size of the tuple is the number of variables.

    class Monomial {
        friend bool operator==(Monomial const &,Monomial const &);
    public:
        // constructors/destructor

        Monomial() {
        }
        Monomial(const Real &, const Tuple &);
        // accessor functions

        void neg() {
            coeff_ = -coeff_;
        }

        void add(Real e, const int sign = 1) {
            coeff_ = coeff_ + sign*e;
        }

        nat nvars() const {
            return expo_.size();
        } // number of variables
        nat expo(const nat) const; // exponent of given variable

        const Real &coeff() const {
            return coeff_;
        } // coefficient
        // Do the two monomials have the same degree in every variable? 
        bool sameDegree(const Monomial &) const;
        bool isConst() const;
        // other functions
        Interval eval(const Box &, NamedBox &) const;
        Real realExpr(const Names & = EmptyNames) const;
        void print(std::ostream & = std::cout, const Names & = EmptyNames) const;

        const Tuple &exponents() const {
            return expo_;
        }

        Tuple &exponents() {
            return expo_;
        }

        const VarBag &vars() const {
            return vars_;
        }

        const NameSet &locals() const {
            return locals_;
        }

        const NameSet &consts() const {
            return consts_;
        }
    private:
        Real coeff_; // coefficient
        Tuple expo_; // exponents
        VarBag vars_; // set of variables in monomial
        NameSet locals_; // set of local variables
        NameSet consts_; // set of constants
    };

    bool operator==(Monomial const &,Monomial const &);

    Real rational(const Real &);
    Real poly_div(const Real &, const Real &);

    Real mk_poly(const Real &);
    void mk_expo(Tuple &, Names &, const Real &);
    Monomial mk_monom(Names &, const Real &);
    Monomial mult_monom(const Monomial &, const Monomial &);
    void push_monom(Monomials &, const Monomial &, const int = 1);
    void add_monoms(Monomials &, const Monomials &, const int = 1);
    nat definition(const Defs &, const std::string);



    std::ostream &operator<<(std::ostream &, const kodiak::Real &);
    std::ostream &operator<<(std::ostream &, const kodiak::RealType);
    std::ostream &operator<<(std::ostream &, const kodiak::Monomial &);
}
// Constant pi 
#define Pi val(Interval::PI())

// Euler constant
#define Ee val(exp(Interval::ONE()))


#endif // KODIAK_REAL

