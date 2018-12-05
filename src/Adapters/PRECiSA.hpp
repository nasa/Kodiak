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

#ifndef KODIAK_PRECISA_HPP
#define KODIAK_PRECISA_HPP

#include "Real.hpp"

namespace kodiak {
    namespace Adapters {
        namespace PRECiSA {

            Real ulp_dp(Real &);

            Real ulp_dp(Real &&);

            Real abs(Real &);

            Real aebounddp_add(Real, Real, Real, Real);

            Real aebounddp_sub(Real, Real, Real, Real);

            Real aebounddp_mul(Real, Real, Real, Real);

            Real aebounddp_div(Real, Real, Real, Real);

            Real aebounddp_flr(Real, Real);

            Real aebounddp_sqt(Real, Real);

            Real aebounddp_neg(Real, Real);

            Real aebounddp_abs(Real, Real);

            Real aebounddp_ln(Real, Real);

            Real aebounddp_exp(Real, Real);

            Real aebounddp_sin(Real, Real);

            Real aebounddp_cos(Real, Real);

            Real aebounddp_atn(Real, Real);

            Real aebounddp_flr_t(Real, Real);

            Real aebounddp_atn_t(Real, Real);

            Real aebounddp_mul_p2(int, Real);

            Real aeboundsp_add(Real, Real, Real, Real);

            Real aeboundsp_sub(Real, Real, Real, Real);

            Real aeboundsp_mul(Real, Real, Real, Real);

            Real aeboundsp_div(Real, Real, Real, Real);

            Real aeboundsp_flr(Real, Real);

            Real aeboundsp_sqt(Real, Real);

            Real aeboundsp_neg(Real, Real);

            Real aeboundsp_abs(Real, Real);

            Real aeboundsp_ln(Real, Real);

            Real aeboundsp_exp(Real, Real);

            Real aeboundsp_sin(Real, Real);

            Real aeboundsp_cos(Real, Real);

            Real aeboundsp_atn(Real, Real);

            Real aeboundsp_flr_t(Real, Real);

            Real aeboundsp_atn_t(Real, Real);

            Real aeboundsp_mul_p2(int, Real);
        }
    }
}

#endif //KODIAK_PRECISA_HPP
