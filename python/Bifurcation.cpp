#include "Bifurcation.hpp"

Bifurcation::Bifurcation(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]): Problem (nlhs, plhs, nrhs, prhs) {
  int field_number = mxGetFieldNumber(prhs[1], "params");
  if (field_number != -1)
    extract_vars(mxGetFieldByNumber(prhs[1], 0, field_number), param_names, params);
  field_number = mxGetFieldNumber(prhs[1], "dfeqs");
  if (field_number != -1)
    extract_strings(mxGetFieldByNumber(prhs[1], 0, field_number), dfeqs);
};

// Provides required parameters for Problem::make_answer
void Bifurcation::make_answer(std::vector<Boxes> &boxes) {
  mwSize dims[2] = {1, int(boxes.size())};
  const char *field_names[] = {"type", "boxes"};
  const char *types[] = {"Possibly Limit Point Bifurcation", "Possibly Hopf Bifurcation"};
  Problem::make_answer(boxes, field_names, types);
};

