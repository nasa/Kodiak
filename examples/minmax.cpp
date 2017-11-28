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

#include <kodiak.hpp>

using namespace kodiak;
using std::cout;
using std::endl;

void test(const bool test = false, const nat debug = 0, const bool bp = false) {
    if (!test) return;

    Real x = var("x");
    Real f = x * (1 - x);

    MinMaxSystem s1;
    s1.set_debug(debug);
    s1.setDefaultEnclosureMethodTrueForBernsteinAndFalseForIntervalArithmetic(bp);
    s1.var("x", -1, 1);
    s1.gt0(x);
    s1.set_precision(-2);
    s1.minmax(f);
    s1.print();
    cout << endl;

    Real l = let("y", 1 - x, x * var("y"));
    MinMaxSystem s2;
    s2.set_debug(debug);
    s2.setDefaultEnclosureMethodTrueForBernsteinAndFalseForIntervalArithmetic(bp);
    s2.var("x", 0, 1);
    s2.set_precision(-2);
    s2.max(l);
    s2.print();
    cout << endl;

    Real v = -Sqrt(x * Sin(x));
    MinMaxSystem s3;
    s3.set_debug(debug);
    s3.setDefaultEnclosureMethodTrueForBernsteinAndFalseForIntervalArithmetic(bp);
    s3.var("x", 1, 3);
    s3.set_precision(-5);
    s3.max(v);
    s3.print();
    cout << endl;

}

//******* Heart Dipole Electrical Model
void hdp(const bool test = false, const nat debug = 0, const bool bp = false) {

    if (!test) return;

    Real x0 = var("x0");
    Real x1 = var("x1");
    Real x2 = var("x2");
    Real x3 = var("x3");
    Real x4 = var("x4");
    Real x5 = var("x5");
    Real x6 = var("x6");
    Real x7 = var("x7");

    Real f = -x0 * (x5 ^ 3) + 3 * x0 * x5 * (x6 ^ 2) - x2 * (x6 ^ 3) + 3 * x2 * x6 * (x5 ^ 2)
             - x1 * (x4 ^ 3) + 3 * x1 * x4 * (x7 ^ 2) - x3 * (x7 ^ 3) + 3 * x3 * x7 * (x4 ^ 2)
             - approx(0.9563453);

    MinMaxSystem hdp;
    hdp.set_debug(debug);
    hdp.setDefaultEnclosureMethodTrueForBernsteinAndFalseForIntervalArithmetic(bp);
    hdp.var("x0", approx(-0.1), approx(0.4));
    hdp.var("x1", approx(0.4), 1);
    hdp.var("x2", approx(-0.7), approx(-0.4));
    hdp.var("x3", approx(-0.7), approx(0.4));
    hdp.var("x4", approx(0.1), approx(0.2));
    hdp.var("x5", approx(-0.1), approx(0.2));
    hdp.var("x6", approx(-0.3), approx(1.1));
    hdp.var("x7", approx(-1.1), approx(-0.3));
    hdp.set_precision(-5);
    hdp.minmax(f);
    hdp.print();
    cout << endl;
}

void luis(const bool test = false, const nat debug = 0, const bool bp = false) {
    if (!test) return;

    Real eta = val(approx(1.5));
    Real x = var("x");
    Real delta1 = cnst("delta1");
    Real delta2 = cnst("delta2");
    Real delta3 = cnst("delta3");
    Real delta4 = cnst("delta4");
    Real delta5 = cnst("delta5");
    Real delta6 = cnst("delta6");
    Real delta7 = cnst("delta7");
    Real deltap1 = cnst("deltap1");
    Real deltap2 = cnst("deltap2");
    Real deltap3 = cnst("deltap3");
    Real deltap4 = cnst("deltap4");
    Real deltap5 = cnst("deltap5");
    Real deltap6 = cnst("deltap6");
    Real deltap7 = cnst("deltap7");

    Real f = (1 / (2 * eta)) * (deltap1 + x * deltap2 + (x ^ 2) * deltap3 +
                                (x ^ 3) * deltap4 + (x ^ 4) * deltap5 +
                                (x ^ 5) * deltap6 + (x ^ 6) * deltap7);
    Real l = (delta1 ^ 2) + (delta2 ^ 2) * (x ^ 2) + (delta3 ^ 2) * (x ^ 4) +
             (delta4 ^ 2) * (x ^ 6) + (delta5 ^ 2) * (x ^ 8) +
             (delta6 ^ 2) * (x ^ 10) + (delta7 ^ 2) * (x ^ 12) - Sq(f);

    MinMaxSystem luis1("l1");
    luis1.set_debug(debug);
    luis1.setDefaultEnclosureMethodTrueForBernsteinAndFalseForIntervalArithmetic(bp);
    luis1.var("x", 0, approx(5.5));
    luis1.cnst("delta1", approx(9.1717));
    luis1.cnst("delta2", 0);
    luis1.cnst("delta3", 0);
    luis1.cnst("delta4", 0);
    luis1.cnst("delta5", 0);
    luis1.cnst("delta6", 0);
    luis1.cnst("delta7", 0);
    luis1.cnst("deltap1", approx(5.1293));
    luis1.cnst("deltap2", 0);
    luis1.cnst("deltap3", 0);
    luis1.cnst("deltap4", 0);
    luis1.cnst("deltap5", 0);
    luis1.cnst("deltap6", approx(0.0001));
    luis1.cnst("deltap7", approx(0.0003));
    luis1.max(l);
    luis1.print();
    cout << endl;

    MinMaxSystem luis2("l2");
    luis2.set_debug(debug);
    luis2.setDefaultEnclosureMethodTrueForBernsteinAndFalseForIntervalArithmetic(bp);
    luis2.var("x", 0, approx(5.5));
    luis2.cnst("delta1", approx(4.7572));
    luis2.cnst("delta2", 0);
    luis2.cnst("delta3", 0);
    luis2.cnst("delta4", 0);
    luis2.cnst("delta5", 0);
    luis2.cnst("delta6", 0);
    luis2.cnst("delta7", 0);
    luis2.cnst("deltap1", approx(5.1293));
    luis2.cnst("deltap2", 0);
    luis2.cnst("deltap3", 0);
    luis2.cnst("deltap4", 0);
    luis2.cnst("deltap5", 0);
    luis2.cnst("deltap6", approx(0.0001));
    luis2.cnst("deltap7", approx(0.0003));
    luis2.max(l);
    luis2.print();
    cout << endl;

    MinMaxSystem luis3("l3");
    luis3.set_debug(debug);
    luis3.setDefaultEnclosureMethodTrueForBernsteinAndFalseForIntervalArithmetic(bp);
    luis3.var("x", 0, approx(5.5));
    luis3.cnst("delta1", approx(13.7572));
    luis3.cnst("delta2", 0);
    luis3.cnst("delta3", approx(0.0009));
    luis3.cnst("delta4", approx(0.0008));
    luis3.cnst("delta5", 0);
    luis3.cnst("delta6", 0);
    luis3.cnst("delta7", 0);
    luis3.cnst("deltap1", approx(14.6297) - approx(12.8847));
    luis3.cnst("deltap2", 0);
    luis3.cnst("deltap3", 0);
    luis3.cnst("deltap4", 0);
    luis3.cnst("deltap5", 0);
    luis3.cnst("deltap6", 0);
    luis3.cnst("deltap7", 0);
    luis3.max(l);
    luis3.print();
    cout << endl;

    MinMaxSystem luis4("l4");
    luis4.set_debug(debug);
    luis4.setDefaultEnclosureMethodTrueForBernsteinAndFalseForIntervalArithmetic(bp);
    luis4.var("x", 0, approx(5.5));
    luis4.cnst("delta1", approx(6.8234));
    luis4.cnst("delta2", 0);
    luis4.cnst("delta3", 0);
    luis4.cnst("delta4", 0);
    luis4.cnst("delta5", 0);
    luis4.cnst("delta6", 0);
    luis4.cnst("delta7", 0);
    luis4.cnst("deltap1", approx(5.1293));
    luis4.cnst("deltap2", 0);
    luis4.cnst("deltap3", 0);
    luis4.cnst("deltap4", 0);
    luis4.cnst("deltap5", 0);
    luis4.cnst("deltap6", approx(0.0001));
    luis4.cnst("deltap7", approx(0.0003));
    luis4.max(l);
    luis4.print();
    cout << endl;

}

void pete(const bool test = false, const nat debug = 0, const bool bp = false) {

    if (!test) return;

    //******* Pete's problem 1
    // (assert (>= (- (* a0min ( - 1 a1max)) (* a2max a2max)) 95000))
    // (assert (> a0min  0.0))
    // (assert (< a0min a0max))
    // (assert (< a0max 1000000.0))
    // (assert (> a1min  0.0))
    // (assert (< a1min a1max))
    // (assert (< a1max 1.0))
    // (assert (> a2min  1.0))
    // (assert (< a2min a2max))
    // (assert (< a2max 1000.0))
    // (assert (> a3min  1.0))
    // (assert (< a3min a3max))
    // (assert (< a3max 50.0))
    // What I want to do is to find the maximal value of:
    // (* (- a0max  a0min) (- a1max  a1min) (- a2max  a2min) (- a3max  a3min))

    Real a0min = var("a0min");
    Real a0max = var("a0max");
    Real a1min = var("a1min");
    Real a1max = var("a1max");
    Real a2min = var("a2min");
    Real a2max = var("a2max");
    Real a3min = var("a3min");
    Real a3max = var("a3max");

    MinMaxSystem pete("p");
    pete.set_debug(debug);
    pete.setDefaultEnclosureMethodTrueForBernsteinAndFalseForIntervalArithmetic(bp);
    pete.var("a0min", 0, 1000000);
    pete.var("a0max", 0, 1000000);
    pete.var("a1min", 0, 1);
    pete.var("a1max", 0, 1);
    pete.var("a2min", 1, 1000);
    pete.var("a2max", 1, 1000);
    pete.var("a3min", 1, 50);
    pete.var("a3max", 1, 50);
    pete.ge(a0min * (1 - a1max) - Sq(a2max), val(95000));
    pete.gt0(a0min);
    pete.lt(a0min, a0max);
    pete.lt(a0max, 1000000);
    pete.gt0(a1min);
    pete.lt(a1min, a1max);
    pete.lt(a1max, 1);
    pete.gt(a2min, 1);
    pete.lt(a2min, a2max);
    pete.lt(a2max, 1000);
    pete.gt(a3min, 1);
    pete.lt(a3min, a3max);
    pete.lt(a3max, 50);

    Real p = (a0max - a0min) * (a1max - a1min) * (a2max - a2min) * (a3max - a3min);

    pete.set_debug(debug);
    pete.set_varselect(0);
    pete.set_maxdepth(25);
    pete.set_precision(-2);
    pete.max(p);
    pete.print();
    cout << endl;
}

int main(int, char *[]) {

    Kodiak::init();
    Kodiak::set_safe_input(false);

    try {
        test(true);
        hdp(true);
        luis(true);
        pete(true);
    } catch (Growl growl) {
        cout << growl.what() << endl;
    }
}

