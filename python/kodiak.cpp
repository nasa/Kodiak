// Main entrance file for interface to Kodiak C++ library for matlab.
// Matlab can only call mexFunction inside this file.
// First input from matlab is used to determine what kind of problem is passed.
// Currently only bifurcations supported.
// Uses Google protobuff for IPC communication via files.
// All the management is contained in Messenger class

#include "mex.h"
#include "kodiakmessages.pb.h"
#include "Messenger.hpp"
// File in which temporary messages are stored
#define FILE_NAME "kodiak_messages"


void mexFunction (int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
  Messenger messenger(FILE_NAME);
  // Creates a temporary buffer to hold the string defining the type of problem
  char buf[10];
  mxGetString(prhs[0], buf, 5);
  switch (buf[0]) {
  case 'b':
    // If type of problems starts as 'b', it's bifurcation problem
    // Create a new bifurcation
    Bifurcation bifurcation(nlhs, plhs, nrhs, prhs);
    // Write the bifurcation definition to shared file
    messenger.write(bifurcation);
    // Temporary holder for ans;
    std::vector<Boxes> ans;
    // Read the answer from the shared file to ans
    messenger.read(ans, buf[0]);
    // Convert the answer to matlab format
    bifurcation.make_answer(ans);
    break;
  }
}
