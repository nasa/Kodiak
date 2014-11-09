#ifndef _BOOL_
#define _BOOL_

#include "Real.hpp"

namespace kodiak {
  
  class RelExpr {

  public:
    RelExpr(const Real &, const RelType, const bool = false, 
	    const Substitution & = EmptySubstitution, const nat nvars = 0);
    Real ope() const { return ope_; }
    RelType op() const { return op_; } 
    
    // > 0 : relation wrt 0 is certainly true
    // = 0 : relation wrt 0 is certainly false
    // = -1 : relation wrt 0 is possibly true
    // = -2 : relation wrt 0 is possibly true within eps (a positive small number). 
    int eval(Box &box, const bool bp = false, const real eps = 0) {
      NamedBox constbox;
      return eval(box,constbox,bp,eps);
    }
    int eval(Box &, NamedBox &, const bool = false, const real = 0);
    // Partial derivate for given variable
    Real d(const nat) const; 
    void print(std::ostream &) const;
  private:
    Real ope_;
    RelType op_;
    // Partial derivatives of the real expression for each variable
    Realn D_;
  };



std::ostream &operator<< (std::ostream &, const kodiak::RelExpr &);
std::ostream &operator<< (std::ostream &, const kodiak::RelType);
}
#endif 
