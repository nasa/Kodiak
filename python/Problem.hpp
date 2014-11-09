// A generic class used for problem definitions.
// All problems types should extend it.
// Not intended to be used on its own.
#ifndef _PROBLEM_
#define _PROBLEM_
#include <vector>
#include <string>
#include "mex.h"

typedef std::pair<double, double> Interval;
typedef std::vector<Interval> Box;
typedef std::vector<Box> Boxes;

class Problem {
public:
  //Constructor takes all the inputs from matlab
  // and extracts necessary values from them.
  Problem(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
  //Extracts variable from input mxArray and uses names and i as outputs
  void extract_vars(const mxArray *prhs, std::vector<std::string> &names, std::vector<Interval> &i);
  //Extracts strings from input mxArray and uses s as output
  void extract_strings(const mxArray *prhs, std::vector<std::string> &s);
  
  double get_res(){return res;};
  int get_prec(){return prec;};

  std::vector<std::string> const get_var_names() const {return var_names;};
  std::vector<Interval> const get_var_ints() const {return vars;};
  std::vector<std::string> const get_cnstrs() const {return cnstrs;};
  //Makes an interval matrix from intervals in the box, second param used as output
  void make_interval_mx(Box, double *);
  //Makes an matrix of boxes from boxes in the boxes, second param used as output  
  void make_boxes_mx(Boxes, mxArray *);
  //Makes a complete answer and stores it in plhs[0] which is the answer in matlab.
  void make_answer(std::vector<Boxes> boxes, const char *field_names[] ,const char *types[]);

  mxArray **plhs;
  int nlhs;

private:
  
  double res;
  int prec;
  
  std::vector<std::string> var_names;
  std::vector<Interval> vars;
  std::vector<std::string> cnstrs;
};
#endif