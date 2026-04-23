#include "unity.h"
#include "ss_inhibit_safety_conditions.h"
#include <ss_calib.h>

void setUp(void) {}
void tearDown(void) {}

/*==========================================================
 * UT-ISC-01
 * Covers: SW-LL-FR-03, SW-LL-FR-04, SW-LL-FR-05, SW-LL-FR-06 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_all_conditions_true(void)
{
    bool result = inhibit_safety_conditions(
        0.1f,   // inclination < max [cite: 15]
        true,   // door closed [cite: 15]
        true,   // seatbelt fastened [cite: 15]
        SS_GEAR_D, // gear is D [cite: 15]
        true    // system enabled [cite: 15]
    );

    TEST_ASSERT_TRUE(result);
}

/*==========================================================
 * UT-ISC-02
 * Covers: SW-LL-FR-04 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_high_inclination(void)
{
    bool result = inhibit_safety_conditions(
        1.0f,   // inclination too high [cite: 15]
        true,
        true,
        SS_GEAR_D,
        true
    );

    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-03
 * Covers: SW-LL-FR-05 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_door_open(void)
{
    bool result = inhibit_safety_conditions(
        0.1f,
        false,  // door open [cite: 15]
        true,
        SS_GEAR_D,
        true
    );

    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-04
 * Covers: SW-LL-FR-06 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_seatbelt_unfastened(void)
{
    bool result = inhibit_safety_conditions(
        0.1f,
        true,
        false,  // seatbelt off [cite: 15]
        SS_GEAR_D,
        true
    );

    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-05
 * Covers: SW-LL-FR-03 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_wrong_gear(void)
{
    bool result = inhibit_safety_conditions(
        0.1f,
        true,
        true,
        0,      // not D [cite: 15]
        true
    );

    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-06
 * Covers: SW-LL-FR-14 [cite: 16]
 *==========================================================*/
void test_inhibit_safety_conditions_disabled_system(void)
{
    bool result = inhibit_safety_conditions(
        0.1f,
        true,
        true,
        SS_GEAR_D,
        false   // system disabled [cite: 16]
    );

    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-07
 * Covers: SW-LL-FR-04 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_boundary_inclination(void)
{
    bool result = inhibit_safety_conditions(
        SS_INCLINATION_MAX_RAD, // boundary condition [cite: 15]
        true,
        true,
        SS_GEAR_D,
        true
    );

    TEST_ASSERT_TRUE(result);
}

/*==========================================================
 * UT-ISC-08
 * Covers: SW-LL-FR-04 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_zero_inclination(void)
{
    bool result = inhibit_safety_conditions(0.0f, true, true, SS_GEAR_D, true);
    TEST_ASSERT_TRUE(result);
}

/*==========================================================
 * UT-ISC-09
 * Covers: SW-LL-FR-04 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_negative_inclination(void)
{
    bool result = inhibit_safety_conditions(-0.2f, true, true, SS_GEAR_D, true);
    TEST_ASSERT_TRUE(result);
}

/*==========================================================
 * UT-ISC-10
 * Covers: SW-LL-FR-03, SW-LL-FR-04, SW-LL-FR-05, SW-LL-FR-06, SW-LL-FR-14 [cite: 15, 16]
 *==========================================================*/
void test_inhibit_safety_conditions_all_false(void)
{
    bool result = inhibit_safety_conditions(1.0f, false, false, 0, false);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-11
 * Covers: SW-LL-FR-03, SW-LL-FR-04, SW-LL-FR-05, SW-LL-FR-06 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_only_enabled_true(void)
{
    bool result = inhibit_safety_conditions(1.0f, false, false, 0, true);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-12
 * Covers: SW-LL-FR-05 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_only_door_true(void)
{
    bool result = inhibit_safety_conditions(1.0f, true, false, 0, false);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-13
 * Covers: SW-LL-FR-06 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_only_seatbelt_true(void)
{
    bool result = inhibit_safety_conditions(1.0f, false, true, 0, false);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-14
 * Covers: SW-LL-FR-03 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_only_gear_correct(void)
{
    bool result = inhibit_safety_conditions(1.0f, false, false, SS_GEAR_D, false);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-15
 * Covers: SW-LL-FR-04 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_inclination_just_below_limit(void)
{
    bool result = inhibit_safety_conditions(SS_INCLINATION_MAX_RAD - 0.0001f, true, true, SS_GEAR_D, true);
    TEST_ASSERT_TRUE(result);
}

/*==========================================================
 * UT-ISC-16
 * Covers: SW-LL-FR-04 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_inclination_just_above_limit(void)
{
    bool result = inhibit_safety_conditions(SS_INCLINATION_MAX_RAD + 0.0001f, true, true, SS_GEAR_D, true);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-17
 * Covers: SW-LL-FR-05, SW-LL-FR-06 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_door_and_seatbelt_false(void)
{
    bool result = inhibit_safety_conditions(0.1f, false, false, SS_GEAR_D, true);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-18
 * Covers: SW-LL-FR-03 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_wrong_gear_but_all_else_ok(void)
{
    bool result = inhibit_safety_conditions(0.1f, true, true, 2, true);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-19
 * Covers: SW-LL-FR-14 [cite: 16]
 *==========================================================*/
void test_inhibit_safety_conditions_disabled_with_valid_inputs(void)
{
    bool result = inhibit_safety_conditions(0.1f, true, true, SS_GEAR_D, false);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-20
 * Covers: SW-LL-FR-04, SW-LL-FR-05 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_high_inclination_and_door_open(void)
{
    bool result = inhibit_safety_conditions(1.0f, false, true, SS_GEAR_D, true);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-21
 * Covers: SW-LL-FR-04, SW-LL-FR-06 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_high_inclination_and_seatbelt_off(void)
{
    bool result = inhibit_safety_conditions(1.0f, true, false, SS_GEAR_D, true);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-22
 * Covers: SW-LL-FR-03, SW-LL-FR-14 [cite: 15, 16]
 *==========================================================*/
void test_inhibit_safety_conditions_valid_except_gear_and_enabled(void)
{
    bool result = inhibit_safety_conditions(0.1f, true, true, 0, false);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-23
 * Covers: SW-LL-FR-04, SW-LL-FR-05 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_boundary_with_door_open(void)
{
    bool result = inhibit_safety_conditions(SS_INCLINATION_MAX_RAD, false, true, SS_GEAR_D, true);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-24
 * Covers: SW-LL-FR-04, SW-LL-FR-06 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_boundary_with_seatbelt_off(void)
{
    bool result = inhibit_safety_conditions(SS_INCLINATION_MAX_RAD, true, false, SS_GEAR_D, true);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-25
 * Covers: SW-LL-FR-04, SW-LL-FR-03 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_boundary_with_wrong_gear(void)
{
    bool result = inhibit_safety_conditions(SS_INCLINATION_MAX_RAD, true, true, 0, true);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-26
 * Covers: SW-LL-FR-04, SW-LL-FR-14 [cite: 15, 16]
 *==========================================================*/
void test_inhibit_safety_conditions_boundary_with_disabled_system(void)
{
    bool result = inhibit_safety_conditions(SS_INCLINATION_MAX_RAD, true, true, SS_GEAR_D, false);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-27
 * Covers: SW-LL-FR-04 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_small_positive_inclination(void)
{
    bool result = inhibit_safety_conditions(0.0001f, true, true, SS_GEAR_D, true);
    TEST_ASSERT_TRUE(result);
}

/*==========================================================
 * UT-ISC-28
 * Covers: SW-LL-FR-04 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_large_negative_inclination(void)
{
    bool result = inhibit_safety_conditions(-5.0f, true, true, SS_GEAR_D, true);
    TEST_ASSERT_TRUE(result);
}

/*==========================================================
 * UT-ISC-29
 * Covers: SW-LL-FR-04 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_large_positive_inclination(void)
{
    bool result = inhibit_safety_conditions(100.0f, true, true, SS_GEAR_D, true);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-30
 * Covers: SW-LL-FR-05 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_all_valid_except_door(void)
{
    bool result = inhibit_safety_conditions(0.1f, false, true, SS_GEAR_D, true);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-31
 * Covers: SW-LL-FR-06 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_all_valid_except_seatbelt(void)
{
    bool result = inhibit_safety_conditions(0.1f, true, false, SS_GEAR_D, true);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-32
 * Covers: SW-LL-FR-03 [cite: 15]
 *==========================================================*/
void test_inhibit_safety_conditions_all_valid_except_gear(void)
{
    bool result = inhibit_safety_conditions(0.1f, true, true, 99, true);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-33
 * Covers: SW-LL-FR-14 [cite: 16]
 *==========================================================*/
void test_inhibit_safety_conditions_all_valid_except_enabled(void)
{
    bool result = inhibit_safety_conditions(0.1f, true, true, SS_GEAR_D, false);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-ISC-34
 * Covers: SW-LL-FR-03, SW-LL-FR-04, SW-LL-FR-05, SW-LL-FR-06, SW-LL-FR-14 [cite: 15, 16]
 *==========================================================*/
void test_inhibit_safety_conditions_minimum_inputs_true_case(void)
{
    bool result = inhibit_safety_conditions(0.0f, true, true, SS_GEAR_D, true);
    TEST_ASSERT_TRUE(result);
}

/*==========================================================
 * UT-ISC-35
 * Covers: SW-LL-FR-03, SW-LL-FR-04, SW-LL-FR-05, SW-LL-FR-06, SW-LL-FR-14 [cite: 15, 16]
 *==========================================================*/
void test_inhibit_safety_conditions_maximum_edge_failure(void)
{
    bool result = inhibit_safety_conditions(SS_INCLINATION_MAX_RAD + 1.0f, false, false, 0, false);
    TEST_ASSERT_FALSE(result);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_inhibit_safety_conditions_all_conditions_true);
    RUN_TEST(test_inhibit_safety_conditions_high_inclination);
    RUN_TEST(test_inhibit_safety_conditions_door_open);
    RUN_TEST(test_inhibit_safety_conditions_seatbelt_unfastened);
    RUN_TEST(test_inhibit_safety_conditions_wrong_gear);
    RUN_TEST(test_inhibit_safety_conditions_disabled_system);
    RUN_TEST(test_inhibit_safety_conditions_boundary_inclination);

    RUN_TEST(test_inhibit_safety_conditions_zero_inclination);
    RUN_TEST(test_inhibit_safety_conditions_negative_inclination);
    RUN_TEST(test_inhibit_safety_conditions_all_false);
    RUN_TEST(test_inhibit_safety_conditions_only_enabled_true);
    RUN_TEST(test_inhibit_safety_conditions_only_door_true);
    RUN_TEST(test_inhibit_safety_conditions_only_seatbelt_true);
    RUN_TEST(test_inhibit_safety_conditions_only_gear_correct);
    RUN_TEST(test_inhibit_safety_conditions_inclination_just_below_limit);
    RUN_TEST(test_inhibit_safety_conditions_inclination_just_above_limit);
    RUN_TEST(test_inhibit_safety_conditions_door_and_seatbelt_false);
    RUN_TEST(test_inhibit_safety_conditions_wrong_gear_but_all_else_ok);
    RUN_TEST(test_inhibit_safety_conditions_disabled_with_valid_inputs);
    RUN_TEST(test_inhibit_safety_conditions_high_inclination_and_door_open);
    RUN_TEST(test_inhibit_safety_conditions_high_inclination_and_seatbelt_off);
    RUN_TEST(test_inhibit_safety_conditions_valid_except_gear_and_enabled);
    RUN_TEST(test_inhibit_safety_conditions_boundary_with_door_open);
    RUN_TEST(test_inhibit_safety_conditions_boundary_with_seatbelt_off);
    RUN_TEST(test_inhibit_safety_conditions_boundary_with_wrong_gear);
    RUN_TEST(test_inhibit_safety_conditions_boundary_with_disabled_system);
    RUN_TEST(test_inhibit_safety_conditions_small_positive_inclination);
    RUN_TEST(test_inhibit_safety_conditions_large_negative_inclination);
    RUN_TEST(test_inhibit_safety_conditions_large_positive_inclination);
    RUN_TEST(test_inhibit_safety_conditions_all_valid_except_door);
    RUN_TEST(test_inhibit_safety_conditions_all_valid_except_seatbelt);
    RUN_TEST(test_inhibit_safety_conditions_all_valid_except_gear);
    RUN_TEST(test_inhibit_safety_conditions_all_valid_except_enabled);
    RUN_TEST(test_inhibit_safety_conditions_minimum_inputs_true_case);
    RUN_TEST(test_inhibit_safety_conditions_maximum_edge_failure);

    return UNITY_END();
}
