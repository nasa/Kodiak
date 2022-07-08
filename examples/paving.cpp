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
    Real y = var("y");
    Real z = var("z");

    Paver p0;
    p0.setDefaultEnclosureMethodTrueForBernsteinAndFalseForIntervalArithmetic(bp);
    p0.set_debug(debug);
    p0.var("x", 0, 10);
    p0.eq0((x ^ 2) - 5 * x + 6);
    p0.set_resolutions(0); // Any resolution for the variables
    p0.pave();
    p0.print(); // Solutions are x=2 or x=3
    p0.gnuplot("x", "x");
    cout << endl;

    Paver p1;
    p1.setDefaultEnclosureMethodTrueForBernsteinAndFalseForIntervalArithmetic(bp);
    p1.set_debug(debug);
    p1.var("x", -3, 3);
    p1.var("y", -3, 3);
    p1.var("z", -3, 3);
    p1.eq0(3 * x + 2 * y - z - 1);
    p1.eq0(2 * x - 2 * y + 4 * z + 2);
    p1.eq0(-x + rat(1, 2) * y - z);
    p1.set_resolutions(0); // Any resolution for the variables
    p1.set_precision(-2); // Precision 1E-2
    p1.pave(); // Find zeros
    p1.print(); // Solution x=1, y=-2, and z= -2
    p1.gnuplot("x", "y", "z");
    cout << endl;

    /*
       The following examples are taken from
       "The Interval Library filib++ 2.0 - Design, Features and Sample Programs",
       M. Lerch, G. Tischler, J. Wolff von Gudenberg, W. Hofschuster, and W. Kramer,
       Preprint 2001/4, Universität Wuppertal, 2001.
    */

    Paver p2;
    p2.setDefaultEnclosureMethodTrueForBernsteinAndFalseForIntervalArithmetic(bp);
    p2.set_debug(debug);
    p2.var("x", approx(-1.4), approx(0.9));
    p2.eq0(Sin(1 / (x - 1)) * Ln(rat(3, 2) - Abs(x)));
    p2.set_resolutions(0); // Any resolution for the variables
    p2.set_precision(-2); // Set tolerance to 1E-2
    p2.pave(); // Find zeros
    p2.print(); // Five solutions
    p2.gnuplot("x", "x");
    cout << endl;

    Paver p3;
    p3.setDefaultEnclosureMethodTrueForBernsteinAndFalseForIntervalArithmetic(bp);
    p3.set_debug(debug);
    p3.var("x", 4, 10);
    p3.eq0(1 - Sqrt(x - 4));
    p3.set_resolutions(0); // Any resolution for the variables
    p3.set_precision(-2); // Set tolerance to 1E-2
    p3.pave(); // Find zeros
    p3.print(); // Solution is 5
    p3.gnuplot("x", "x");
    cout << endl;

    Paver p4;
    p4.setDefaultEnclosureMethodTrueForBernsteinAndFalseForIntervalArithmetic(bp);
    p4.set_debug(debug);
    p4.var("x", 0, 10);
    p4.eq0(((x - 1) ^ 3) * ((x - 2) ^ 2) * (x - 3));
    p4.set_resolutions(0); // Any resolution for the variables
    p4.set_precision(-3); // Set tolerance to 1E-3
    p4.pave(); // Find zeros
    p4.print(); // Solutions x=1, x=2, or x=3
    p4.gnuplot("x", "x");
    cout << endl;

    Paver p5;
    p5.setDefaultEnclosureMethodTrueForBernsteinAndFalseForIntervalArithmetic(bp);
    p5.set_debug(debug);
    p5.var("x", -5, 5);
    p5.var("y", -5, 5);
    p5.eq0(3 * (x ^ 2) + 2 * (y ^ 2) - 35);
    p5.eq0(4 * (x ^ 2) - 3 * (y ^ 2) - 24);
    p5.set_resolutions(0); // Any resolution for the variables
    p5.set_precision(-3); // Set tolerance to 1E-3
    p5.pave(); // Find zeros
    p5.print(); // Solutions: (± 3, ± 2)
    p5.gnuplot("x", "y");
    cout << endl;

    Paver wheel(bp ? "wheel_bp" : "wheel_ia");
    wheel.setDefaultEnclosureMethodTrueForBernsteinAndFalseForIntervalArithmetic(bp);
    wheel.set_debug(debug);
    wheel.var("x", -5, 5);
    wheel.var("y", -5, 5);
    wheel.le0((x ^ 2) + (y ^ 2) - 16);
    wheel.ge0((x ^ 2) + (y ^ 2) - 4);
    wheel.ge0(x * y);
    wheel.set_resolutions(0.005); // Resolution 0.5% of original box
    wheel.set_precision(-2); // Set tolerance to 1E-3
    wheel.full(); // Find full paving
    wheel.print();
    wheel.style.set_filled(false);
    wheel.gnuplot("x", "y");
    cout << endl;
}

void luis(const bool test = false, const nat debug = 0, const bool bp = false) {
    if (!test) return;
    Real x1 = var("x1");
    Real x2 = var("x2");

    Paver p(bp ? "safe_bp" : "safe_ia");
    p.setDefaultEnclosureMethodTrueForBernsteinAndFalseForIntervalArithmetic(bp);
    p.set_debug(debug);
    p.var("x1", -2, 2);
    p.var("x2", -2, 2);
    p.le0((x1 ^ 2) * (x2 ^ 4) + (x2 ^ 2) * (x1 ^ 4) - 3 * (x1 ^ 2) * (x2 ^ 2) - x1 * x2 + (x1 ^ 6) / 200 +
          (x2 ^ 6) / 200 - dec(7, -2));
    p.le0(-(rat(1, 2) * (x1 ^ 2) * (x2 ^ 4) + (x2 ^ 2) * (x1 ^ 4) - 3 * (x1 ^ 2) * (x2 ^ 2) - (x1 ^ 5) * (x2 ^ 3) / 10 +
            dec(9, -1)));
    p.set_resolutions(0.001); // Resolution 0.1% of original box
    p.set_precision(-3); // Precision 1E-3
    p.full();
    p.print();
    p.style.set_filled(false);
    p.gnuplot("x1", "x2");
}

int main(int, char *[]) {

    Kodiak::init();
    Kodiak::set_safe_input(false);

    try {
        test(true);
        luis(true);
    } catch (Growl const & growl) {
        cout << growl.what() << endl;
    }
}

