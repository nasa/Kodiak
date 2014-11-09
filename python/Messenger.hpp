// This class deals with reading and writing from files using Google protobuf.
// Reading blocks the reader until another process produces an answer.
// The structure of the shared file described in kodiakmessages.proto
// Usage:
//      Messenger messenger(message_file);
//      Problem prob;
//      messenger.write(prob);
//      std::vector<Boxes> ans;
//      messenger.read(ans, 'b');

#ifndef _MESSENGER_
#define _MESSENGER_
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Problem.hpp"
#include "Bifurcation.hpp"
#include "kodiakmessages.pb.h"

typedef std::pair<double, double> Interval;
typedef std::vector<Interval> Box;
typedef std::vector<Box> Boxes;

class Messenger {
public:
  Messenger(std::string message_file);
  ~Messenger();

  // Write methods take a problem of specific type and write it to the shared file.
  void write(Bifurcation bifurcation);
  void write(Problem problem);

  // Read methods read the answer from the shared file.
  void read(std::vector<Boxes> &answer, const char type);

  // Clears the contents of the shared file
  void clear();

private:
  std::string f_name;
  // Holds the contents of the shared file
  kodiak::KodiakMessages kodiak_messages;
};
#endif
