#ifndef KODIAK_NEWPAVER_HPP
#define KODIAK_NEWPAVER_HPP

#include <memory>

#include "Paver.hpp"
#include "Expressions/Boolean/Bool.hpp"

namespace kodiak {

    using namespace kodiak::BooleanExpressions;

    class NewPaver : public Paver {
    public:
        using Paver::Paver;

        nat numberOfRelationalFormulas() const {
            return this->booleanExpression_ != nullptr ? 1 : 0;
        }

        void print(std::ostream &os) const {
            os << "System: " << ID << std::endl;
            os << "Vars: ";
            variablesEnclosures_.print(os);
            os << std::endl;
            if (!constantsEnclosures_.empty()) {
                os << "Consts: ";
                constantsEnclosures_.print(os);
                os << std::endl;
            }
            os << "Formula: ";
            this->booleanExpression_->print(os);
            os << std::endl;
            for (nat i = 0; i < paving_.ntypes(); i++)
                os << style.title(i) << ": " << paving_.size(i) << " boxes" << std::endl;
            print_info(os);
            os << std::endl;
            paving_.save(ID, style.titles());
        }

    protected:

        void evaluate(PrePaving &paving, Ints &certainties, Environment &box) override {
            currentBoxCertainty_ = 1;
            nat numberOfFormulas = 1;
            Box aBox = box.box;
            for (nat i = 0; i < numberOfFormulas; ++i) {
                bool isCurrentFormulaTrue = certainties[i] > 0;
                if (isCurrentFormulaTrue) continue;

//                bool isSelectionNonEmpty = not dirvars().empty();
//                if (isSelectionNonEmpty) {
//                    Real currentFormulaLHS = relationalFormulas_[i].ope();
//                    nat mostRecentSelectionVariable = dirvars().back().var;
//                    if (not currentFormulaLHS.hasVar(mostRecentSelectionVariable)
//                        /* TODO: Remove the following line and other users trivially true
//                         * that seems useless:
//                         * && where(dirvars(), dirvars().back().var) != EXTERIOR */
//                            ) {
//                        // top variable doesn't appear in i-th formula, don't check it again
//                        currentBoxCertainty_ = (currentBoxCertainty_ > 0 ? certainties[i] : std::max(currentBoxCertainty_, certainties[i]));
//                        continue;
//                    }
//                }
                evalGlobalDefinitions(); // TODO: Take out from the loop, since it should be constant or, if not, it would be asymmetric
                certainties[i] = CertaintyClass::certainty2Int(
                        this->booleanExpression_->eval(box, this->defaultEnclosureMethodTrueBernsteinFalseInterval_,
                                                       this->absoluteToleranceForStoppingBranchAndBound_));
                // if i-th formula is certainly not true, conjunction of formulas is certainly not true
                if (certainties[i] == 0) {
                    currentBoxCertainty_ = 0;
                    if (search_ == FULL)
                        paving.push_box(3, aBox); // Whole interval (certainly not)
                    return;
                }
                if (certainties[i] < 0)
                    currentBoxCertainty_ = (currentBoxCertainty_ > 0 ? certainties[i] : std::max(currentBoxCertainty_,
                                                                                                 certainties[i]));
            }
            if (currentBoxCertainty_ > 0) // Conjunction of formulas is certainly true
                paving.push_box(0, aBox); // Whole interval (certainly)
            else // Conjunction of formulas is possibly true
                paving.push_box(std::abs(currentBoxCertainty_), aBox); // Whole interval (possibly)
        }

        virtual void select(DirVar &dirvar, Ints &certainties, Environment &box) {
            round_robin(dirvar, certainties, box);
        }

    public:
        void setBooleanExpression(const kodiak::BooleanExpressions::Node &expr) {
            // TODO: Index expression
            this->booleanExpression_ = expr.clone();
        }

        void setBooleanExpression(const std::unique_ptr<kodiak::BooleanExpressions::Node> expr) {
            // TODO: Index expression
            this->booleanExpression_ = expr->clone();
        }

        const kodiak::BooleanExpressions::Node &getBooleanExpression() const {
            return *(this->booleanExpression_);
        }

    private:
        std::unique_ptr<kodiak::BooleanExpressions::Node> booleanExpression_;
    };

}

#endif //KODIAK_NEWPAVER_HPP
