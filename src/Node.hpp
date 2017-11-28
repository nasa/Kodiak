#ifndef _NODE_
#define _NODE_

#include "Real.hpp"

namespace kodiak {

    class Max_Node;
    class Min_Node;
    class Error_Node;
    class Floor_Node;

    class Node {
        friend class Real;
    
    public:
        virtual ~Node() = default;

        virtual bool operator== (const Node &) const {
            return false;
        }
        virtual bool operator==(const Error_Node &) const {
            return false;
        }
        virtual bool operator==(const Floor_Node &) const {
            return false;
        }
        virtual bool operator==(const Max_Node &) const {
            return false;
        }
        virtual bool operator==(const Min_Node &) const {
            return false;
        }

    protected:

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

        bool isVal() const {
            return true;
        }
        Real deriv(const nat) const;
        Interval eval(const Box &, NamedBox &, const bool);
        Real subs(const Substitution &, Names &) const;
        void print(std::ostream & = std::cout) const;
    private:
        Interval val_; // value
    };

    class RealVar_Node : public Node {
        friend class Real;
    public:
        RealVar_Node(const nat, const std::string = "");

        bool isVar() const {
            return true;
        }
        Real deriv(const nat) const;
        Interval eval(const Box &, NamedBox &, const bool);
        Real subs(const Substitution &, Names &) const;
        std::string name() const;
        void print(std::ostream & = std::cout) const;
    private:
        nat var_; // variable index
        std::string name_; // variable name (for printing only)
    };

    class RealName_Node : public Node {
        friend class Real;
    public:
        RealName_Node(const std::string, const bool = true);

        bool isName() const {
            return true;
        }
        Real deriv(const nat) const;
        Interval eval(const Box &, NamedBox &, const bool);
        Real subs(const Substitution &, Names &) const;
        void print(std::ostream & = std::cout) const;
    private:
        std::string name_; // variable name
        nat db_; // De Bruijn index 
        bool local_; // Is this variable local?
    };

    class RealUnary_Node : public Node {
        friend class Real;
    public:
        RealUnary_Node(const OpType, const Real &);

        bool isUnary() const {
            return true;
        }
        Real deriv(const nat) const;
        Interval eval(const Box &, NamedBox &, const bool);
        Real subs(const Substitution &, Names &) const;
        void print(std::ostream & = std::cout) const;
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

        virtual bool operator==(const Error_Node &) const override;
        virtual bool operator==(const Node &      ) const override;
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

        virtual bool operator==(const Floor_Node &) const override;
        virtual bool operator==(const Node &      ) const override;
    private:
        const Real operand_;
    };

    class Max_Node : public Node {
        friend class Real;
    public:
        Max_Node(std::vector<Real>&);
        ~Max_Node() = default;

        Real deriv(const nat) const;
        Interval eval(const Box &, NamedBox &, const bool);
        Real subs(const Substitution &, Names &) const;
        void print(std::ostream & = std::cout) const;
        const std::vector<Real> &getOperands() const;
        bool isMax() const override;

        virtual bool operator==(const Max_Node &) const override;
        virtual bool operator==(const Node &) const override;
    private:
        std::vector<Real> operands_; // operand operand
    };

    // TODO: Refactor Max_Node and Min_Node into specialization of abstract Nary_Node
    class Min_Node : public Node {
        friend class Real;
    public:
        Min_Node(std::initializer_list<const Real>);
        Min_Node(std::vector<Real>&);
        ~Min_Node() = default;

        Real deriv(const nat) const;
        Interval eval(const Box &, NamedBox &, const bool);
        Real subs(const Substitution &, Names &) const;
        void print(std::ostream & = std::cout) const;
        const std::vector<Real> &getOperands() const;
        bool isMin() const override;

        virtual bool operator==(const Min_Node &) const override;
        virtual bool operator==(const Node &) const override;
    private:
        std::vector<Real> operands_; // operand operand
    };

    class RealBinary_Node : public Node {
        friend class Real;
    public:
        RealBinary_Node(const OpType, const Real &, const Real &);

        bool isBinary() const {
            return true;
        }
        Real deriv(const nat) const;
        Interval eval(const Box &, NamedBox &, const bool);
        Real subs(const Substitution &, Names &) const;
        void print(std::ostream & = std::cout) const;
    private:
        OpType op_; // operator
        Real ope1_; // operand first operand
        Real ope2_; // operand second operand
    };

    class RealPower_Node : public Node {
        friend class Real;
    public:
        RealPower_Node(const Real &, const nat);

        bool isPower() const {
            return true;
        }
        Real deriv(const nat) const;
        Interval eval(const Box &, NamedBox &, const bool);
        Real subs(const Substitution &, Names &) const;
        void print(std::ostream & = std::cout) const;
    private:
        Real ope_; // operand
        nat n_; // exponent
    };

    class RealLetin_Node : public Node {
        friend class Real;
    public:
        RealLetin_Node(const std::string, const Real &, const Real &);

        bool isLetin() const {
            return true;
        }
        Real deriv(const nat) const;
        Interval eval(const Box &, NamedBox &, const bool);
        Real subs(const Substitution &, Names &) const;
        void print(std::ostream & = std::cout) const;
    private:
        std::string name_; // local variable
        Real let_; // let expression
        Real in_; // in expression
    };

    class RealIfnz_Node : public Node {
        friend class Real;
    public:
        RealIfnz_Node(const Real&, const Real &, const Real &);

        bool isIfnz() const {
            return true;
        }
        Real deriv(const nat) const;
        Interval eval(const Box &, NamedBox &, const bool);
        Real subs(const Substitution &, Names &) const;
        void print(std::ostream & = std::cout) const;
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

        bool isPolynomial() const {
            return true;
        }
        Real deriv(const nat) const;
        Interval eval(const Box &, NamedBox &, const bool);
        Real subs(const Substitution &, Names &) const;
        void print(std::ostream & = std::cout) const;

        nat nterms() const {
            return monoms_.size();
        } // number of monomials
        void print_metadata(std::ostream & = std::cout) const;
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

        bool isRational() const {
            return true;
        }
        Real deriv(const nat) const;
        Interval eval(const Box &, NamedBox &, const bool);
        Real subs(const Substitution &, Names &) const;
        void print(std::ostream & = std::cout) const;
        void print_metadata(std::ostream & = std::cout) const;

        nat numnterms() const {
            return num_monoms_.size();
        }

        nat dennterms() const {
            return den_monoms_.size();
        }
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
#endif


