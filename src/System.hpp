#ifndef _SYSTEM_
#define _SYSTEM_

#include "Bool.hpp"

namespace kodiak {

  // Cache evaluation of a system during Branch and Bound 
  class Certainties {
  public:
    void init(const nat n) { certs_.assign(n,-1), cert_ = -1; }
    int get() const { return cert_; } 
    int get(const nat i) const { return (i < certs_.size() ? certs_[i] : -1); };
    int set(const nat i, const int cert) { if (i < certs_.size()) certs_[i] = cert; return cert; }
    int set(const int cert) { cert_ = cert; return cert; }      
    void print(std::ostream &) const;
  private:
    // > 0 : relation wrt 0 is certainly true
    // = 0 : relation wrt 0 is certainly false
    // = -1 : relation wrt 0 is possibly true
    // = -2 : relation wrt 0 is possibly true within eps (a positive small number). 
    Ints certs_; // Vector of evaluations, 1 per constraint
    int cert_; // Total evaluation
  };
  
  class System {
    
  public:
    System(const std::string = ""); 

    // Tolerance is a non-negative number on precise we need the output to be.
    // Resolution is a number between [0,1] that is understood as a precision in the range of the varibles.

    // Get system's identifier
    std::string id() const { return id_; }
    // Set system's identifier
    void set_id(const std::string id) { id_ = id; }	

    // Set default enclosure method. If bp is true, Bernstein polynomials are used whenever possible. 
    // Evaluation of these polynomials is expensive but the enclosures 
    // are more precise.
    void set_bp(const bool bp = true) { bp_ = bp; }
    
    // Set default resolution for every variable, i.e., number in the interval [0,1]
    // that represents a percentage on the width of the original box.
    void set_tolerance(const real);
    void set_precision(int precision) { set_tolerance(pow(10,precision)); }
    void set_granularity(const real);
    void set_resolution(const std::string, const real);
    void set_resolution(const nat, const real);
    void set_resolutions(const real);

    real tolerance() const { return tolerance_; }
    real granularity() const { return granularity_; }
    real resolution(const std::string) const;
    real resolution(const nat) const;

    void set_conjunctive(const bool conjunctive = true) { conjunctive_ = conjunctive; }
    bool conjunctive() const { return conjunctive_; }

    void set_varselect(const nat vs) { varselect_ = vs; }

    const NamedBox &varbox() const { return varbox_; }
    const NamedBox &constbox() const { return constbox_; }

    nat var(const std::string id, const Interval &val) { return var(id,val,val); }
    nat var(const std::string, const Interval &, const Interval &);
    nat var(const Interval &val) { return var("",val,val); }
    nat var(const Interval &lb, const Interval &ub) { return var("",lb,ub); }
    void vars(const NamedBox &);
    nat cnst(const std::string id, const Interval &I) { return cnst(id,I,I); }
    nat cnst(const std::string id, const Interval &L, const Interval &U) { return constbox_.push(id,L,U); }
    void cnsts(const NamedBox &constbox) { constbox_.insert(constbox); }

    void def(const std::string id, const Real &e) { def(id,e,bp_); }
    void def(const std::string, const Real &, const bool);
    void defs(const NamedBox defbox, const std::vector< std::pair<std::string,Real> > defs);
    void rel0(const Real &, const RelType, const bool);
    void eq0(const Real &e) { eq0(e,bp_); }
    void eq0(const Real &e, const bool bp) { rel0(e,EQ,bp); }
    void le0(const Real &e) { le0(e,bp_); }
    void le0(const Real &e, const bool bp) { rel0(e,LE,bp); }
    void lt0(const Real &e) { lt0(e,bp_); }
    void lt0(const Real &e, const bool bp) { rel0(e,LT,bp); }
    void ge0(const Real &e) { ge0(e,bp_); }
    void ge0(const Real &e, const bool bp) { rel0(e,GE,bp); }
    void gt0(const Real &e) { gt0(e,bp_); }
    void gt0(const Real &e, const bool bp) { rel0(e,GT,bp); }
    void eq(const Real &e1, const Real &e2) { eq0(e1-e2,bp_); }
    void eq(const Real &e1, const Real &e2, const bool bp) { rel0(e1-e2,EQ,bp); }
    void le(const Real &e1, const Real &e2) { le0(e1-e2,bp_); }
    void le(const Real &e1, const Real &e2, const bool bp) { rel0(e1-e2,LE,bp); }
    void lt(const Real &e1, const Real &e2) { lt0(e1-e2,bp_); }
    void lt(const Real &e1, const Real &e2, const bool bp) { rel0(e1-e2,LT,bp); }
    void ge(const Real &e1, const Real &e2) { ge0(e1-e2,bp_); }
    void ge(const Real &e1, const Real &e2, const bool bp) { rel0(e1-e2,GE,bp); }
    void gt(const Real &e1, const Real &e2) { gt0(e1-e2,bp_); }
    void gt(const Real &e1, const Real &e2, const bool bp) { rel0(e1-e2,GT,bp); }
    void eq(const Real &e1, const Interval &e2) { eq0(e1-e2,bp_); }
    void eq(const Real &e1, const Interval &e2, const bool bp) { rel0(e1-e2,EQ,bp); }
    void le(const Real &e1, const Interval &e2) { le0(e1-e2,bp_); }
    void le(const Real &e1, const Interval &e2, const bool bp) { rel0(e1-e2,LE,bp); }
    void lt(const Real &e1, const Interval &e2) { lt0(e1-e2,bp_); }
    void lt(const Real &e1, const Interval &e2, const bool bp) { rel0(e1-e2,LT,bp); }
    void ge(const Real &e1, const Interval &e2) { ge0(e1-e2,bp_); }
    void ge(const Real &e1, const Interval &e2, const bool bp) { rel0(e1-e2,GE,bp); }
    void gt(const Real &e1, const Interval &e2) { gt0(e1-e2,bp_); }
    void gt(const Real &e1, const Interval &e2, const bool bp) { rel0(e1-e2,GT,bp); }
    const RelExpr &rel(const nat f) const { return rels_[f]; }
    
    nat nvars() const { return varbox_.size(); }
    nat nconsts() const { return constbox_.size(); }
    nat nrels() const { return rels_.size(); }
    nat ndefs() const { return defs_.size(); }
    void evalDefs() { evalDefs(varbox_.box()); };
    void evalDefs(Box &);
    int evalSystem() { return evalSystem(varbox_.box()); }
    int evalSystem(Box &);
    int evalSystem(Box &, Certainties &, const DirVars &);
    void sosSystem(Interval &, Certainties &, const Box &, const nat);
    void printSystem(std::ostream & = std::cout, const nat = 0) const;

  protected:
    static nat IDs_; // Total number of indentifiers (for generating new names)
    std::string id_; // Identifier of the system
    NamedBox varbox_;
    NamedBox constbox_;
    NamedBox defbox_;
    std::vector<RelExpr> rels_; // List of relational formulas
    bool conjunctive_; // Specifies if rels_  should be understood in a conjunctive or disjunctive way
    // If bp is true, Bernstein polynomials are used whenever possible. 
    // Evaluation of these polynomials is expensive but the enclosures 
    // are more precise.
    std::vector< std::pair<std::string,Real> > defs_; // Global definitions
    Substitution defdx_; // Constant substitutions of d(defs_)/dx 
    bool bp_; // Default value
    Bools bps_; // Value of bp for each formula in rels_
    real tolerance_; // Non-negative real number that specifies an absolute tolerance value to stop the branch-and-bound 
    // recursion.
    real granularity_; // Real number in the interval [0,1] interpreted as a percentage of the initial box.
    // The granularity specifies the default resolution of each variable.
    real resolution_;
    Point resolutions_; // Resolution of variables in varbox_. The resolution of a variable is a non-negative number
    // that specifies the smallest range considered for that vaible.
    nat varselect_;
  };



std::ostream& operator<< (std::ostream &, const Certainties &);
}
#endif
