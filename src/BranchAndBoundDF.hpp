#ifndef _BRANCHANDBOUNDDF_
#define _BRANCHANDBOUNDDF_

#include "types.hpp"

namespace kodiak {

  template <typename Expression, typename Answer> class BranchAndBoundDF {
  public:
    nat splits() const { return splits_; }
    nat depth() const { return depth_; }
    nat maxdepth() const { return maxdepth_; }
    nat debug() const { return debug_; }
    clock_t time() const { return time_; }
    void print_info(std::ostream &os=std::cout) const {
      os << "Splits: " << splits() << ", Depth: " << depth() 
	 << ", Time: " << (time()/1000.0)  << "s";
    }
    void set_maxdepth(nat maxdepth) { maxdepth_ = maxdepth; }
    void set_debug(const nat debug = 1) { debug_ = debug; }
    
    const Expression &theExpr() const { return expr_; }
    const Box &theBox() const { return box_; }

  protected:
    BranchAndBoundDF() : maxdepth_(0), depth_(0), splits_(0), time_(0), debug_(0) {}
    
    // Basic enclosure function
    virtual void evaluate(Answer &, Expression &, Box &) = 0;

    // First answer was computed by first recursion.
    virtual void combine(Answer &, const DirVar &, const Answer &) = 0;
    // Second answer was computed by second recursion.
    virtual void combine(Answer &, const DirVar &, const Answer &, const Answer &) = 0;
    virtual void branch(Expression &, Box &) {}
    virtual void unbranch(Expression &, Box &) {}
    virtual void accumulate(const Answer &) {} 
    virtual bool local_exit(const Answer &) { return false; }
    virtual bool global_exit(const Answer &) { return false; }
    virtual bool prune(const Answer &) { return false; }
    virtual bool sound(const Answer &, const Expression &, const Box &) { 
      return true; 
    }
    virtual void select(DirVar &dirvar, Expression &e, Box &box) {
      round_robin(dirvar,e,box);
    }
    void round_robin(DirVar &dirvar, Expression &, Box &box) {
      dirvar.init(box.size());
      nat next_var;
      if (dirvars_.empty())
	next_var = 0;
      else 
	next_var = dirvars_.back().var+1;
      for (nat round = 0; round < box.size(); round++) {
	if (next_var >= box.size())
	  next_var = 0;
	if (box[next_var].isPoint())
	  ++next_var;
	else {
	  dirvar.var = next_var;
	  dirvar.dir = (dirvars_.size() % 2) == 0;
	  break;
	}
      }
    }

    virtual void split(const DirVar &dirvar, Box &box, const Interval &i, const real mid) {
      if (dirvar.dir) 
	box[dirvar.var] = Interval(i.inf(),mid);
      else 
	box[dirvar.var] = Interval(mid,i.sup());
    }

    // Branch and bound algorithm
    // e: expression
    // box: box of variables (variables are indexed by nat)
    void bandb(Answer &answer, const Expression &expr, const Box &box) {
      expr_ = expr;
      box_ = box;
      dirvars_.clear();
      splits_ = 0;
      depth_ = 0;
      global_exit_ = false;
      
      clock_ = clock();
      Expression nexpr = expr;
      Box nbox = box;
      bandb_(answer,nexpr,nbox);
      time_= 1000*(clock()-clock_)/CLOCKS_PER_SEC;
      assert(debug_ == 0 || sound(answer,expr_,box_));
    }

    const DirVars& dirvars() const { return dirvars_; }
    DirVars& dirvars() { return dirvars_; }

    private:

    void bandb_(Answer &answer, const Expression &expr, Box &box) {	
      nat depth = dirvars_.size();
      if (depth > depth_) 
	depth_ = depth;
      Expression e = expr;
      evaluate(answer,e,box);
      accumulate(answer);
      ++splits_;
      global_exit_ = global_exit(answer);
      if (box.empty() || (maxdepth_ > 0 && depth+1 >= maxdepth_) ||
	  global_exit_ || local_exit(answer) || prune(answer))
	return;
      DirVar dirvar;
      select(dirvar,e,box);
      if (dirvar.var >= box.size())
	return;
      Interval i = box[dirvar.var];
      real mid = i.mid();
      split(dirvar,box,i,mid);
      dirvars_.push_back(dirvar);
      Answer ans1;
      branch(e,box);
      bandb_(ans1,e,box);
      unbranch(e,box);
      if (global_exit_) dirvar.onlyone = true;
      if (dirvar.onlyone) {
	dirvars_.pop_back();
	box[dirvar.var] = i;
	combine(answer,dirvar,ans1);
	return;
      }
      dirvar.next();
      dirvars_.pop_back();
      split(dirvar,box,i,mid);
      dirvars_.push_back(dirvar);
      Answer ans2;
      branch(e,box);
      bandb_(ans2,e,box);
      unbranch(e,box);
      dirvars_.pop_back();
      box[dirvar.var] = i;
      combine(answer,dirvar,ans1,ans2);
    }
    
    // Maxdepth 0 means loop until one leaf satisfies global exit or until all 
    // the leaves satisfy either prune or local exit 
    nat  maxdepth_;    // Maximum depth
    nat  depth_;       // Current depth
    nat  splits_;      // Current splits
    DirVars dirvars_;  // Stack of direction and variable selection
    clock_t clock_;    // Variable for timing
    clock_t time_;     // Time in milliseconds
    Expression expr_;  // Original expression
    Box box_;          // Original box
    nat debug_;        // If debug >= 0, assert sound predicate
    bool global_exit_; // Global exit
  };
}

#endif
