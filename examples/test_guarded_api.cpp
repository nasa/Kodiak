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

// Test program for Kodiak guarded API (v2.1.0+)
//
// Tests the exception-safe C API wrappers that return status codes instead
// of throwing C++ exceptions. This validates the integration for FFI use cases.

#include <iostream>
#include <cstring>
#include "../src/Adapters/Codiak.h"

using std::cout;
using std::endl;

int tests_passed = 0;
int tests_failed = 0;

#define TEST(name) \
    cout << "\n[TEST] " << name << "..." << endl; \
    bool test_passed = true;

#define ASSERT(condition, msg) \
    if (!(condition)) { \
        cout << "  FAIL: " << msg << endl; \
        test_passed = false; \
    }

#define END_TEST() \
    if (test_passed) { \
        tests_passed++; \
        cout << "  PASSED" << endl; \
    } else { \
        tests_failed++; \
        cout << "  FAILED" << endl; \
    }

// Test 1: Status codes are defined correctly
void test_status_codes() {
    TEST("Status code definitions")
    ASSERT(KODIAK_OK == 0, "KODIAK_OK == 0")
    ASSERT(KODIAK_DIV_BY_ZERO == 1, "KODIAK_DIV_BY_ZERO == 1")
    ASSERT(KODIAK_ERROR == 2, "KODIAK_ERROR == 2")
    END_TEST()
}

// Test 2: minmax_system_maximize_guarded success path
void test_maximize_guarded_success() {
    TEST("minmax_system_maximize_guarded - success path")

    CMinMaxSystem sys = minmax_system_create("test_system");
    ASSERT(sys != nullptr, "System created")

    CInterval lb = interval_create(0.0, 0.0);
    CInterval ub = interval_create(10.0, 10.0);
    minmax_system_register_variable(sys, "x", lb, ub);

    CReal x = real_create_variable(0, "x");
    CReal x_squared = real_create_multiplication(x, x);

    char errbuf[256];
    int status = minmax_system_maximize_guarded(sys, x_squared, errbuf, sizeof(errbuf));
    ASSERT(status == KODIAK_OK, "Status is KODIAK_OK")

    double lb_out, ub_out;
    status = minmax_system_maximum_lower_bound_guarded(sys, &lb_out, errbuf, sizeof(errbuf));
    ASSERT(status == KODIAK_OK, "Lower bound getter succeeds")

    status = minmax_system_maximum_upper_bound_guarded(sys, &ub_out, errbuf, sizeof(errbuf));
    ASSERT(status == KODIAK_OK, "Upper bound getter succeeds")

    ASSERT(lb_out >= 90.0 && ub_out <= 110.0, "Bounds are reasonable (around 100)")

    END_TEST()
}

// Test 3: real_create_division_guarded with div-by-zero
void test_division_guarded_div_by_zero() {
    TEST("real_create_division_guarded - division by zero")

    CInterval one = interval_create(1.0, 1.0);
    CReal num = real_create_value(one);

    CInterval zero_interval = interval_create(-1.0, 1.0);
    CReal den = real_create_value(zero_interval);

    CReal result = nullptr;
    char errbuf[256];
    int status = real_create_division_guarded(num, den, &result, errbuf, sizeof(errbuf));

    ASSERT(status == KODIAK_DIV_BY_ZERO, "Status is KODIAK_DIV_BY_ZERO")
    ASSERT(result == nullptr, "Result pointer is NULL on failure")
    ASSERT(std::strstr(errbuf, "division by an interval that contains zero") != nullptr,
           "Error message contains expected text")

    cout << "  Error message: \"" << errbuf << "\"" << endl;

    END_TEST()
}

// Test 4: real_create_division_guarded success
void test_division_guarded_success() {
    TEST("real_create_division_guarded - success path")

    CInterval ten = interval_create(10.0, 10.0);
    CReal num = real_create_value(ten);

    CInterval two = interval_create(2.0, 2.0);
    CReal den = real_create_value(two);

    CReal result = nullptr;
    char errbuf[256];
    int status = real_create_division_guarded(num, den, &result, errbuf, sizeof(errbuf));

    ASSERT(status == KODIAK_OK, "Status is KODIAK_OK")
    ASSERT(result != nullptr, "Result pointer is non-NULL on success")

    END_TEST()
}

// Test 5: Error buffer handling - NULL buffer
void test_error_buffer_null() {
    TEST("Error buffer handling - NULL buffer (no crash)")

    CInterval one = interval_create(1.0, 1.0);
    CReal num = real_create_value(one);
    CInterval zero = interval_create(-1.0, 1.0);
    CReal den = real_create_value(zero);

    CReal result = nullptr;
    int status = real_create_division_guarded(num, den, &result, nullptr, 0);

    ASSERT(status == KODIAK_DIV_BY_ZERO, "Status is KODIAK_DIV_BY_ZERO")

    END_TEST()
}

// Test 6: Error buffer handling - truncation
void test_error_buffer_truncation() {
    TEST("Error buffer handling - truncation and NUL-termination")

    CInterval one = interval_create(1.0, 1.0);
    CReal num = real_create_value(one);
    CInterval zero = interval_create(-1.0, 1.0);
    CReal den = real_create_value(zero);

    char errbuf[20];
    CReal result = nullptr;
    int status = real_create_division_guarded(num, den, &result, errbuf, sizeof(errbuf));

    ASSERT(status == KODIAK_DIV_BY_ZERO, "Status is KODIAK_DIV_BY_ZERO")
    ASSERT(errbuf[19] == '\0', "Buffer is NUL-terminated")
    ASSERT(std::strlen(errbuf) == 19, "Message truncated to fit")

    cout << "  Truncated message: \"" << errbuf << "\"" << endl;

    END_TEST()
}

// Test 7: minmax_system_minmax_guarded
void test_minmax_guarded() {
    TEST("minmax_system_minmax_guarded - min and max bounds")

    CMinMaxSystem sys = minmax_system_create("minmax_test");

    CInterval lb = interval_create(-5.0, -5.0);
    CInterval ub = interval_create(5.0, 5.0);
    minmax_system_register_variable(sys, "x", lb, ub);

    CReal x = real_create_variable(0, "x");

    char errbuf[256];
    int status = minmax_system_minmax_guarded(sys, x, errbuf, sizeof(errbuf));
    ASSERT(status == KODIAK_OK, "minmax succeeds")

    double min_lb;
    status = minmax_system_minimum_lower_bound_guarded(sys, &min_lb, errbuf, sizeof(errbuf));
    ASSERT(status == KODIAK_OK, "minimum_lower_bound succeeds")

    double max_ub;
    status = minmax_system_maximum_upper_bound_guarded(sys, &max_ub, errbuf, sizeof(errbuf));
    ASSERT(status == KODIAK_OK, "maximum_upper_bound succeeds")

    ASSERT(min_lb >= -6.0 && min_lb <= -4.0, "Minimum bound is reasonable")
    ASSERT(max_ub >= 4.0 && max_ub <= 6.0, "Maximum bound is reasonable")

    cout << "  Min lower bound: " << min_lb << ", Max upper bound: " << max_ub << endl;

    END_TEST()
}

// Test 8: paver_pave_guarded success
void test_paver_guarded_success() {
    TEST("paver_pave_guarded - success path")

    CPaver paver = paver_create("test_paver");
    ASSERT(paver != nullptr, "Paver created")

    CInterval lb = interval_create(0.0, 0.0);
    CInterval ub = interval_create(10.0, 10.0);
    paver_register_variable(paver, "x", lb, ub);
    paver_set_maxdepth(paver, 5);
    paver_set_precision(paver, 10);

    CReal x = real_create_variable(0, "x");
    CInterval five = interval_create(5.0, 5.0);
    CReal five_real = real_create_value(five);
    CBool condition = bool_create_greater_than(x, five_real);

    char errbuf[256];
    int status = paver_pave_guarded(paver, condition, errbuf, sizeof(errbuf));
    ASSERT(status == KODIAK_OK, "paver_pave succeeds")

    END_TEST()
}

int main(int, char *[]) {
    cout << "\n======================================" << endl;
    cout << "Kodiak Guarded API Test Suite (v2.1.0)" << endl;
    cout << "======================================\n" << endl;

    test_status_codes();
    test_maximize_guarded_success();
    test_division_guarded_div_by_zero();
    test_division_guarded_success();
    test_error_buffer_null();
    test_error_buffer_truncation();
    test_minmax_guarded();
    test_paver_guarded_success();

    cout << "\n======================================" << endl;
    cout << "Test Summary" << endl;
    cout << "======================================" << endl;
    cout << "Passed: " << tests_passed << endl;
    cout << "Failed: " << tests_failed << endl;
    cout << "Total:  " << (tests_passed + tests_failed) << endl;

    if (tests_failed == 0) {
        cout << "\n✓ ALL TESTS PASSED!\n" << endl;
        return 0;
    } else {
        cout << "\n✗ SOME TESTS FAILED\n" << endl;
        return 1;
    }
}
