// Defines bifurcation problems.
// Extends Problem class and adds extra
// methods to read the data for bifurcations
// intented to be only used in the mexFunction
#ifndef _BIFURCATION_
#define _BIFURCATION_
#include "Problem.hpp"

class Bifurcation: public Problem {
public:
  Bifurcation(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
  std::vector<std::string> const get_param_names() const {
    return param_names;
  };
  std::vector<Interval> const get_param_ints() const {
    return params;
  };
  std::vector<std::string> const get_dfeqs() const {
    return dfeqs;
  };
  void make_answer(std::vector<Boxes> &boxes);
private:
  std::vector<std::string> param_names;
  std::vector<Interval> params;
  std::vector<std::string> dfeqs;
};
#endif