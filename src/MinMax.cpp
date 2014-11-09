#include "MinMax.hpp"

using namespace kodiak;

MinMax::MinMax() : morm_(0) {
  mm_ = Interval::Empty(); 
} 

void MinMax::init(const int morm) {
  morm_ = morm; 
  mm_ = Interval::Empty(); 
  min_point_.clear(); 
  max_point_.clear();
}

Interval MinMax::min() const {
  if (empty() || min_point_.empty()) 
    return Interval::Empty();
  return Interval(mm_.inf(),ub_of_min_); 
}

Interval MinMax::max() const {
  if (empty() || max_point_.empty())
    return Interval::Empty();
  return Interval(lb_of_max_,mm_.sup());
}

Interval MinMax::inclosure() const {
  if (min_point_.empty() || max_point_.empty())
    return Interval::Empty();
  return Interval(ub_of_min_,lb_of_max_);
}

bool MinMax::sound() const {			
  return mm_.empty() ||
    ((min_point_.empty() || mm_.inf() <= ub_of_min_) && 
     (max_point_.empty() || lb_of_max_ <= mm_.sup())); 
}

real MinMax::lb_of_min() const {
  if (!empty())
    return mm_.inf();
  throw Growl("Kodiak (lb_of_min): lower bound of min not found"); 
}

real MinMax::ub_of_min() const {
  if (!min_point_.empty())
    return ub_of_min_;
  throw Growl("Kodiak (ub_of_min): upper bound of min not found"); 
}

real MinMax::lb_of_max() const {
  if (!min_point_.empty())
    return lb_of_max();
  throw Growl("Kodiak (lb_of_max): lower bound of max not found"); 
}

real MinMax::ub_of_max() const {
  if (!empty())
    return mm_.sup();
  throw Growl("Kodiak (ub_of_max): upper bound of max not found"); 
}

MinMaxType MinMax::min_or_max() const {
  switch (morm_) {
  case -1 : return MIN;
  case 1 : return MAX;
  default: return MINMAX;
  }
}

void MinMax::print(std::ostream &os) const {
  if (morm_ <= 0) {
    os << "min in " << min() << ". Diameter: " << min().diam() << std::endl;
    os << "ub_of_min point: " <<  min_point() << std::endl;
  }
  if (morm_ >= 0) {
    os << "max in " << max() << ". Diameter: " << max().diam() << std::endl;
    os << "lb_of_max point: " << max_point() << std::endl;
  }
  if (morm_ == 0) {
    os << "outclosure: " << outclosure() << std::endl;
    os << "inclosure: " << inclosure() << std::endl;
  }
}
