#include "Bool.hpp"

using namespace kodiak;

RelExpr::RelExpr(const Real &r, const RelType rel, const bool poly, const Substitution &sigma, 
		 const nat nvars) : 
  op_(rel) {
  ope_ = poly ? polynomial(r) : r;
  D_.resize(std::max(nvars,r.maxvar()));
  for (nat v=0; v < D_.size(); ++v) {
    D_[v] = (poly ? polynomial(r.deriv(v)) : r.deriv(v)).subs(sigma);
  }
}

// > 0  : relation wrt 0 is certainly true
// = 0  : relation wrt 0 is certainly false
// = -1 : relation wrt 0 is possibly true
// = -2 : relation wrt 0 is possibly true within eps (a positive small number). 
int RelExpr::eval(Box &box, NamedBox &constbox, const bool enclosure, const real eps) {
  Interval i = ope_.eval(box,constbox,enclosure);
  int cly = i.rel0(op_,eps);
  if (cly >= 0) return cly;
  for (nat v=0; v < ope_.maxvar(); v++) {
    Interval save_v = box[v];
    try {
      i = d(v).eval(box,constbox,enclosure);
      bool inc = i.cge(0);
      bool dec = i.cle(0);
      if (inc && (op_ == LE || op_ == LT) ||
	  dec && (op_ == GE || op_ == GT)) {
	box[v] = save_v.Sup();
	Interval sup = ope_.eval(box,constbox);
	int dly = sup.rel0(op_);
	if (dly > 0) {
	  box[v] = save_v;
	  return dly;
	}
	box[v] = save_v.Inf();
	Interval inf = ope_.eval(box,constbox);
	dly = inf.rel0(op_);
	if (dly == 0) {
	  box[v] = save_v;
	  return dly;
	}
      } else if (inc && (op_ == GE || op_ == GT) ||
		 dec && (op_ == LE || op_ == LT)) { 
	box[v] = save_v.Inf();
	Interval inf = ope_.eval(box,constbox);
	int dly = inf.rel0(op_);
	if (dly > 0) {
	  box[v] = save_v;
	  return dly;
	}
	box[v] = save_v.Sup();
	Interval sup = ope_.eval(box,constbox);
	dly = sup.rel0(op_);
	if (dly == 0) {
	  box[v] = save_v;
	  return dly;
	}
      } else if ((inc || dec) && op_ == EQ) {
	box[v] = save_v.Inf();
	Interval inf = ope_.eval(box,constbox);
	if (inc && inf.cgt(0) || dec && inf.clt(0)) {
	  box[v] = save_v;
	  return 0;
	}
	box[v] = save_v.Sup();
	Interval sup = ope_.eval(box,constbox);
	if (inc && sup.clt(0) || dec && sup.cgt(0)) {
	  box[v] = save_v;
	  return 0;
	}
      } 
    } catch (Growl growl) {}
    box[v] = save_v;
  }
  return cly;
}

Real RelExpr::d(const nat v) const {
  if (v < D_.size())
    return D_[v];
  return val(0);
}

void RelExpr::print(std::ostream &os) const {
  os << ope_ << " " << op_ << " 0";
}

std::ostream &kodiak::operator<< (std::ostream &os, const RelExpr &r) {
  r.print(os);
  return os;
}

std::ostream &kodiak::operator<< (std::ostream &os, const RelType rel) {
  switch (rel) {
  case EQ: os << "="; break;
  case LE: os << "<="; break;
  case LT: os << "<"; break;
  case GE: os << ">="; break;
  case GT: os << ">"; break;
  }
  return os;
}
