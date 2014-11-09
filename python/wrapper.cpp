#include "wrapper.hpp"
#include "math.h"
#include "algorithm"


using namespace kodiak;

Wrapper::Wrapper() {
  Kodiak::init();
  Kodiak::set_safe_input(true);
  Kodiak::set_debug(false);
  precision = -1;
  resolution = 0.01;
  paving_mode = kodiak::STD;
  equilibrium = false;
  var_select = 1;
  depth = 0;
  bp = false;
  out_file_name = "";
  no_outputs = false;
}

Wrapper::~Wrapper() {
}

Real Wrapper::build_poly(const std::string op, const Real r, const Real r2) {
  double n;
  if (op.compare("+") == 0)
    return r + r2;
  else if (op.compare("-") == 0)
    return r - r2;
  else if (op.compare("*") == 0)
    return r * r2;
  else if (op.compare("/") == 0)
    return r / r2;
  else if (op.compare("^") == 0) {
    if (r2.isVal()) {
      if (r2.isZero())
        return val(Interval::ONE());
      if (r2.isOne())
        return r;
      //check if exponent is an integer
      if (modf(r2.val().sup(), &n) == 0)
        return r ^ r2.val().sup();
    }
    throw Growl("exponent must be int");
  } else if (op.compare("rat") == 0) {
    return val(kodiak::rat(r.val().sup(), r2.val().sup()));
  } else if (op.compare("dec") == 0) {
    return val(kodiak::dec(r.val().sup(), r2.val().sup()));
  }
  return r;
}

Real Wrapper::build_poly(const std::string op, const Real r) {
  if (op.compare("u-") == 0) {
    return -r;
  } else if (op.compare("sin") == 0) {
    return Sin(r);
  } else if (op.compare("cos") == 0) {
    return Cos(r);
  } else if (op.compare("tan") == 0) {
    return Tan(r);
  } else if (op.compare("asin") == 0) {
    return Asin(r);
  } else if (op.compare("acos") == 0) {
    return Acos(r);
  } else if (op.compare("atan") == 0) {
    return Atan(r);
  } else if (op.compare("sq") == 0) {
    return Sq(r);
  } else if (op.compare("sqrt") == 0) {
    return Sqrt(r);
  } else if (op.compare("ln") == 0) {
    return Ln(r);
  } else if (op.compare("exp") == 0) {
    return Exp(r);
  } else if (op.compare("abs") == 0) {
    return Abs(r);
  }
  return r;
}

void Wrapper::pave() {
  bool debug = Kodiak::debug();
  Paver paver(name.empty() ? "" : name);

  paver.set_precision(precision);
  paver.set_debug(debug);
  paver.set_maxdepth(depth);

  assign_vars(paver);
  assign_consts(paver);


  assign_defs(paver);


  assign_constraints(paver);

  paver.set_resolutions(resolution);

  assign_resolutions(paver);

  if (paving_mode == kodiak::STD) {
    paver.pave();
  } else if (paving_mode == FIRST) {
    paver.first();
  } else if (paving_mode == FULL) {
    paver.full();
  }
  pav_ans = paver.answer();

  print(paver);
}

void Wrapper::bifurcation(bool equilibrium) {
  bool debug = Kodiak::debug();
  Bifurcation bifurcation(name.empty() ? "" : name);

  bifurcation.set_precision(precision);
  bifurcation.set_bp(bp);
  bifurcation.set_debug(Kodiak::debug());
  bifurcation.set_maxdepth(depth);

  // Supplies variables to problem
  assign_vars(bifurcation);

  // Supplies params to problem
  assign_params(bifurcation);

  // Supplies consts to problem
  assign_consts(bifurcation);


  assign_defs(bifurcation);

  // Supplies dfeq to problem
  assign_dfeqs(bifurcation);

  // Supplies constraint to problem
  assign_constraints(bifurcation);


  bifurcation.set_resolutions(resolution);

  // Supplies resolutions for variables to problem
  assign_resolutions(bifurcation);



  bifurcation.bifurcation(paving_mode == FIRST ? true : false, equilibrium);

  pav_ans = bifurcation.answer();

  print(bifurcation);
};

void Wrapper::minmax(std::string mode) {
  bool debug = Kodiak::debug();
  MinMaxSystem mms(name.empty() ? "" : name);
  mms.set_debug(debug);
  mms.set_bp(bp);

  mms.set_varselect(var_select);
  mms.set_maxdepth(depth);
  mms.set_precision(precision);

  mms.set_granularity(granularity);

  assign_vars(mms);
  assign_consts(mms);
  assign_constraints(mms);
  assign_defs(mms);

  mms.set_resolutions(resolution);

  assign_resolutions(mms);

  if (mode.compare("minmax") == 0) {
    mms.minmax(objfn);
  } else if (mode.compare("max") == 0) {
    mms.max(objfn);
  } else if (mode.compare("min") == 0) {
    mms.min(objfn);
  }

  print(mms);

}
template<class T>
void Wrapper::assign_vars(T &prob) {
  std::vector<std::string>::iterator it = var_names.begin();
  std::vector<Interval>::iterator it2 = var_intervals.begin();
  for (; it != var_names.end(); ++it, ++it2) {
    prob.var(*it, *it2);
  }
}

template<class T>
void Wrapper::assign_params(T &prob) {
  std::vector<std::string>::iterator it = param_names.begin();
  std::vector<Interval>::iterator it2 = param_intervals.begin();
  for (; it != param_names.end(); ++it, ++it2) {
    prob.param(*it, *it2);
  }
}

template<class T>
void Wrapper::assign_consts(T &prob) {
  std::vector<std::string>::iterator it = const_names.begin();
  std::vector<Interval>::iterator it2 = const_vals.begin();
  for (; it != const_names.end(); ++it, ++it2) {
    prob.cnst(*it, *it2);
  }
}

template<class T>
void Wrapper::assign_dfeqs(T &prob) {
  std::vector<Real>::iterator it = dfeqs.begin();
  for (; it != dfeqs.end(); ++it) {
    prob.dfeq(*it);
  }
}

template<class T>
void Wrapper::assign_defs(T &prob) {
  std::vector<std::pair<std::string, Real> >::iterator it = defs.begin();
  for (; it != defs.end(); ++it) {
    if (Kodiak::debug())
      std::cout << "definition " << (*it).first << " = " << (*it).second << std::endl;
    prob.def((*it).first, (*it).second);
  }
}


template<class T>
void Wrapper::assign_constraints(T &prob) {
  std::vector<Real>::iterator it = constraints.begin();
  std::vector<std::string>::iterator it2 = operators.begin();
  for (; it != constraints.end(); ++it, ++it2) {
    if ((*it2).compare("=") == 0) {
      prob.eq0((*it));
    } else if ((*it2).compare("<") == 0) {
      prob.lt0((*it));
    } else if ((*it2).compare("<=") == 0) {
      prob.le0((*it));
    } else if ((*it2).compare(">") == 0) {
      prob.gt0((*it));
    } else if ((*it2).compare(">=") == 0) {
      prob.ge0((*it));
    }
  }
}


template<class T>
void Wrapper::assign_resolutions(T &prob) {
  std::vector<std::pair<std::string, real> >::iterator it = resolutions.begin();
  for (; it != resolutions.end(); ++it) {
    prob.set_resolution((*it).first, (*it).second);
  }
}


void Wrapper::add_var(std::string name, Interval i) {
  var_names.push_back(name);
  var_intervals.push_back(i);

};

void Wrapper::add_param(std::string name, Interval i) {
  param_names.push_back(name);
  param_intervals.push_back(i);

};

void Wrapper::add_dfeq(Real dfeq) {
  dfeqs.push_back(dfeq);
};

void Wrapper::add_constraint(Real c, std::string op) {
  constraints.push_back(c);
  operators.push_back(op);
};

void Wrapper::add_const(std::string name, Interval val) {
  const_names.push_back(name);
  const_vals.push_back(val);
}

void Wrapper::add_def(std::string name, Real val) {
  std::pair<std::string, Real> p(name, val);
  defs.push_back(p);
}

void Wrapper::set_resolution(std::string name, real n) {
  std::pair<std::string, real> res;
  res.first = name;
  res.second =  n;
  resolutions.push_back(res);
};

template<class T>
void Wrapper::print(T &prob) {
  if (!no_outputs) {
    if (out_file_name.empty()) {
      prob.print();
      std::cout << std::endl;
    } else {
      std::ofstream out;
      if (file_exists(out_file_name.c_str())) {
        out.open(out_file_name.c_str(), std::ios_base::app);
      } else {
        out.open(out_file_name.c_str());
      }
      std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
      std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
      prob.print();
      std::cout << std::endl;
      std::cout.rdbuf(coutbuf);
    }
  }
}

void Wrapper::plot2D(std::string name, std::string var1, std::string var2) {
  pav_ans.gnuplot(name, var1, var2, kodiak::Paver::default_style(false, false));
};


void Wrapper::plot3D(std::string name, std::string var1, std::string var2, std::string var3) {
  pav_ans.gnuplot(name, var1, var2, var3, kodiak::Paver::default_style(false, false));
};

std::vector<Boxes*> Wrapper::get_boxes(){
  std::vector<Boxes*> boxes;
  for(int i = 0; i < pav_ans.ntypes(); i++){
    boxes.push_back(&(pav_ans.boxes(i)));
  }
  return boxes;
};
