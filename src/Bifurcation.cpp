/* Branch and bound algorithm for bifurcation analysis. This code implements the 
   Hopf bifurcation criterion provided in the following paper:
   [EW00] M'Hammed El Kahoui and Andreas Weber, Deciding Hopf Bifurcations by Quantifier Elimination 
   in a Software-component Architecture, J. Symbolic Computations (2000),
   30, 161-179. 
*/

#include "Bifurcation.hpp"

using namespace kodiak;

Bifurcation::Bifurcation(const std::string id) : Paver(id), nvars_(0), nparams_(0), ndfeqs_(0), equilibrium_(false) {
  typenames_.resize(2);
  colors_.resize(2);
}

void Bifurcation::params(const NamedBox &parambox) { 
  if (varbox_.empty() || (nvars_ != 0 && nparams_ != varbox_.size()-nvars_))
    Growl("Kodiak (params): variables must be added before parameters");
  if (nvars_ == 0)
    nvars_ = varbox_.size();
    nat r = resolutions_.size();
  resolutions_.insert(resolutions_.end(),varbox_.size(),0);
  for (nat v=0; v < parambox.size(); ++v) {
    Interval X = parambox.val(v);
    real resolution = (X.sup()-X.inf())*granularity_;
    resolutions_[r+v] = resolution;
  }  
  varbox_.insert(parambox); 

  nparams_ += parambox.size();
}

nat Bifurcation::param(const std::string id, const Interval &lb, const Interval &ub) {
  if (varbox_.empty() || (nvars_ != 0 && nparams_ != varbox_.size()-nvars_))
    Growl("Kodiak (param): variables must be added before parameters");
  if (nvars_ == 0)
    nvars_ = varbox_.size();
  ++nparams_;
  real resolution = (ub.sup()-lb.inf())*granularity_;
  resolutions_.push_back(resolution);
  return varbox_.push(id,lb,ub);
}

// Index equation and add it to eqs_. The Boolean parameter specifies the type of evaluation 
// for this equation (bp = true means Bernstein enclosure)
void Bifurcation::dfeq(const Real &e, const bool bp) {
  if (nparams_ == 0)
    Growl("Kodiak (dfeq): variables and parameters must be added before differential equations");
  if (ndfeqs_ < nrels())
    Growl("Kodiak (dfeq): differential equations must be added before constrainsts");
  if (nvars_ == ndfeqs_) 
    Growl("Kodiak (dfeq): no more differential equations than variables can be added");
  eq0(e,bp);
  ndfeqs_++;
  if (nvars_ == ndfeqs_) {
    // The system has as many differential equations as variables
    // Compute (n+1) symbolic coefficients (coeff_) of characteristic polynomials and
    // n symbolic Hurwitz (hurwitz_) determinants.
    Substitution jacobian;
    for (nat f=0; f < ndfeqs_; ++f) {
      for (nat v=0; v < nvars_; ++v) {
	std::ostringstream os;
	os << "j[" << f << "," << v << "]";
	jacobian[os.str()] = rel(f).d(v);
      }
    }
    characteristic_coeffs(coeffs_,nvars_,jacobian,"j");
    hurwitz_.resize(nvars_);
    Coeffs_.resize(nvars_+2);
    Coeffs_[nvars_] = Interval(1);
    Substitution delta;
    hurwitz_subs(delta,nvars_,"h");
    for (nat i=0; i < coeffs_.size(); ++i) {
      if (i < nvars_) {
	hurwitz_[i] = hurwitz_det(delta,nvars_,i,"h");
	if (bp_)
	  hurwitz_[i] = polynomial(hurwitz_[i]);
      }
      if (bp_) 
	coeffs_[i] = polynomial(coeffs_[i]); 
    }
  }
}

void Bifurcation::bifurcation(const bool first, const bool equilibrium) {
  if (equilibrium) {
    typenames_[0] = "Possibly Equilibrium";
    typenames_[1] = "Almost Certainly Equilibrium";
    colors_[0] = "red";
    colors_[1] = "dark-red";
  } else {
    typenames_[0] = "Possibly Limit Point Bifurcation";
    typenames_[1] = "Possibly Hopf Bifurcation";
    colors_[0] = "blue";
    colors_[1] = "red";
  }
  if (ndfeqs_ < nvars_) {
    std::ostringstream os;
    os << "Kodiak (bifurcation): Number of differential equations (" << ndfeqs_ 
       << ") cannot be less than number of variables (" << nvars_ << ")";
    throw Growl(os.str());
  }
  set_flags(nvars_-1);
  equilibrium_ = equilibrium;
  pave(first?FIRST:STD);
}

void Bifurcation::evaluate(PrePaving &paving, Ints &certainties, Box &box) {
  cert_ = 1;
  for (nat f = 0; f < nrels(); ++f) {
    // if f-th formula is certainly true, don't check it again
    if (certainties[f] > 0) continue;
    if (!dirvars().empty() && rels_[f].ope().isFresh(dirvars().back().var) &&
	where(dirvars(),dirvars().back().var) != EXTERIOR) {
      // top variable doesn't appear in f-th formula, don't check it again
      cert_ = (cert_ > 0 ? certainties[f] : std::max(cert_,certainties[f]));
      continue;
    }
    certainties[f] = rels_[f].eval(box,bps_[f],tolerance_);
    // if f-th formula is certainly not true, conjunction of formulas is certainly not true
    if (certainties[f] == 0) {
      cert_ = 0;
      return; 
    }
    if (certainties[f] < 0) 
      cert_ = (cert_ > 0 ? certainties[f] : std::max(cert_,certainties[f]));
  }
  // At this point, cert_ is negative
  // assert(cert_ < 0);
  if (equilibrium_) {
    paving.push_box(std::abs(cert_+1),box); // Whole interval (possibly)
    return;
  }
  bool possibly_steady = false;
  // Conjunction of formulas is possibly true
  Coeffs_[0] = coeffs_[0].eval(box,bp_);
  // If a(0) is negative, system is unstable and it cannot be a bifurcation point
  if (Coeffs_[0].clt(0)) {
    cert_ = 0;
    return;
  }
  possibly_steady = Coeffs_[0].contains(0);
  if (!possibly_steady) { // Check if it is possibly hopf
    // At this point a(0) > 0
    // If nvars_ == 1, system is stable and it cannot be a bifurcation point
    if (nvars_ == 1) {
      cert_ = 0;
      return;
    }
    for (nat i=1; i < nvars_; ++i) 
      Coeffs_[i] = coeffs_[i].eval(box,bp_);
    // May be a bifurcation point
    // Computer Hurwitz determinants and check that 
    // det(0),...,det(n-3) > 0, det(n-2) is zero
    for (nat i=0; i < nvars_-1; ++i) {
      Coeffs_[nvars_+1] = hurwitz_[i].eval(Coeffs_,bp_);
      nat f = nrels()+i;
      certainties[f] = Coeffs_[nvars_+1].rel0(i<nvars_-2?GT:EQ,tolerance_);
      if (certainties[f] == 0) {
	cert_ = 0;
	return;
      }
      if (certainties[f] < 0) 
	cert_ = std::max(cert_,certainties[f]);
    }
  } else {
    nat f = nrels();
    certainties[f] = Coeffs_[0].rel0(EQ,tolerance_);
    if (certainties[f] < 0) 
      cert_ = std::max(cert_,certainties[f]);
  }
  paving.push_box(possibly_steady?0:1,box); // Whole interval (possibly)
}

void Bifurcation::print(std::ostream &os) const {
  os << "System: " << id_ << std::endl;
  os << "Vars: ";
  varbox_.print(os,0,nvars_);
  os << std::endl;
  os << "Params: ";
  if (nparams_ != 0) 
    varbox_.print(os,nvars_);
  os << std::endl;
  if (!constbox_.empty()) {
    os << "Consts: ";
    constbox_.print(os);
    os << std::endl;
  } 
  os << "Differential Equations: " << std::endl;
  for (nat f=0; f < ndfeqs_; ++f) 
    os << "dot_" << varbox_.name(f) << " = " << rel(f).ope() << std::endl;
  if (ndfeqs_ < nrels())
    os << "Constraints: " << std::endl;
  for (nat f=ndfeqs_; f < nrels(); ++f) 
    os << id_ << "_" << f << ": " << rel(f) << std::endl;
  if (debug() && nrels() > 0) {
    os << std::endl << "<BEGIN DEBUG INFORMATION>" << std::endl;
    os << "*** Partial derivatives: " << std::endl;
    for (nat f=0; f < nrels(); ++f) {
      for (nat v=0; v < varbox_.size(); ++v) 
	os << "d" << id_ << "_" << f << "/d" << varbox_.name(v) 
	   << " = " << rel(f).d(v) << std::endl;
    }
    os << "*** Coefficients of characteristic polynomial: " << std::endl;
    for (nat v=0; v < coeffs_.size(); ++v) {
      os << "x" << v << "X^" << v << ": " << coeffs_[v] << std::endl;
    }
    os << "*** Hurwitz determinants: " << std::endl;
    for (nat v=0; v < hurwitz_.size(); ++v) {
      os << "h" << v << ": " << hurwitz_[v] << std::endl;
    }
    os << "<END DEBUG INFORMATION>" << std::endl << std::endl;
  }
  for (nat i=0; i < typenames_.size(); i++) 
    os << typenames_[i] << ": " << paving_.size(i) << " boxes" << std::endl;
  print_info(os);
  os << std::endl;
  paving_.save(longId(),typenames_);
}

