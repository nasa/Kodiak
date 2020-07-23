#ifndef KODIAK_NODE
#define KODIAK_NODE

#include "Real.hpp"

#include <typeinfo>

namespace kodiak {

    class Max_Node;
    class Min_Node;
    class Error_Node;
    class Floor_Node;

    class Node {
        friend class Real;
    
    public:
        virtual ~Node() = default;

        bool operator==(const Node &other) const {
            return typeid(*this) == typeid(other) && isEqual(other);
        }

        NameSet const &getLocals() const {
            return this->localVariables_;
        }

        NameSet const &getConsts() const {
            return this->globalConstants_;
        }

        VarBag const &getVars() const {
            return this->variableIndexes_;
        }

        RealType getType() const {
            return this->realExpressionType_;
        }

        nat getNumberOfVariables() const {
            return this->numOfVariables;
        }

    protected:

        virtual bool isEqual(Node const &) const { return false; }

        Node() : numOfVariables(0), use_(0) {
        }

        virtual bool isVal() const {
            return false;
        }

        virtual bool isVar() const {
            return false;
        }

        virtual bool isName() const {
            return false;
        }

        virtual bool isUnary() const {
            return false;
        }

        virtual bool isBinary() const {
            return false;
        }

        virtual bool isPower() const {
            return false;
        }

        virtual bool isLetin() const {
            return false;
        }

        virtual bool isIfnz() const {
            return false;
        }

        virtual bool isPolynomial() const {
            return false;
        }

        virtual bool isRational() const {
            return false;
        }

        virtual bool isError() const {
            return false;
        }

        virtual bool isFloor() const {
            return false;
        }

        virtual bool isMax() const {
            return false;
        }

        virtual bool isMin() const {
            return false;
        }

        virtual Real deriv(const nat) const {
            return NotAReal;
        }

        virtual Interval eval(const Box &, NamedBox &, const bool) = 0;
        virtual Real subs(const Substitution &, Names &) const = 0;
        virtual void print(std::ostream & = std::cout) const = 0;

        std::string toString() const;

        VarBag variableIndexes_;
        nat numOfVariables;
        NameSet localVariables_;
        NameSet globalConstants_;
        RealType realExpressionType_;
    
    private:
        int use_;
    };

    class RealVal_Node : public Node {
        friend class Real;
    public:
        RealVal_Node(const Interval &);

        bool isVal() const override {
            return true;
        }
        Real deriv(const nat) const override;
        Interval eval(const Box &, NamedBox &, const bool) override;
        Real subs(const Substitution &, Names &) const override;
        void print(std::ostream & = std::cout) const override;

    protected:
        bool isEqual(Node const &other) const override {
            return this->val_ == static_cast<RealVal_Node const &>(other).val_;
        }

    private:
        Interval val_; // value
    };

    class RealVar_Node : public Node {
        friend class Real;
    public:
        RealVar_Node(const nat, const std::string = "");

        bool isVar() const override {
            return true;
        }
        Real deriv(const nat) const override;
        Interval eval(const Box &, NamedBox &, const bool) override;
        Real subs(const Substitution &, Names &) const override;
        std::string name() const;
        void print(std::ostream & = std::cout) const override;

    protected:
        bool isEqual(Node const &other) const override {
            return this->var_ == static_cast<RealVar_Node const &>(other).var_;
        }

    private:
        nat var_; // variable index
        std::string name_; // variable name (for printing only)
    };

    class RealName_Node : public Node {
        friend class Real;
    public:
        RealName_Node(const std::string, const bool = true);

        bool isName() const override {
            return true;
        }
        Real deriv(const nat) const override;
        Interval eval(const Box &, NamedBox &, const bool) override;
        Real subs(const Substitution &, Names &) const override;
        void print(std::ostream & = std::cout) const override;

    protected:
        bool isEqual(Node const &other) const override {
             return this->name_ == static_cast<RealName_Node const &>(other).name_;
        };

    private:
        std::string name_; // variable name
        nat db_; // De Bruijn index 
        bool local_; // Is this variable local?
    };

    class RealUnary_Node : public Node {
        friend class Real;
    public:
        RealUnary_Node(const OpType, const Real &);

        bool isUnary() const override{
            return true;
        }
        Real deriv(const nat) const override;
        Interval eval(const Box &, NamedBox &, const bool) override;
        Real subs(const Substitution &, Names &) const override;
        void print(std::ostream & = std::cout) const override;

    protected:
        bool isEqual(Node const &other) const override {
            auto &otherUnaryNode = static_cast<RealUnary_Node const &>(other);
            return this->op_  == otherUnaryNode.op_
                && this->ope_ == otherUnaryNode.ope_;
        }

    private:
        OpType op_; // operator
        Real ope_; // operand operand
    };

    class Error_Node : public Node {
        friend class Real;
    public:
        Error_Node(const Real &, const Real &);

        Real deriv(const nat) const override;
        Interval eval(const Box &, NamedBox &, const bool) override;
        Real subs(const Substitution &, Names &) const override;
        void print(std::ostream &) const override;
        const Real &getValue() const;
        const Real &getError() const;
        bool isError() const override;

    protected:
        bool isEqual(Node const &) const override;

    private:
        const Real value_;
        const Real error_;
    };

    class Floor_Node : public Node {
        friend class Real;
    public:
        Floor_Node(const Real &);

        Real deriv(const nat) const override;
        Interval eval(const Box &, NamedBox &, const bool) override;
        Real subs(const Substitution &, Names &) const override;
        void print(std::ostream &) const override;
        const Real &getOperand() const;
        bool isFloor() const override;

    protected:
        bool isEqual(Node const & other) const override;

    private:
        const Real operand_;
    };

    class Max_Node : public Node {
        friend class Real;
    public:
        Max_Node(std::initializer_list<Real> const reals) : Max_Node(std::vector<Real>{reals}) {}
        Max_Node(std::vector<Real> const &);
        ~Max_Node() = default;

        Real deriv(const nat) const override;
        Interval eval(const Box &, NamedBox &, const bool) override;
        Real subs(const Substitution &, Names &) const override;
        void print(std::ostream & = std::cout) const override;
        const std::vector<Real> &getOperands() const;
        bool isMax() const override;

    protected:
        bool isEqual(Node const &) const override;

    private:
        std::vector<Real> operands_; // operand operand
    };

    // TODO: Refactor Max_Node and Min_Node into specialization of abstract Nary_Node
    class Min_Node : public Node {
        friend class Real;
    public:
        Min_Node(std::initializer_list<Real> const reals) : Min_Node(std::vector<Real>{reals}) {}
        Min_Node(std::vector<Real> const &);
        ~Min_Node() = default;

        Real deriv(const nat) const override;
        Interval eval(const Box &, NamedBox &, const bool) override;
        Real subs(const Substitution &, Names &) const override;
        void print(std::ostream & = std::cout) const override;
        const std::vector<Real> &getOperands() const;
        bool isMin() const override;

    protected:
        bool isEqual(Node const & other) const override;

    private:
        std::vector<Real> operands_; // operand operand
    };

    class RealBinary_Node : public Node {
        friend class Real;
    public:
        RealBinary_Node(const OpType, const Real &, const Real &);

        bool isBinary() const override {
            return true;
        }
        Real deriv(const nat) const override;
        Interval eval(const Box &, NamedBox &, const bool) override;
        Real subs(const Substitution &, Names &) const override;
        void print(std::ostream & = std::cout) const override;

    protected:
        bool isEqual(Node const &other) const override {
            auto &otherBinaryNode = static_cast<RealBinary_Node const &>(other);
            return this->op_   == otherBinaryNode.op_
                && this->ope1_ == otherBinaryNode.ope1_
                && this->ope2_ == otherBinaryNode.ope2_;
        }

    private:
        OpType op_; // operator
        Real ope1_; // operand first operand
        Real ope2_; // operand second operand
    };

    class RealPower_Node : public Node {
        friend class Real;
    public:
        RealPower_Node(const Real &, const nat);

        bool isPower() const override {
            return true;
        }
        Real deriv(const nat) const override;
        Interval eval(const Box &, NamedBox &, const bool) override;
        Real subs(const Substitution &, Names &) const override;
        void print(std::ostream & = std::cout) const override;

    protected:
        bool isEqual(Node const &) const override { return false; }

    private:
        Real ope_; // operand
        nat n_; // exponent
    };

    class RealLetin_Node : public Node {
        friend class Real;
    public:
        RealLetin_Node(const std::string, const Real &, const Real &);

        bool isLetin() const override {
            return true;
        }
        Real deriv(const nat) const override;
        Interval eval(const Box &, NamedBox &, const bool) override;
        Real subs(const Substitution &, Names &) const override;
        void print(std::ostream & = std::cout) const override;

    protected:
        bool isEqual(Node const &node) const override {
            auto other = static_cast<RealLetin_Node const &>(node);
            return this->name_ == other.name_ &&
                this->let_ == other.let_ &&
                this->in_ == other.in_;
        }

    private:
        std::string name_; // local variable
        Real let_; // let expression
        Real in_; // in expression
    };

    class RealIfnz_Node : public Node {
        friend class Real;
    public:
        RealIfnz_Node(const Real&, const Real &, const Real &);

        bool isIfnz() const override {
            return true;
        }
        Real deriv(const nat) const override;
        Interval eval(const Box &, NamedBox &, const bool) override;
        Real subs(const Substitution &, Names &) const override;
        void print(std::ostream & = std::cout) const override;

    protected:
        bool isEqual(Node const &) const override { return false; }

    private:
        Real cond_; // condition
        Real lt_; // return lt_ when cond_ < 0
        Real gt_; // return gt_ when cond_ > 0
    };

    enum MonoInfo {
        UNK, INC, DEC, CNST
    };
    typedef std::vector<MonoInfo> MonoInfos;
    typedef std::vector<Boxes> BCoeffs;

    class Polynomial_Node : public Node {
        friend class Real;
    public:

        Polynomial_Node() {
        }
        Polynomial_Node(const Monomials &, const Names &);

        bool isPolynomial() const override {
            return true;
        }
        Real deriv(const nat) const override;
        Interval eval(const Box &, NamedBox &, const bool) override;
        Real subs(const Substitution &, Names &) const override;
        void print(std::ostream & = std::cout) const override;

        nat nterms() const {
            return monoms_.size();
        } // number of monomials
        void print_metadata(std::ostream & = std::cout) const;

    protected:
        bool isEqual(Node const &other) const override {
            auto &otherPolynomialNode = static_cast<Polynomial_Node const &>(other);
            return this->monoms_ == otherPolynomialNode.monoms_;
        }

    private:
        Names names_;

        nat numberOfVariables() const {
            return numOfVariables;
        } // number of variables
        nat degree(const nat) const; // degree in a variable 
        //    void print_metadata(std::ostream & = std::cout) const;
        bool diffConstants(NamedBox &);
        void diffVariables(const Box &, NamedBox &);
        Interval eval_bp(const Box &, NamedBox &);
        Interval eval_poly(const Box &, NamedBox &) const;
        bool isMonotone(const nat);
        Monomials monoms_; // vector of monomials
        Tuple degs_; // degree in each variable
        Box B_; // current varbox
        NamedBox C_; // current constbox
        Box a_; // current coefficients
        nat s_; // number of intervals in current box that have zero in their interior
        BCoeffs b_; /* Tensor of Bernstein coefficients. k = nterms()
                 { term 0:   b_0, ... , b_(degs_0) } 
variable x0:     { ...            ...              } 
                 { term k-1: b_0, ... , b_(degs_0) }
...              ...
                 { term 0:   b_0, ... , b_(degs_(n-1)) } 
variable x(n-1): { ...            ...                  } 
                 { term k-1: b_0, ... , b_(degs_(n-1)) }
    */
        MonoInfos m_; // Monotonicity information per variable
        Ints diff_; // Auxiliary array to compute differences betwen 
        // new and current box
        // diff[i] < 0  : varbox[i] == B_[i] 
        // diff[i] == 0 : varbox[i] in B_[i]
        // diff[i] > 0  : varbox[i] not in B[i] and varbox[i] != B_[i]
    };

    // Parameter is PolyExpr. Returns POLYNOMIAL
    Real poly_polyexpr(const Real &);

    // Parameter is MonomExpr. Integer is sign of monomial. Returns POLYNOMIAL
    Real poly_monomexpr(const Real &, const int = 1);

    // Parameters are POLYNOMIALs p1 and p2. Integer sign represents either addition or substraction.
    // Returns POLYNOMIAL p1 * sign*p2
    Real poly_add(const Real &, const Real &, const int = 1);

    // Parameters are POLYNOMIALs p1 and p2 . Returns POLYNOMIAL p1*p2
    Real poly_mult(const Real &, const Real &);

    // Parameter is POLYNOMIAL p. Returns POLYNOMIAL -p
    Real poly_neg(const Real &);

    // Parameter is POLYNOMIAL p. Natural number n represents exponent. Returns POLYNOMIAL p^n
    Real poly_pow(const Real &, const nat);

    class Rational_Node : public Node {
        friend class Real;
    public:

        Rational_Node() {
        }
        Rational_Node(const Monomials &, const Monomials &, const Names &);

        bool isRational() const override {
            return true;
        }
        Real deriv(const nat) const override;
        Interval eval(const Box &, NamedBox &, const bool) override;
        Real subs(const Substitution &, Names &) const override;
        void print(std::ostream & = std::cout) const override;
        void print_metadata(std::ostream & = std::cout) const;

        nat numnterms() const {
            return num_monoms_.size();
        }

        nat dennterms() const {
            return den_monoms_.size();
        }

    protected:
        bool isEqual(Node const &) const override { return false; }

    private:
        Names names_;

        nat numberOfVariables() const {
            return numOfVariables;
        } // number of variables
        nat degree(const nat) const; // degree in a variable
        // (max of numerator degree, denominator degree)
        // void print_metadata(std::ostream & = std::cout) const;
        //   moved to public, may wish to move back
        bool diffConstants(NamedBox &);
        void diffVariables(const Box &, NamedBox &);
        Interval eval_bp(const Box &, NamedBox &);
        Interval eval_rat(const Box &, NamedBox &) const;
        Monomials num_monoms_; // vector of monomials for numerator
        Monomials den_monoms_; // vector of monomials for denominator
        Tuple degs_; // degree in each variable (max of numerator degree,
        //   denominator degree)
        Box B_; // current varbox
        NamedBox C_; // current constbox
        Box num_a_; // current coefficients for numerator
        Box den_a_; // current coefficients for denominator
        BCoeffs num_b_; // Bernstein coefficients for numerator
        BCoeffs den_b_; // Bernstein coefficients for denominator
        // format as per Polynomial_Node
        Ints diff_; // Auxiliary array to compute differences betwen 
        // new and current box
        // diff[i] < 0  : varbox[i] == B_[i] 
        // diff[i] == 0 : varbox[i] in B_[i]
        // diff[i] > 0  : varbox[i] not in B[i] and varbox[i] != B_[i]
    };

    Real rational(const Real &);

    // Parameters are POLYNOMIALs p1 and p2. Returns RATIONAL p1/p2
    Real poly_div(const Real &, const Real &);



    std::ostream &operator<<(std::ostream &, const kodiak::MonoInfo);
}
#endif // KODIAK_NODE
