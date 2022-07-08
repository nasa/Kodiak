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

/* 
 * The following examples are found in the Wikipedia page on Bifurcation analysis 
 */
void wiki(const bool test = false, const nat debug = 0, const bool bp = false) {
    if (!test) return;

    Real x = var("x");
    Real y = var("y");
    Real alpha = var("alpha");
    Real beta = var("beta");
    Real mu = var("mu");

    Bifurcation wiki(bp ? "wiki_bp" : "wiki_ia");
    wiki.setDefaultEnclosureMethodTrueForBernsteinAndFalseForIntervalArithmetic(bp);
    wiki.set_debug(debug);
    wiki.var("x", -2, 2);
    wiki.var("y", -3, 3);
    wiki.param("alpha", -1, 1);
    wiki.dfeq(alpha - (x ^ 2));
    wiki.dfeq(-y);
    wiki.set_resolutions(0); // Any resolution for the variables
    wiki.bifurcation();
    wiki.print();
    wiki.gnuplot_3D("x", "y", "alpha");
    cout << endl;

    Bifurcation hopf1(bp ? "Hopf1_bp" : "Hopf1_ia");
    hopf1.setDefaultEnclosureMethodTrueForBernsteinAndFalseForIntervalArithmetic(bp);
    hopf1.set_debug(debug);
    hopf1.var("x", 0, 3);
    hopf1.var("y", 0, 3);
    hopf1.param("alpha", dec(1, -1));
    hopf1.param("beta", 0, 3);
    hopf1.dfeq(-x + alpha * y + (x ^ 2) * y);
    hopf1.dfeq(beta - alpha * y - (x ^ 2) * y);
    hopf1.set_resolutions(0); // Any resolution for the variables
    hopf1.set_precision(-3); // Precision 1E-3
    hopf1.bifurcation();
    hopf1.print();
    hopf1.gnuplot_2D("x", "beta", true);
    hopf1.gnuplot_2D("y", "beta", true);
    hopf1.gnuplot_3D("x", "y", "beta", true);
    cout << endl;

    Bifurcation hopf2(bp ? "Hopf2_bp" : "Hopf2_ia");
    hopf2.setDefaultEnclosureMethodTrueForBernsteinAndFalseForIntervalArithmetic(bp);
    hopf2.set_debug(debug);
    hopf2.var("x", 0, 1);
    hopf2.var("y", 0, 1);
    hopf2.param("mu", -1, 1);
    hopf2.dfeq(mu * (1 - (y ^ 2)) * x - y);
    hopf2.dfeq(x);
    hopf2.set_resolutions(0.01);
    hopf2.bifurcation();
    hopf2.print();
    hopf2.gnuplot_3D("x", "y", "mu");
    cout << endl;
}

/*
 * Cusp' system 
 */
void cusp(const bool test = false, const nat debug = 0, const bool bp = false) {
    if (!test) return;
    Real y = var("y");
    Real b1 = var("b1");
    Real b2 = var("b2");
    Real ydot = b1 + b2 * y - (y ^ 3);

    Bifurcation cusp("cusp");
    cusp.setDefaultEnclosureMethodTrueForBernsteinAndFalseForIntervalArithmetic(bp);
    cusp.set_debug(debug);

    cusp.var("y", -3, 3);
    cusp.param("b1", -3, 3);
    cusp.param("b2", -3, 3);
    cusp.dfeq(ydot);
    cusp.set_resolutions(0.001); // Resolution 0.1% of original box
    cusp.bifurcation();
    cusp.print();
    cusp.gnuplot_3D("b1", "b2", "y");
    cout << endl;
}

/* The following example is taken from 
   [EW00] M'Hammed El Kahoui and Andreas Weber, Deciding Hopf Bifurcations by Quantifier Elimination 
   in a Software-component Architecture, J. Symbolic Computations (2000),
   30, 161-179. */

void CH(const bool test = false, const nat debug = 0, const bool bp = false) {
    if (!test) return;

    // Chow and Hale (1996)
    Real x = var("x");
    Real y = var("y");
    Real a = var("a");

    Bifurcation CH(bp ? "CH_bp" : "CH_ia");
    CH.setDefaultEnclosureMethodTrueForBernsteinAndFalseForIntervalArithmetic(bp);
    CH.set_debug(debug);
    CH.var("x", 0, 1);
    CH.var("y", 0, 1);
    CH.param("a", -1, 1);
    CH.dfeq(x * (1 + rat(1, 4) * (a ^ 2) - rat(1, 4) * ((x - 1 - a) ^ 2) - y));
    CH.dfeq(y * (x - 1));
    CH.set_resolutions(0.001);
    CH.set_precision(-3);
    CH.bifurcation();
    CH.print();
    CH.gnuplot_3D("x", "y", "a");
    cout << endl;
}

/* 
 * Lorenz system 
 */
void lorenz(const bool test = false, const nat debug = 0, const bool bp = false) {
    if (!test) return;
    cout << "** bifurcation (Kodiak): This example takes several seconds..." << endl;
    Real x = var("x");
    Real y = var("y");
    Real z = var("z");
    Real sigma = var("sigma");
    Real rho = var("rho");
    Real beta = var("beta");

    Real xdot = sigma * (y - x);
    Real ydot = x * (rho - z) - y;
    Real zdot = x * y - beta * z;

    Bifurcation lorenz(bp ? "Lorenz_bp" : "Lorenz_ia");
    lorenz.setDefaultEnclosureMethodTrueForBernsteinAndFalseForIntervalArithmetic(bp);
    lorenz.set_debug(debug);

    lorenz.var("x", 0, 8);
    lorenz.var("y", 0, 8);
    lorenz.var("z", 0, 29);

    lorenz.param("sigma", 3);
    lorenz.param("rho", 1, 30);
    lorenz.param("beta", 1, 2);
    lorenz.dfeq(xdot);
    lorenz.dfeq(ydot);
    lorenz.dfeq(zdot);
    lorenz.set_resolutions(0.001); // Resolution 0.1% of original box
    lorenz.set_resolution("beta", 0.01); // Resolution 1% of original box
    lorenz.bifurcation();
    lorenz.print();
    lorenz.gnuplot_2D("beta", "rho", true);
    cout << endl;
}

int main(int, char *[]) {

    Kodiak::init();
    Kodiak::set_safe_input(false);

    try {
        wiki(true);
        cusp(true);
        CH(true);
        lorenz(true);
    } catch (Growl const & growl) {
        cout << growl.what() << endl;
    }
}
