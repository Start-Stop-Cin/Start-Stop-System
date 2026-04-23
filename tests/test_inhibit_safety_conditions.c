#include "unity.h"
#include "ss_inhibit_safety_conditions.h"

/*
 * UT-ISC-01
 * Requirement ID: Blank 
 * Category: Blank 
 * Requirement Summary: Blank 
 * Test Objective: Blank 
 */
void test_inhibit_safety_conditions_all_conditions_true(void)
{
    bool result = inhibit_safety_conditions(
        0.1f,   // inclination < max
        true,   // door closed
        true,   // seatbelt fastened
        SS_GEAR_D,
        true    // system enabled
    );

    TEST_ASSERT_TRUE(result);
}

/*
 * UT-ISC-02
 * Requirement ID: Blank 
 * Category: Blank 
 * Requirement Summary: Blank 
 * Test Objective: Blank 
 */
void test_inhibit_safety_conditions_high_inclination(void)
{
    bool result = inhibit_safety_conditions(
        1.0f,   // inclination too high
        true,
        true,
        SS_GEAR_D,
        true
    );

    TEST_ASSERT_FALSE(result);
}

/*
 * UT-ISC-03
 * Requirement ID: Blank 
 * Category: Blank 
 * Requirement Summary: Blank 
 * Test Objective: Blank 
 */
void test_inhibit_safety_conditions_door_open(void)
{
    bool result = inhibit_safety_conditions(
        0.1f,
        false,  // door open
        true,
        SS_GEAR_D,
        true
    );

    TEST_ASSERT_FALSE(result);
}

/*
 * UT-ISC-04
 * Requirement ID: Blank 
 * Category: Blank 
 * Requirement Summary: Blank 
 * Test Objective: Blank 
 */
void test_inhibit_safety_conditions_seatbelt_unfastened(void)
{
    bool result = inhibit_safety_conditions(
        0.1f,
        true,
        false,  // seatbelt off
        SS_GEAR_D,
        true
    );

    TEST_ASSERT_FALSE(result);
}

/*
 * UT-ISC-05
 * Requirement ID: Blank 
 * Category: Blank 
 * Requirement Summary: Blank 
 * Test Objective: Blank 
 */
void test_inhibit_safety_conditions_wrong_gear(void)
{
    bool result = inhibit_safety_conditions(
        0.1f,
        true,
        true,
        0,      // not D
        true
    );

    TEST_ASSERT_FALSE(result);
}

/*
 * UT-ISC-06
 * Requirement ID: Blank 
 * Category: Blank 
 * Requirement Summary: Blank 
 * Test Objective: Blank 
 */
void test_inhibit_safety_conditions_disabled_system(void)
{
    bool result = inhibit_safety_conditions(
        0.1f,
        true,
        true,
        SS_GEAR_D,
        false   // system disabled
    );

    TEST_ASSERT_FALSE(result);
}

/*
 * UT-ISC-07
 * Requirement ID: Blank 
 * Category: Blank 
 * Requirement Summary: Blank 
 * Test Objective: Blank 
 */
void test_inhibit_safety_conditions_boundary_inclination(void)
{
    bool result = inhibit_safety_conditions(
        SS_INCLINATION_MAX_RAD, // boundary condition
        true,
        true,
        SS_GEAR_D,
        true
    );

    TEST_ASSERT_TRUE(result);
}