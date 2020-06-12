#ifndef KODIAK_BOOLEANCHECKER_HPP
#define KODIAK_BOOLEANCHECKER_HPP

#include "Real.hpp"
#include "NewPaver.hpp"
#include "Expressions/Boolean/Bool.hpp"

namespace kodiak {

using namespace BooleanExpressions;

class BooleanChecker : public System, public BranchAndBoundDF<Bool, Certainty, Environment> {

public:

  BooleanChecker() : _booleanExpression(True) {}

  void setExpression(const Bool &expr);

  void printDebuggingInfo(std::ostream &os = std::cout) const;

  Certainty check();

protected:

  void evaluate(Certainty &answer, Bool &expr, Environment &env) override;

  void combine(Certainty &answer, const DirVar &dirvar, const Certainty &answer1) override;

  void combine(Certainty &answer, const DirVar &dirvar, const Certainty &answer1, const Certainty &answer2) override;

  bool local_exit(const Certainty &answer) override;

  bool global_exit(const Certainty &answer) override;

  bool smallestBoxSubdivisionReached(Environment &env) const;

  virtual void select(DirVar &dirvar, Bool &e, Environment &box) override;

private:
  Bool _booleanExpression;
  std::unique_ptr<Environment> witness_;
};

}

#endif
