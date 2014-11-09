#include "mex.h"
#include "Problem.hpp"
#include <iostream>
#include <sstream>
#include <string>

//Extracts values form prhs, which are right hand side values in matlab.
Problem::Problem(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
  //If field_number != 1 checks are necessary to allow rearanging and skipping fields in input structure
  int field_number = mxGetFieldNumber(prhs[1],"res");
  if(field_number != -1)
    res = mxGetScalar(mxGetFieldByNumber(prhs[1], 0, field_number));
  field_number = mxGetFieldNumber(prhs[1],"prec");
  if(field_number != -1)
    prec = int(mxGetScalar(mxGetFieldByNumber(prhs[1], 0, field_number)));
  field_number = mxGetFieldNumber(prhs[1],"vars");
  if(field_number != -1)
    extract_vars(mxGetFieldByNumber(prhs[1], 0, field_number), var_names, vars);
  field_number = mxGetFieldNumber(prhs[1],"cnstrs");
  if(field_number != -1)
    extract_strings(mxGetFieldByNumber(prhs[1], 0, field_number), cnstrs);
  this->plhs = plhs;
  this->nlhs = nlhs;
};

//Extracts variables and stores them in names and interv.
void Problem::extract_vars(const mxArray *prhs, std::vector<std::string> &names, std::vector<Interval> &interv) {
  if (mxIsCell(prhs)) {
    int elems = mxGetNumberOfElements(prhs);
    for (int i = 0, o = (elems / 3), p = (elems / 3 * 2); i < elems / 3; ++i, ++o, ++p) {
      char name[255];
      mxGetString(mxGetCell(prhs, i), name, 254);
      names.push_back(name);
      double lb, ub;
      lb = mxGetScalar(mxGetCell(prhs, o));
      ub = mxGetScalar(mxGetCell(prhs, p));
      interv.push_back(std::make_pair(lb, ub));
      // mexPrintf("var %s, lb = %f, ub = %f\n", name, lb, ub);
    };

  };
};

//Extracts strings and stores them in s.
void Problem::extract_strings(const mxArray *prhs, std::vector<std::string> &s) {
  if (mxIsCell(prhs)) {
    int elems = mxGetNumberOfElements(prhs);
    for (int i = 0; i < elems; ++i) {
      char name[255];
      mxGetString(mxGetCell(prhs, i), name, 254);
      s.push_back(name);
    }
  };
};

void Problem::make_interval_mx(Box box, double *mx) {
  for (int p = 0, o = box.size(); p < box.size(); ++p, ++o) {
    mx[p] = box[p].first;
    mx[o] = box[p].second;
  }
}

void Problem::make_boxes_mx(Boxes boxes, mxArray *mx) {
  for (int i = 0; i < boxes.size(); ++i) {
    // Creates a matrix(boxes.size(), 2) and stores the interval inside the box
    mxArray *box = mxCreateDoubleMatrix(boxes[i].size(), 2, mxREAL);
    make_interval_mx(boxes[i], mxGetPr(box));
    mxSetCell(mx, i, mxDuplicateArray(box));
  };
}

void Problem::make_answer(std::vector<Boxes> boxes, const char *field_names[], const char *types[]) {
  mxArray *ans = mxCreateStructMatrix(boxes.size(), 1, 2, field_names);
  for (int a = 0; a < boxes.size(); ++a) {
    mxSetFieldByNumber(ans, a, 0, mxCreateString(types[a]));
    //Creates a cell matrix(boxes.size(),1) and populates it with individual boxes
    mxArray *boxes_mx = mxCreateCellMatrix(boxes[a].size(), 1);
    make_boxes_mx(boxes[a], boxes_mx);
    mxSetFieldByNumber(ans, a, 1, mxDuplicateArray(boxes_mx));
  }
  plhs[0] = mxDuplicateArray(ans);
};