#include "Real.hpp"
#include "PRECiSA.hpp"

namespace kodiak {
    namespace Adapters {
        namespace PRECiSA {

            Real ulp_dp(Real &x) {
                return DUlp(x);

            }

            Real ulp_dp(Real &&x) {
                return DUlp(x);

            }

            Real abs(Real &x) {
                return Abs(x);
            }

            Real aebounddp_add(Real r1, Real e1, Real r2, Real e2) {
                return e1 + e2 + DUlp(Abs(r1 + r2) + e1 + e2) / kodiak::val(2);
            }


            Real aebounddp_sub(Real r1, Real e1, Real r2, Real e2) {
                return e1 + e2 + DUlp(Abs(r1 - r2) + e1 + e2) / kodiak::val(2);
            }


            Real aebounddp_mul(Real r1, Real e1, Real r2, Real e2) {
                return Abs(r1) * e2 + Abs(r2) * e1 + e1 * e2 +
                       DUlp((Abs(r1) + e1) * (Abs(r2) + e2)) / kodiak::val(2);
            }


            Real aebounddp_div(Real r1, Real e1, Real r2, Real e2) {
                return (Abs(r2) * e1 + Abs(r1) * e2) / (r2 * r2 - e2 * Abs(r2)) +
                       DUlp((Abs(r1) + e1) / (Abs(r2) - e2)) / kodiak::val(2);
            }


            Real aebounddp_flr(Real, Real e1) {
                return e1 + kodiak::val(1);
            }

            Real aebounddp_flr_t(Real, Real e1) {
                return e1;
            }

            Real aebounddp_sqt(Real r1, Real e1) {
                return Sqrt(e1) + DUlp(Sqrt(r1 + e1)) / kodiak::val(2);
            }

            Real aebounddp_neg(Real, Real e1) {
                return e1;
            }

            Real aebounddp_abs(Real, Real e1) {
                return e1;
            }

            Real aebounddp_ln(const Real r1, const Real e1) {
                return - Ln(1 - (e1 / r1)) + DUlp(Max({Abs(Ln(r1-e1)),Abs(Ln(r1+e1))})) / val(2);
            }

            Real aebounddp_exp(Real r1, Real e1) {
                return (Exp(e1) - 1) * Exp(r1) + DUlp(Exp(r1 + e1)) / val(2);
            }

            Real aebounddp_sin(Real r1, Real e1) {
                return kodiak::Min({kodiak::val(2), e1}) +
                       DUlp(Abs(Sin(r1)) + kodiak::Min({kodiak::val(2), e1})) / kodiak::val(2);
            }

            Real aebounddp_cos(Real r1, Real e1) {
                return kodiak::Min({kodiak::val(2), e1}) +
                       DUlp(Abs(Cos(r1)) + kodiak::Min({kodiak::val(2), e1})) / kodiak::val(2);
            }

            Real aebounddp_atn(Real r1, Real e1) {
                return DUlp(Atan(Abs(r1) + e1)) / kodiak::val(2) + e1;
            }

            Real aebounddp_atn_t(Real r1, Real e1) {
                return DUlp(Atan(Abs(r1) + e1)) / val(2) +
                       e1 / 1 + Min({(r1 - e1) ^ 2, (r1 + e1) ^ 2});
            }

            Real aebounddp_mul_p2(int n, Real e) {
                return (kodiak::val(2) ^ n) * e;
            }

            Real aeboundsp_add(Real r1, Real e1, Real r2, Real e2) {
                return e1 + e2 + SUlp(Abs(r1 + r2) + e1 + e2) / kodiak::val(2);
            }


            Real aeboundsp_sub(Real r1, Real e1, Real r2, Real e2) {
                return e1 + e2 + SUlp(Abs(r1 - r2) + e1 + e2) / kodiak::val(2);
            }


            Real aeboundsp_mul(Real r1, Real e1, Real r2, Real e2) {
                return Abs(r1) * e2 + Abs(r2) * e1 + e1 * e2 +
                       SUlp((Abs(r1) + e1) * (Abs(r2) + e2)) / kodiak::val(2);
            }


            Real aeboundsp_div(Real r1, Real e1, Real r2, Real e2) {
                return (Abs(r2) * e1 + Abs(r1) * e2) / (r2 * r2 - e2 * Abs(r2)) +
                       SUlp((Abs(r1) + e1) / (Abs(r2) - e2)) / kodiak::val(2);
            }


            Real aeboundsp_flr(Real, Real e1) {
                return e1 + kodiak::val(1);
            }

            Real aeboundsp_flr_t(Real, Real e1) {
                return e1;
            }

            Real aeboundsp_sqt(Real r1, Real e1) {
                return Sqrt(e1) + SUlp(Sqrt(r1 + e1)) / kodiak::val(2);
            }

            Real aeboundsp_neg(Real, Real e1) {
                return e1;
            }

            Real aeboundsp_abs(Real, Real e1) {
                return e1;
            }

            Real aeboundsp_ln(const Real r1, const Real e1) {
                return - Ln(1 - (e1 / r1)) + SUlp(Max({Abs(Ln(r1-e1)),Abs(Ln(r1+e1))})) / val(2);
            }

            Real aeboundsp_exp(Real r1, Real e1) {
                return (Exp(e1) - 1) * Exp(r1) + SUlp(Exp(r1 + e1)) / val(2);
            }

            Real aeboundsp_sin(Real r1, Real e1) {
                return kodiak::Min({kodiak::val(2), e1}) +
                       SUlp(Abs(Sin(r1)) + kodiak::Min({kodiak::val(2), e1})) / kodiak::val(2);
            }

            Real aeboundsp_cos(Real r1, Real e1) {
                return kodiak::Min({kodiak::val(2), e1}) +
                       SUlp(Abs(Cos(r1)) + kodiak::Min({kodiak::val(2), e1})) / kodiak::val(2);
            }

            Real aeboundsp_atn(Real r1, Real e1) {
                return SUlp(Atan(Abs(r1) + e1)) / kodiak::val(2) + e1;
            }

            Real aeboundsp_atn_t(Real r1, Real e1) {
                return SUlp(Atan(Abs(r1) + e1)) / val(2) +
                       e1 / 1 + Min({(r1 - e1) ^ 2, (r1 + e1) ^ 2});
            }

            Real aeboundsp_mul_p2(int n, Real e) {
                return (kodiak::val(2) ^ n) * e;
            }
        }
    }
}
