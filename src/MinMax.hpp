#ifndef _MINMAX_
#define _MINMAX_

#include "Real.hpp"

namespace kodiak {

  enum MinMaxType { MIN, MINMAX, MAX };

  class MinMax {
  public:
    friend class MinMaxSystem;
    MinMax();
    void init(const int);
    Interval min() const;
    Interval max() const;
    const Interval &outclosure() const { return mm_; }
    Interval inclosure() const;
    real lb_of_min() const;
    real ub_of_min() const;
    real lb_of_max() const;
    real ub_of_max() const;
    bool sound() const;			
    bool empty() const { return mm_.empty(); }
    const Point& min_point() const { return min_point_; }
    const Point& max_point() const { return max_point_; }
    MinMaxType min_or_max() const;
    void print(std::ostream & = std::cout) const;
  private:
    void resize(nat v) { min_point_.resize(v); max_point_.resize(v); }
    Interval mm_;      // Enclosure of e(x) for all x in box  
    real  ub_of_min_;  // Upper bound of min e(x) for all x in box
    Point min_point_;  // Point such that min() <= e(min_point_) <= ub_of_min_
    real  lb_of_max_;  // Lower bound of max e(x) for all x in box
    Point max_point_;  // Point where  lb_of_max_ <= e(max_point_) <= max()
    int   morm_;       // 0: min & max, -1: min, 1: max
  };

}

#endif
