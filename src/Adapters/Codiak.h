// Notices:
//
// Copyright 2017 United States Government as represented by the Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
//
// Disclaimers:
//
// No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED,
// IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, ANY WARRANTY THAT THE SUBJECT
// SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED, WILL CONFORM TO THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN ENDORSEMENT BY GOVERNMENT AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS, RESULTING DESIGNS,
// HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS RESULTING FROM USE OF THE SUBJECT SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND LIABILITIES REGARDING THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS."
//
// Waiver and Indemnity:  RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT,
// ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES, EXPENSES OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING
// FROM, RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.  RECIPIENT'S SOLE REMEDY FOR ANY SUCH
// MATTER SHALL BE THE IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#ifndef KODIAK_CODIAK_H
#define KODIAK_CODIAK_H

typedef void* CInterval;
typedef void* CReal;
typedef void* CBool;
typedef void* CRealVector;
typedef const void* CString;
typedef unsigned int CUInt;
typedef int CInt;
typedef void* CMinMaxSystem;
typedef void* CPaver;


//////////////////////////////////////////////////////////////////////
//
// Real Expressions
//
//////////////////////////////////////////////////////////////////////

EXTERNC CInterval interval_create(double lb, double ub);
EXTERNC CInterval interval_create_from_rational(CInt numerator, CUInt denominator);
EXTERNC void interval_print(CInterval);
EXTERNC int real_equal_to(CReal, CReal);
EXTERNC CReal real_create_value(CInterval p);
EXTERNC CReal real_create_variable(CUInt index, CString name);
EXTERNC CReal real_create_local_variable(CString name);
EXTERNC CReal real_create_letin(CString, CReal, CReal);
EXTERNC void real_print(CReal p);
EXTERNC CRealVector real_vector_create();
EXTERNC void real_vector_add(CRealVector, CReal);
EXTERNC void real_vector_print(CRealVector);
EXTERNC CString real_to_string(CReal p);
EXTERNC CReal real_create_minimum(CRealVector);
EXTERNC CReal real_create_maximum(CRealVector);
EXTERNC CReal real_create_absolute_value(CReal);
EXTERNC CReal real_create_sine(CReal);
EXTERNC CReal real_create_arcsine(CReal);
EXTERNC CReal real_create_cosine(CReal);
EXTERNC CReal real_create_arccosine(CReal);
EXTERNC CReal real_create_tangent(CReal);
EXTERNC CReal real_create_arctangent(CReal);
EXTERNC CReal real_create_sqrt(CReal);
EXTERNC CReal real_create_eexponent(CReal);
EXTERNC CReal real_create_elogarithm(CReal);
EXTERNC CReal real_create_floor(CReal);
EXTERNC CReal real_create_double_ulp(CReal);
EXTERNC CReal real_create_single_ulp(CReal);
EXTERNC CReal real_create_negation(CReal);
EXTERNC CReal real_create_addition(CReal, CReal);
EXTERNC CReal real_create_subtraction(CReal, CReal);
EXTERNC CReal real_create_multiplication(CReal, CReal);
EXTERNC CReal real_create_division(CReal, CReal);


//////////////////////////////////////////////////////////////////////
//
// Boolean Expressions
//
//////////////////////////////////////////////////////////////////////

EXTERNC int bool_equal_to(CBool, CBool);
EXTERNC CBool bool_create_true();
EXTERNC CBool bool_create_false();
EXTERNC CBool bool_create_possibly();
EXTERNC CBool bool_create_within_eps();
EXTERNC void  bool_print(CBool);
EXTERNC CBool bool_create_not(CBool);
EXTERNC CBool bool_create_and(CBool, CBool);
EXTERNC CBool bool_create_or(CBool, CBool);
EXTERNC CBool bool_create_implies(CBool, CBool);
EXTERNC CBool bool_create_equal_to(CReal, CReal);
EXTERNC CBool bool_create_less_than(CReal, CReal);
EXTERNC CBool bool_create_less_than_or_equal_to(CReal, CReal);
EXTERNC CBool bool_create_greater_than(CReal, CReal);
EXTERNC CBool bool_create_greater_than_or_equal_to(CReal, CReal);


//////////////////////////////////////////////////////////////////////
//
// Real Error Expressions
//
//////////////////////////////////////////////////////////////////////

EXTERNC CReal real_create_error_negation(CReal, CReal);
EXTERNC CReal real_create_error_addition(CReal, CReal, CReal, CReal);
EXTERNC CReal real_create_error_subtraction(CReal, CReal, CReal, CReal);
EXTERNC CReal real_create_error_multiplication(CReal, CReal, CReal, CReal);
EXTERNC CReal real_create_error_division(CReal, CReal, CReal, CReal);
EXTERNC CReal real_create_error_sine(CReal, CReal);
EXTERNC CReal real_create_error_arcsine(CReal, CReal);
EXTERNC CReal real_create_error_cosine(CReal, CReal);
EXTERNC CReal real_create_error_arccosine(CReal, CReal);
EXTERNC CReal real_create_error_tangent(CReal, CReal);
EXTERNC CReal real_create_error_arctangent(CReal, CReal);
EXTERNC CReal real_create_error_arctangent_tight(CReal, CReal);
EXTERNC CReal real_create_error_eexponent(CReal, CReal);
EXTERNC CReal real_create_error_elogarithm(CReal, CReal);
EXTERNC CReal real_create_error_floor(CReal, CReal);
EXTERNC CReal real_create_error_floor_tight(CReal, CReal);
EXTERNC CReal real_create_error_sqrt(CReal, CReal);
EXTERNC CReal real_create_error_power_of_two_multiplication(CInt, CReal);

EXTERNC CReal real_create_single_error_negation(CReal, CReal);
EXTERNC CReal real_create_single_error_addition(CReal, CReal, CReal, CReal);
EXTERNC CReal real_create_single_error_subtraction(CReal, CReal, CReal, CReal);
EXTERNC CReal real_create_single_error_multiplication(CReal, CReal, CReal, CReal);
EXTERNC CReal real_create_single_error_division(CReal, CReal, CReal, CReal);
EXTERNC CReal real_create_single_error_sine(CReal, CReal);
EXTERNC CReal real_create_single_error_cosine(CReal, CReal);
EXTERNC CReal real_create_single_error_arctangent(CReal, CReal);
EXTERNC CReal real_create_single_error_arctangent_tight(CReal, CReal);
EXTERNC CReal real_create_single_error_eexponent(CReal, CReal);
EXTERNC CReal real_create_single_error_elogarithm(CReal, CReal);
EXTERNC CReal real_create_single_error_floor(CReal, CReal);
EXTERNC CReal real_create_single_error_floor_tight(CReal, CReal);
EXTERNC CReal real_create_single_error_sqrt(CReal, CReal);
EXTERNC CReal real_create_single_error_power_of_two_multiplication(CInt, CReal);


//////////////////////////////////////////////////////////////////////
//
// MinMaxSystem
//
//////////////////////////////////////////////////////////////////////

EXTERNC CMinMaxSystem minmax_system_create(CString name);
EXTERNC void minmax_system_print(CMinMaxSystem p);
EXTERNC void minmax_system_register_variable(CMinMaxSystem p, CString name, CInterval lb, CInterval ub);
EXTERNC void minmax_system_set_maxdepth(CMinMaxSystem p, CUInt depth);
EXTERNC void minmax_system_set_precision(CMinMaxSystem p, CInt precision);
EXTERNC void minmax_system_minimize(CMinMaxSystem p, CReal pExpression);
EXTERNC void minmax_system_maximize(CMinMaxSystem p, CReal pExpression);
EXTERNC void minmax_system_minmax(CMinMaxSystem pSys, CReal pExp);
EXTERNC double minmax_system_minimum_lower_bound(CMinMaxSystem p);
EXTERNC double minmax_system_minimum_upper_bound(CMinMaxSystem p);
EXTERNC double minmax_system_maximum_lower_bound(CMinMaxSystem p);
EXTERNC double minmax_system_maximum_upper_bound(CMinMaxSystem p);


//////////////////////////////////////////////////////////////////////
//
// Paver
//
//////////////////////////////////////////////////////////////////////

EXTERNC CPaver paver_create(CString name);
EXTERNC void paver_print(CPaver p);
EXTERNC void paver_register_variable(CPaver p, CString name, CInterval lb, CInterval ub);
EXTERNC void paver_set_maxdepth(CPaver p, CUInt depth);
EXTERNC void paver_set_precision(CPaver p, CInt precision);
EXTERNC void paver_pave(CPaver p, CBool pExpression);
EXTERNC void paver_save_paving(CPaver p, CString filename);

#endif //KODIAK_CODIAK_H
