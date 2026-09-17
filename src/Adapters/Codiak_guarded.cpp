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
// HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS RESULTING FROM USE OF THE SUBJECT SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND LIABILITIES REGARDING THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS
// IS."
//
// Waiver and Indemnity:  RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT,
// ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES, EXPENSES OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING
// FROM, RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.  RECIPIENT'S SOLE REMEDY FOR ANY SUCH
// MATTER SHALL BE THE IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.

// Guarded API implementation (v2.1.0+)
//
// Exception-safe wrappers around Kodiak C API functions. These catch C++ exceptions
// (kodiak::Growl) and return status codes instead, making Kodiak safe to use from
// FFI contexts where C++ exception unwinding is not supported (Haskell, Python ctypes,
// Julia, etc.)
//
// Key design decisions:
//
// 1. CLASSIFICATION: Not all exceptions are equal. Division by an interval containing
//    zero is a legitimate mathematical outcome (unbounded result), while other exceptions
//    are real failures. The classification logic distinguishes between the two via exact
//    substring matching on the exception message.
//
// 2. CALLER CONTRACTS:
//    - On KODIAK_OK: output parameters are populated, system/paver is usable
//    - On KODIAK_DIV_BY_ZERO or KODIAK_ERROR: output parameters are zeroed/NULLed,
//      system/paver is DEAD and must not be reused
//
// 3. ERROR MESSAGES: Captured from std::exception::what() into caller-provided buffer.
//    Always NUL-terminated when buffer is non-NULL and non-zero-length.

#include <cstring>
#include <exception>

#include "Codiak.h"

// The only Growl condition that is a legitimate mathematical outcome rather
// than a failure. Thrown from Real.cpp (construction) and Node.cpp (evaluation);
// both messages end with this exact text.
//
// The match is deliberately this exact substring and nothing looser. Kodiak
// raises Growl for many other zero/domain conditions whose messages read
// similarly -- "ifnz cannot be evaluated when the first argument is an interval
// containing zero", "denumerator of a rational expression cannot be zero",
// "sqrt expects a nonnegative interval", "ln expects a positive interval",
// "acos expects an interval in [-1,1]" -- and every one of those is a failure.
static const char *KODIAK_DIV_BY_ZERO_MSG =
    "division by an interval that contains zero";

// Copies message into errbuf (always NUL-terminated when a buffer is given).
static void kodiak_copy_message(const char *message, char *errbuf, int errbuflen) {
    if (errbuf != nullptr && errbuflen > 0) {
        std::strncpy(errbuf, message, static_cast<size_t>(errbuflen - 1));
        errbuf[errbuflen - 1] = '\0';
    }
}

// Copies what() into errbuf and classifies it.
static int kodiak_classify(const char *what, char *errbuf, int errbuflen) {
    kodiak_copy_message(what, errbuf, errbuflen);
    return std::strstr(what, KODIAK_DIV_BY_ZERO_MSG) != nullptr
             ? KODIAK_DIV_BY_ZERO
             : KODIAK_ERROR;
}

static int kodiak_unknown(char *errbuf, int errbuflen) {
    kodiak_copy_message("unknown non-std::exception", errbuf, errbuflen);
    return KODIAK_ERROR;
}

// Helper macro for void-returning guarded wrappers
#define KODIAK_GUARDED_VOID(call) \
    try { \
        call; \
        return KODIAK_OK; \
    } catch (const std::exception &ex) { \
        return kodiak_classify(ex.what(), errbuf, errbuflen); \
    } catch (...) { \
        return kodiak_unknown(errbuf, errbuflen); \
    }

// Helper macro for value-returning guarded wrappers
#define KODIAK_GUARDED_VALUE(out_init, call, out_assign) \
    if (out != nullptr) *out = out_init; \
    try { \
        auto result = call; \
        if (out != nullptr) *out = out_assign; \
        return KODIAK_OK; \
    } catch (const std::exception &ex) { \
        return kodiak_classify(ex.what(), errbuf, errbuflen); \
    } catch (...) { \
        return kodiak_unknown(errbuf, errbuflen); \
    }


//////////////////////////////////////////////////////////////////////
//
// MinMaxSystem guarded operations
//
//////////////////////////////////////////////////////////////////////

int minmax_system_maximize_guarded(CMinMaxSystem p, CReal pExpression,
                                   char *errbuf, int errbuflen)
{
    KODIAK_GUARDED_VOID(minmax_system_maximize(p, pExpression))
}

int minmax_system_minmax_guarded(CMinMaxSystem pSys, CReal pExp,
                                char *errbuf, int errbuflen)
{
    KODIAK_GUARDED_VOID(minmax_system_minmax(pSys, pExp))
}


//////////////////////////////////////////////////////////////////////
//
// Real expression construction (guarded)
//
//////////////////////////////////////////////////////////////////////

int real_create_division_guarded(CReal num, CReal den, CReal *out,
                                char *errbuf, int errbuflen)
{
    KODIAK_GUARDED_VALUE(nullptr,
                        real_create_division(num, den),
                        result)
}


//////////////////////////////////////////////////////////////////////
//
// MinMaxSystem bound getters (guarded)
//
//////////////////////////////////////////////////////////////////////

int minmax_system_maximum_lower_bound_guarded(CMinMaxSystem p, double *out,
                                             char *errbuf, int errbuflen)
{
    KODIAK_GUARDED_VALUE(0.0,
                        minmax_system_maximum_lower_bound(p),
                        result)
}

int minmax_system_maximum_upper_bound_guarded(CMinMaxSystem p, double *out,
                                             char *errbuf, int errbuflen)
{
    KODIAK_GUARDED_VALUE(0.0,
                        minmax_system_maximum_upper_bound(p),
                        result)
}

int minmax_system_minimum_lower_bound_guarded(CMinMaxSystem p, double *out,
                                             char *errbuf, int errbuflen)
{
    KODIAK_GUARDED_VALUE(0.0,
                        minmax_system_minimum_lower_bound(p),
                        result)
}

int minmax_system_minimum_upper_bound_guarded(CMinMaxSystem p, double *out,
                                             char *errbuf, int errbuflen)
{
    KODIAK_GUARDED_VALUE(0.0,
                        minmax_system_minimum_upper_bound(p),
                        result)
}


//////////////////////////////////////////////////////////////////////
//
// Paver guarded operations
//
//////////////////////////////////////////////////////////////////////

int paver_pave_guarded(CPaver p, CBool pExpression,
                      char *errbuf, int errbuflen)
{
    KODIAK_GUARDED_VOID(paver_pave(p, pExpression))
}

int paver_save_paving_guarded(CPaver p, CString filename,
                             char *errbuf, int errbuflen)
{
    KODIAK_GUARDED_VOID(paver_save_paving(p, filename))
}
