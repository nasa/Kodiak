#include <types.hpp>
#include <iostream>
#include "Codiak.h"
#include "Real.hpp"
#include "MinMaxSystem.hpp"
#include "Adapters/PRECiSA.hpp"

using namespace kodiak;
using namespace kodiak::Adapters::PRECiSA;

CInterval interval_create(double lb, double ub) {
    return new Interval(lb,ub);
}

CInterval interval_create_from_rational(CInt numerator, CUInt denominator) {
    return new Interval(rat(numerator, denominator));
}

void interval_print(CInterval p) {
    Interval *pInterval = static_cast<Interval *>(p);
    std::cout << "An the interval is: ";
    pInterval->print(std::cout);
    std::cout << std::endl;
}

CReal real_create_value(CInterval p) {
    const Interval interval = *static_cast<Interval *>(p);
    return new Real(val(interval));
}

CReal real_create_variable(CUInt index, CString name) {
    std::string castedName = static_cast<const char*>(name);
    return new Real(var(index, castedName));
}

void real_print(CReal p) {
    Real *pReal = static_cast<Real *>(p);
    std::cout << "Real(";
    std::cout << *pReal;
    std::cout << ")";
    std::cout << std::endl;
}

CRealVector real_vector_create() {
    return new std::vector<Real>();
}

void real_vector_add(CRealVector pv, CReal p) {
    Real *pReal = static_cast<Real *>(p);
    std::vector<Real> *pRealsVector = static_cast<std::vector<Real> *>(pv);
    pRealsVector->push_back(*pReal);
}

void real_vector_print(CRealVector p) {
    std::vector<Real> *pReals = static_cast<std::vector<Real> *>(p);
    bool firstIteration = true;
    std::cout << "Vector{";
    for (auto &real : *pReals) {
        if (!firstIteration) {
            std::cout << ",";
        }
        std::cout << "Real(";
        std::cout << real;
        std::cout << ")";
        firstIteration = false;
    }
    std::cout << "}" << std::endl;
}

CString real_to_string(CReal p) {
    Real *pReal = static_cast<Real *>(p);
    std::ostringstream os;
    os << "Real(";
    os << *pReal;
    os << ")";
    os << std::endl;
    return os.str().c_str();
}

CReal real_create_maximum(CRealVector p) {
    std::vector<Real> *pReals = static_cast<std::vector<Real> *>(p);
    return new Real(Max(*pReals));
}

CReal real_create_addition(CReal pLeft, CReal pRight) {
    Real *pLeftReal = static_cast<Real *>(pLeft);
    Real *pRightReal = static_cast<Real *>(pRight);
    return new Real(*pLeftReal + *pRightReal);
}

CReal real_create_subtraction(CReal pLeft, CReal pRight) {
    Real *pLeftReal = static_cast<Real *>(pLeft);
    Real *pRightReal = static_cast<Real *>(pRight);
    return new Real(*pLeftReal - *pRightReal);
}

CReal real_create_multiplication(CReal pLeft, CReal pRight) {
    Real *pLeftReal = static_cast<Real *>(pLeft);
    Real *pRightReal = static_cast<Real *>(pRight);
    return new Real(*pLeftReal * *pRightReal);
}

CReal real_create_division(CReal pLeft, CReal pRight) {
    Real *pLeftReal = static_cast<Real *>(pLeft);
    Real *pRightReal = static_cast<Real *>(pRight);
    return new Real(*pLeftReal / *pRightReal);
}

CReal real_create_absolute_value(CReal p) {
    Real *pReal = static_cast<Real *>(p);
    return new Real(Abs(*pReal));
}

CReal real_create_sine(CReal p) {
    Real *pReal = static_cast<Real *>(p);
    return new Real(Sin(*pReal));
}

CReal real_create_cosine(CReal p) {
    Real *pReal = static_cast<Real *>(p);
    return new Real(Cos(*pReal));
}

CReal real_create_arctangent(CReal p) {
    Real *pReal = static_cast<Real *>(p);
    return new Real(Atan(*pReal));
}

CReal real_create_eexponent(CReal p) {
    Real *pReal = static_cast<Real *>(p);
    return new Real(Exp(*pReal));
}

CReal real_create_elogarithm(CReal p) {
    Real *pReal = static_cast<Real *>(p);
    return new Real(Ln(*pReal));
}

CReal real_create_floor(CReal p) {
    Real *pReal = static_cast<Real *>(p);
    return new Real(Floor(*pReal));
}

CReal real_create_double_ulp(CReal p) {
    Real *pReal = static_cast<Real *>(p);
    return new Real(DUlp(*pReal));
}

CReal real_create_single_ulp(CReal p) {
    Real *pReal = static_cast<Real *>(p);
    return new Real(SUlp(*pReal));
}

CReal real_create_negation(CReal p) {
    Real *pReal = static_cast<Real *>(p);
    return new Real(-(*pReal));
}


//////////////////////////////////////////////////////////////////////
//
// Real Error Expressions
//
//////////////////////////////////////////////////////////////////////

CReal real_create_error_negation(CReal p, CReal pe) {
    Real *pReal = static_cast<Real *>(p);
    Real *pErrorReal = static_cast<Real *>(pe);
    return new Real(aebounddp_neg(*pReal, *pErrorReal));
}

CReal real_create_error_addition(CReal pLeft, CReal peLeft, CReal pRight, CReal peRight) {
    Real *pLeftReal = static_cast<Real *>(pLeft);
    Real *pLeftErrorReal = static_cast<Real *>(peLeft);
    Real *pRightReal = static_cast<Real *>(pRight);
    Real *pRightErrorReal = static_cast<Real *>(peRight);
    return new Real(aebounddp_add(*pLeftReal, *pLeftErrorReal, *pRightReal, *pRightErrorReal));
}

CReal real_create_error_subtraction(CReal pLeft, CReal peLeft, CReal pRight, CReal peRight) {
    Real *pLeftReal = static_cast<Real *>(pLeft);
    Real *pLeftErrorReal = static_cast<Real *>(peLeft);
    Real *pRightReal = static_cast<Real *>(pRight);
    Real *pRightErrorReal = static_cast<Real *>(peRight);
    return new Real(aebounddp_sub(*pLeftReal, *pLeftErrorReal, *pRightReal, *pRightErrorReal));
}

CReal real_create_error_multiplication(CReal pLeft, CReal peLeft, CReal pRight, CReal peRight) {
    Real *pLeftReal = static_cast<Real *>(pLeft);
    Real *pLeftErrorReal = static_cast<Real *>(peLeft);
    Real *pRightReal = static_cast<Real *>(pRight);
    Real *pRightErrorReal = static_cast<Real *>(peRight);
    return new Real(aebounddp_mul(*pLeftReal, *pLeftErrorReal, *pRightReal, *pRightErrorReal));
}

CReal real_create_error_division(CReal pLeft, CReal peLeft, CReal pRight, CReal peRight) {
    Real *pLeftReal = static_cast<Real *>(pLeft);
    Real *pLeftErrorReal = static_cast<Real *>(peLeft);
    Real *pRightReal = static_cast<Real *>(pRight);
    Real *pRightErrorReal = static_cast<Real *>(peRight);
    return new Real(aebounddp_div(*pLeftReal, *pLeftErrorReal, *pRightReal, *pRightErrorReal));
}

CReal real_create_error_sine(CReal p, CReal pe) {
    Real *pReal = static_cast<Real *>(p);
    Real *pErrorReal = static_cast<Real *>(pe);
    return new Real(aebounddp_sin(*pReal, *pErrorReal));
}

CReal real_create_error_cosine(CReal p, CReal pe) {
    Real *pReal = static_cast<Real *>(p);
    Real *pErrorReal = static_cast<Real *>(pe);
    return new Real(aebounddp_cos(*pReal, *pErrorReal));
}

CReal real_create_error_arctangent(CReal p, CReal pe) {
    Real *pReal = static_cast<Real *>(p);
    Real *pErrorReal = static_cast<Real *>(pe);
    return new Real(aebounddp_atn(*pReal, *pErrorReal));
}

CReal real_create_error_arctangent_tight(CReal p, CReal pe) {
    Real *pReal = static_cast<Real *>(p);
    Real *pErrorReal = static_cast<Real *>(pe);
    return new Real(aebounddp_atn_t(*pReal, *pErrorReal));
}

CReal real_create_error_eexponent(CReal p, CReal pe) {
    Real *pReal = static_cast<Real *>(p);
    Real *pErrorReal = static_cast<Real *>(pe);
    return new Real(aebounddp_exp(*pReal, *pErrorReal));
}

CReal real_create_error_elogarithm(CReal p, CReal pe) {
    Real *pReal = static_cast<Real *>(p);
    Real *pErrorReal = static_cast<Real *>(pe);
    return new Real(aebounddp_ln(*pReal, *pErrorReal));
}

CReal real_create_error_floor(CReal p, CReal pe){
    Real *pReal = static_cast<Real *>(p);
    Real *pErrorReal = static_cast<Real *>(pe);
    return new Real(aebounddp_flr(*pReal, *pErrorReal));
}

CReal real_create_error_floor_tight(CReal p, CReal pe){
    Real *pReal = static_cast<Real *>(p);
    Real *pErrorReal = static_cast<Real *>(pe);
    return new Real(aebounddp_flr_t(*pReal, *pErrorReal));
}

CReal real_create_error_power_of_two_multiplication(CInt n, CReal pe) {
    Real *pErrorReal = static_cast<Real *>(pe);
    return new Real(aebounddp_mul_p2(n, *pErrorReal));
}

CReal real_create_single_error_negation(CReal p, CReal pe) {
    Real *pReal = static_cast<Real *>(p);
    Real *pErrorReal = static_cast<Real *>(pe);
    return new Real(aebounddp_neg(*pReal, *pErrorReal));
}

CReal real_create_single_error_addition(CReal pLeft, CReal peLeft, CReal pRight, CReal peRight) {
    Real *pLeftReal = static_cast<Real *>(pLeft);
    Real *pLeftErrorReal = static_cast<Real *>(peLeft);
    Real *pRightReal = static_cast<Real *>(pRight);
    Real *pRightErrorReal = static_cast<Real *>(peRight);
    return new Real(aebounddp_add(*pLeftReal, *pLeftErrorReal, *pRightReal, *pRightErrorReal));
}

CReal real_create_single_error_subtraction(CReal pLeft, CReal peLeft, CReal pRight, CReal peRight) {
    Real *pLeftReal = static_cast<Real *>(pLeft);
    Real *pLeftErrorReal = static_cast<Real *>(peLeft);
    Real *pRightReal = static_cast<Real *>(pRight);
    Real *pRightErrorReal = static_cast<Real *>(peRight);
    return new Real(aebounddp_sub(*pLeftReal, *pLeftErrorReal, *pRightReal, *pRightErrorReal));
}

CReal real_create_single_error_multiplication(CReal pLeft, CReal peLeft, CReal pRight, CReal peRight) {
    Real *pLeftReal = static_cast<Real *>(pLeft);
    Real *pLeftErrorReal = static_cast<Real *>(peLeft);
    Real *pRightReal = static_cast<Real *>(pRight);
    Real *pRightErrorReal = static_cast<Real *>(peRight);
    return new Real(aebounddp_mul(*pLeftReal, *pLeftErrorReal, *pRightReal, *pRightErrorReal));
}

CReal real_create_single_error_division(CReal pLeft, CReal peLeft, CReal pRight, CReal peRight) {
    Real *pLeftReal = static_cast<Real *>(pLeft);
    Real *pLeftErrorReal = static_cast<Real *>(peLeft);
    Real *pRightReal = static_cast<Real *>(pRight);
    Real *pRightErrorReal = static_cast<Real *>(peRight);
    return new Real(aebounddp_div(*pLeftReal, *pLeftErrorReal, *pRightReal, *pRightErrorReal));
}

CReal real_create_single_error_sine(CReal p, CReal pe) {
    Real *pReal = static_cast<Real *>(p);
    Real *pErrorReal = static_cast<Real *>(pe);
    return new Real(aebounddp_sin(*pReal, *pErrorReal));
}

CReal real_create_single_error_cosine(CReal p, CReal pe) {
    Real *pReal = static_cast<Real *>(p);
    Real *pErrorReal = static_cast<Real *>(pe);
    return new Real(aebounddp_cos(*pReal, *pErrorReal));
}

CReal real_create_single_error_arctangent(CReal p, CReal pe) {
    Real *pReal = static_cast<Real *>(p);
    Real *pErrorReal = static_cast<Real *>(pe);
    return new Real(aebounddp_atn(*pReal, *pErrorReal));
}

CReal real_create_single_error_arctangent_tight(CReal p, CReal pe) {
    Real *pReal = static_cast<Real *>(p);
    Real *pErrorReal = static_cast<Real *>(pe);
    return new Real(aebounddp_atn_t(*pReal, *pErrorReal));
}

CReal real_create_single_error_eexponent(CReal p, CReal pe) {
    Real *pReal = static_cast<Real *>(p);
    Real *pErrorReal = static_cast<Real *>(pe);
    return new Real(aebounddp_exp(*pReal, *pErrorReal));
}

CReal real_create_single_error_elogarithm(CReal p, CReal pe) {
    Real *pReal = static_cast<Real *>(p);
    Real *pErrorReal = static_cast<Real *>(pe);
    return new Real(aebounddp_ln(*pReal, *pErrorReal));
}

CReal real_create_single_error_floor(CReal p, CReal pe){
    Real *pReal = static_cast<Real *>(p);
    Real *pErrorReal = static_cast<Real *>(pe);
    return new Real(aebounddp_flr(*pReal, *pErrorReal));
}

CReal real_create_single_error_floor_tight(CReal p, CReal pe){
    Real *pReal = static_cast<Real *>(p);
    Real *pErrorReal = static_cast<Real *>(pe);
    return new Real(aebounddp_flr_t(*pReal, *pErrorReal));
}

CReal real_create_single_error_power_of_two_multiplication(CInt n, CReal pe) {
    Real *pErrorReal = static_cast<Real *>(pe);
    return new Real(aebounddp_mul_p2(n, *pErrorReal));
}

//////////////////////////////////////////////////////////////////////
//
// MinMaxSystem
//
//////////////////////////////////////////////////////////////////////

CMinMaxSystem minmax_system_create(CString name) {
    std::string castedName = static_cast<const char*>(name);
    return new MinMaxSystem(castedName);
}

void minmax_system_print(CMinMaxSystem p) {
    static_cast<MinMaxSystem *>(p)->print(std::cout);
}

void minmax_system_register_variable(CMinMaxSystem p, CString name, CInterval plb, CInterval pub) {
    std::string stringName = static_cast<const char*>(name);
    Interval *lowerBound = static_cast<Interval *>(plb);
    Interval *upperBound = static_cast<Interval *>(pub);
    static_cast<MinMaxSystem *>(p)->var(stringName, *lowerBound, *upperBound);
}

void minmax_system_set_maxdepth(CMinMaxSystem p, CUInt depth) {
    static_cast<MinMaxSystem *>(p)->set_maxdepth(depth);
}

void minmax_system_set_precision(CMinMaxSystem p, CInt precision) {
    static_cast<MinMaxSystem *>(p)->set_precision(precision);
}

void minmax_system_maximize(CMinMaxSystem pSys, CReal pExp) {
    Real *pRealExpression = static_cast<Real *>(pExp);
    static_cast<MinMaxSystem *>(pSys)->max(*pRealExpression);
}

double minmax_system_maximum_lower_bound(CMinMaxSystem p) {
    return static_cast<MinMaxSystem *>(p)->answer().lb_of_max();
}

double minmax_system_maximum_upper_bound(CMinMaxSystem p) {
    return static_cast<MinMaxSystem *>(p)->answer().ub_of_max();
}
