#include "System.hpp"

using namespace kodiak;

nat System::IDs_ = 0;

void Certainties::print(std::ostream &os) const {
  os << cert_ << " : " << certs_;
}

std::ostream &kodiak::operator<< (std::ostream &os, const Certainties &certs) {
  certs.print(os);
  return os;
}

System::System(const std::string id) : 
  conjunctive_(true), bp_(false), tolerance_(0.1), granularity_(0), varselect_(0) {
  if (id == "") {
    std::ostringstream os;
    os << "F" << IDs_++;
    id_ = os.str();
  } else 
    id_ = id;
}

// Set default tolerance
void System::set_tolerance(const real tolerance) {
  if (tolerance >= 0) 
    tolerance_ = tolerance;
  else {
    std::ostringstream os;
    os << "Kodiak (set_tolerance): tolerance " << tolerance << " must be non-negative";
    throw Growl(os.str());
  }
}

// Set granularity
void System::set_granularity(const real granularity) {
  if (granularity >= 0 && granularity <= 1) 
    granularity_ = granularity;
  else {
    std::ostringstream os;
    os << "Kodiak (set_granularity): granularity " << granularity << " must be in [0,1]";
    throw Growl(os.str());
  }
}

// Set resolution for variable name
void System::set_resolution(const std::string name, const real resolution) {
  nat v = varbox_.var(name);
  if (v < resolutions_.size()) {
    if (resolution >= 0) 
      resolutions_[v] = resolution;
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
  if (v < resolutions_.size()) {
    if (resolution >= 0)
      resolutions_[v] = resolution;
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
    resolution_ = resolution;
    for (nat v=0; v < resolutions_.size(); ++v) 
      resolutions_[v] = resolution;
  } else {
    std::ostringstream os;
    os << "Kodiak (set_resolution): variable resolution " << resolution << " must be non-negative";
    throw Growl(os.str());
  }
}

// Return resolution of variable name
real System::resolution(const std::string name) const {
  nat v = varbox_.var(name);
  if (v < resolutions_.size()) 
    return resolutions_[v];
  std::ostringstream os;
  os << "Kodiak (resolution): variable \"" << name << "\" doesn't exist in named box";
  throw Growl(os.str());
}

// Return resolution of n-th variable
real System::resolution(const nat v) const {
  if (v < resolutions_.size()) 
    return resolutions_[v];
  std::ostringstream os;
    os << "Kodiak (resolution): variable index " << v << " doesn't exist in named box";
  throw Growl(os.str());
}

void System::vars(const NamedBox &varbox) { 
  nat r = resolutions_.size();
  resolutions_.insert(resolutions_.end(),varbox.size(),0);
  for (nat v=0; v < varbox.size(); ++v) {
    Interval X = varbox.val(v);
    real resolution = (X.sup()-X.inf())*granularity_;
    resolutions_[r+v] = resolution;
  }  
  varbox_.insert(varbox); 
}

nat System::var(const std::string id, const Interval &lb, const Interval &ub) {
  real resolution = (ub.sup()-lb.inf())*granularity_;
  resolutions_.push_back(resolution);
  return varbox_.push(id,lb,ub);
}

void System::def(const std::string id, const Real &e, const bool bp) {
  Real r = e.index(varbox_,constbox_);
  r.checkIndexed(defs_);
  defs_.push_back(make_pair(id,bp ? polynomial(r) : r));
  defbox_.push(id);
  for (nat v=0; v < varbox_.size(); ++v) { 
    Real dx = (bp ? polynomial(r.deriv(v)) : r.deriv(v)).subs(defdx_);
    std::ostringstream os;
    os << "d" << id << "/d" << var_name(v);
    if (dx.isConst()) 
      defdx_[os.str()] = dx;
    else {
      defs_.push_back(make_pair(os.str(),dx));
      defbox_.push(os.str());
    }
  }
}

void System::defs(const NamedBox defbox, const std::vector< std::pair<std::string,Real> > defs){
  Names names = defbox.names();
  Box values = defbox.box();
  for(int i = 0; i < defbox.size(); ++i){
    def(names[i], defs[i].second, false);

  }
}

// Index equation and add it to rels_. The Boolean parameter specifies the type of evaluation 
// for this equation, true meaning Bernstein enclosure.
void System::rel0(const Real &e, const RelType rel, const bool bp) {
  Real r = e.index(varbox_,constbox_);
  r.checkIndexed(defs_);
  rels_.push_back(RelExpr(r,rel,bp,defdx_,r.closed() ? 0 : nvars()));
  bps_.push_back(bp);
}

void System::evalDefs(Box &box) {
  for (nat f = 0; f < ndefs(); ++f) {
    Interval X = defs_[f].second.eval(box,defbox_,bp_);
    defbox_.set(f,X);
  }
}

int System::evalSystem(Box &box) {
  evalDefs();
  int cert = 1;
  for (nat f = 0; f < nrels(); ++f) {
    int c = rels_[f].eval(box,defbox_,bps_[f],tolerance_);
    // if f-th formula is certainly not true, conjunction of formulas is certainly not true
    if (c == 0) return 0;
    if (c < 0) cert = (cert > 0 ? c : std::max(cert,c));
  } 
  return cert;
}

int System::evalSystem(Box &box, Certainties &certs, const DirVars &dirvars) {
  evalDefs();
  if (certs.get() > 0) return certs.get();
  int cert = 1;
  for (nat f = 0; f < nrels(); ++f) {
    // if f-th formula is certainly true, don't check it again
    if (certs.get(f) > 0) continue;
    if (!dirvars.empty() && rels_[f].ope().closed() &&
	rels_[f].ope().isFresh(dirvars.back().var) &&
	where(dirvars,dirvars.back().var) != EXTERIOR) {
      // top variable doesn't appear in f-th formula, don't check it again
      cert = (cert > 0 ? certs.get(f) : std::max(cert,certs.get(f)));
      continue;
    }
    certs.set(f,rels_[f].eval(box,defbox_,bps_[f],tolerance_));
    // if f-th formula is certainly not true, conjunction of formulas is certainly not true
    if (certs.get(f) == 0) 
      return certs.set(0); 
    if (certs.get(f) < 0) 
      cert = (cert > 0 ? certs.get(f) : std::max(cert,certs.get(f)));
  } 
  return certs.set(cert);
}

void System::sosSystem(Interval &X, Certainties &certs, const Box &box, const nat v) {
  X = Interval(0);
  if (certs.get() == -1) { 
    for (nat f = 0; f < nrels(); ++f) {
      if (certs.get(f) == -1 && (rels_[f].ope().open() || rels_[f].ope().hasVar(v))) 
	X += sqr(rels_[f].ope().eval(box,defbox_)); 
    }
  }
}

void System::printSystem(std::ostream &os, const nat debug) const {
  os << "System: " << id_ << std::endl;
  if (debug > 0) {
    os << "Tolerance: " << tolerance_ << std::endl;
    os << "Granularity: " << std::floor(100*(granularity_)) << "%" << std::endl;
    os << "Default Enclosure Method: " << (bp_ ? "BP" : "IA") << std::endl;
  }
  os << "Vars: ";
  varbox_.print(os);
  os << std::endl;
  if (debug > 0) {
    nat v;
    for (v=0; v < varbox_.size(); ++v) 
      if (resolutions_[v] != 0) break;
    if (v < varbox_.size()) {
      os << "Resolutions: " << std::endl;
      os << "  " << varbox_.name(v) << ": " << resolutions_[v];
      for (++v; v < varbox_.size(); ++v) {
	if (resolutions_[v] != 0) 
	  os << ", " << varbox_.name(v) << ": " << resolutions_[v];
      }
      os << std::endl;
    }
  }
  if (!constbox_.empty()) {
    os << "Consts: ";
    constbox_.print(os);
    os << std::endl;
  } 
  if (!defs_.empty()) {
    os << "Definitions: " << std::endl;
    for (nat f=0; f < ndefs(); ++f) {
      os << "  " << defs_[f].first << " = " << defs_[f]. second << std::endl;
    }
  } 
  if (!rels_.empty()) {
    os << "Constraints: " << std::endl;
    for (nat f=0; f < nrels(); ++f) 
      os << "  " << id_ << "_" << f << ": " << rels_[f] << std::endl;
  }
  if (debug > 0) {
    os << "Partial derivatives: " << std::endl;
    for (nat f=0; f < nrels(); ++f) {
      for (nat v=0; v < nvars(); ++v) 
	os << "  d" << id_ << "_" << f << "/d" << varbox_.name(v) 
	   << " = " << rels_[f].d(v) << std::endl;
    }
  }
}

