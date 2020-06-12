#include "BooleanChecker.hpp"

using namespace kodiak;

void BooleanChecker::setExpression(const Bool &expr) {
  // TODO: Index expression
  this->_booleanExpression = expr;
}

void BooleanChecker::printDebuggingInfo(std::ostream &os) const {
  if (this->witness_.get() != nullptr) {
    os << this->witness_->box;
    os << std::endl;
  }
  print_info(os);
  os << std::endl;
  printSystem(os, 10);
}

bool BooleanChecker::smallestBoxSubdivisionReached(Environment &env) const {
  bool noLargeBoxFound = true;
  for (nat var = 0; noLargeBoxFound && var < env.box.size(); var++) {
    if (env.box[var].diam() > this->variableResolutionsAkaSmallestRangeConsideredForEachVariable_[var]) {
      noLargeBoxFound = false;
    }
  }
  return noLargeBoxFound;
}

void BooleanChecker::select(DirVar &dirvar,
                            Bool &e,
                            Environment &box) {
  if (smallestBoxSubdivisionReached(box)) {
    dirvar.var = box.size();
    return;
  }
  round_robin(dirvar, e, box);
}

Certainty BooleanChecker::check() {
  Environment env{EmptyBBox, variablesEnclosures_.box(), EmptyNamedBox};
  Certainty answer;
  branchAndBound(answer, _booleanExpression, env);
  if (global_exit(answer)) {
    witness_ = std::make_unique<Environment>(env);
  }
  return answer;
}

void BooleanChecker::evaluate(Certainty &answer,
                              Bool &expr,
                              Environment &env) {
  Certainty certainty =
          expr.eval(env,
                    defaultEnclosureMethodTrueBernsteinFalseInterval_,
                    absoluteToleranceForStoppingBranchAndBound_);
  answer = certainty;
}

void BooleanChecker::combine(Certainty &answer,
                             const DirVar &,
                             const Certainty &answer1) {
  answer = answer1;
}

void BooleanChecker::combine(Certainty &answer,
                             const DirVar &,
                             const Certainty &answer1,
                             const Certainty &answer2) {
  answer = answer1 && answer2;
}

bool BooleanChecker::local_exit(const Certainty &answer) {
  return answer == Certainty::TRUE;
}

bool BooleanChecker::global_exit(const Certainty &answer) {
  const bool hasToExit = answer == Certainty::FALSE;
  return hasToExit;
}

