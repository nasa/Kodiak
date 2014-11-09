#ifndef _BIFURCATION_
#define _BIFURCATION_

#include "Paver.hpp"

namespace kodiak {

  class Bifurcation : public Paver {
  public: 
    Bifurcation(const std::string = "");
    void bifurcation(const bool = false, const bool = false);
    void first_bifurcation() { bifurcation(true); }
    void equilibrium() { bifurcation(false,true); }
    void first_equilibrium() { bifurcation(true,true); }
    void params(const NamedBox &);
    nat param(const std::string id, const Interval &I) { return param(id,I,I); }
    nat param(const std::string, const Interval &, const Interval &);
    void dfeq(const Real &e) { dfeq(e,bp_); }
    void dfeq(const Real &, const bool);
    void print(std::ostream & = std::cout) const;    
    std::string longId() const { 
      std::ostringstream is;
      if (equilibrium_)
	is << id_ << "_equilibrium";
      else
	is << id_ << "_bifurcation";
      return is.str();
    }
    void gnuplot_2D(const std::string var1, const std::string var2, 
		    const bool zoom = false) const {
      paving_.gnuplot_2D(longId(),var1,var2,typenames_,colors_,zoom);
    }
    void gnuplot_3D(const std::string var1, const std::string var2, const std::string var3, 
		    const bool zoom = false) const {
      paving_.gnuplot_3D(longId(),var1,var2,var3,typenames_,colors_,zoom);
    }

  protected:
    void evaluate(PrePaving &, Ints &, Box &);

  private:
    nat nvars_; // Number of variables
    nat nparams_; // Number of parameters
    nat ndfeqs_; // Number of differential equations
    Realn coeffs_; // Coefficients of characteristic polynomial 
    // (coefficient k corresponds to degree n-k)
    Box Coeffs_; // Interval evaluation of coeffs_ in a given box 
    // (size is n+2, which corresponds to n+1 coefficients and memory for temporal computations)
    Realn hurwitz_; // Hurtwitz determinants
    bool equilibrium_; // Compute equilibrium points only
    Names typenames_; // Names of each type of paving (for printing and plotting)   
    Names colors_; // Color for each type of paving (for printing and plotting)
  };

}

#endif
