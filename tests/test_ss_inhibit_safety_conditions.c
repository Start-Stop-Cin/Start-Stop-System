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

/*==========================================================
 * UT-STM-01
 * Test standstill_management initial state
 *==========================================================*/
void test_standstill_management_initial_false(void)
{
    bool result = standstill_management(false, false, true, true);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-STM-02
 * Test standstill_management set condition
 *==========================================================*/
void test_standstill_management_set_condition(void)
{
    bool result1 = standstill_management(true, false, false, true);
    TEST_ASSERT_FALSE(result1);

    bool result2 = standstill_management(true, false, false, true);
    TEST_ASSERT_TRUE(result2);
}

/*==========================================================
 * UT-STM-03
 * Test standstill_management reset condition
 *==========================================================*/
void test_standstill_management_reset_condition(void)
{
    standstill_management(true, false, false, true);
    bool result1 = standstill_management(true, true, true, true);
    TEST_ASSERT_TRUE(result1);

    bool result2 = standstill_management(true, true, true, true);
    TEST_ASSERT_FALSE(result2);
}

/*==========================================================
 * UT-STM-04
 * Test standstill_management reset after set
 *==========================================================*/
void test_standstill_management_reset_after_set(void)
{
    standstill_management(true, false, false, true);
    bool result = standstill_management(false, true, true, true);
    TEST_ASSERT_TRUE(result);
}

/*==========================================================
 * UT-STM-05
 * Test standstill_management hold state
 *==========================================================*/
void test_standstill_management_hold_state(void)
{
    standstill_management(true, false, false, true);
    bool result = standstill_management(false, false, true, true);
    TEST_ASSERT_TRUE(result);
}

/*==========================================================
 * UT-DMB-01
 * Test drivecycle_memory_block ignition off
 *==========================================================*/
void test_drivecycle_memory_block_ignition_off(void)
{
    bool result = drivecycle_memory_block(false, false, 0.0f);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-DMB-02
 * Test drivecycle_memory_block engine stop request
 *==========================================================*/
void test_drivecycle_memory_block_engine_stop_request(void)
{
    bool result = drivecycle_memory_block(true, true, 0.0f);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-DMB-03
 * Test drivecycle_memory_block high speed
 *==========================================================*/
void test_drivecycle_memory_block_high_speed(void)
{
    bool result = drivecycle_memory_block(true, false, 10.0f);
    TEST_ASSERT_TRUE(result);

    bool result2 = drivecycle_memory_block(true, false, 10.0f);
    TEST_ASSERT_TRUE(result2);
}

/*==========================================================
 * UT-DMB-04
 * Test drivecycle_memory_block memory effect
 *==========================================================*/
void test_drivecycle_memory_block_memory_effect(void)
{
    drivecycle_memory_block(true, false, 10.0f);
    bool result = drivecycle_memory_block(true, false, 5.0f);
    TEST_ASSERT_TRUE(result);
}

/*==========================================================
 * UT-DMB-05
 * Test drivecycle_memory_block reset memory
 *==========================================================*/
void test_drivecycle_memory_block_reset_memory(void)
{
    drivecycle_memory_block(true, false, 10.0f);
    bool result = drivecycle_memory_block(false, false, 5.0f);
    TEST_ASSERT_FALSE(result);

    bool result2 = drivecycle_memory_block(true, false, 5.0f);
    TEST_ASSERT_FALSE(result2);
}

/*==========================================================
 * UT-DMB-06
 * Test drivecycle_memory_block low speed after reset
 *==========================================================*/
void test_drivecycle_memory_block_low_speed(void)
{
    drivecycle_memory_block(false, false, 5.0f); // reset
    bool result = drivecycle_memory_block(true, false, 5.0f);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-SRF-01
 * Test sr_flip_flop set
 *==========================================================*/
void test_sr_flip_flop_set(void)
{
    sr_state_t state = {0};
    bool result = sr_flip_flop(true, false, &state);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_TRUE(state.Q);
}

/*==========================================================
 * UT-SRF-02
 * Test sr_flip_flop reset
 *==========================================================*/
void test_sr_flip_flop_reset(void)
{
    sr_state_t state = {1};
    bool result = sr_flip_flop(false, true, &state);
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_FALSE(state.Q);
}

/*==========================================================
 * UT-SRF-03
 * Test sr_flip_flop hold S=0 R=0
 *==========================================================*/
void test_sr_flip_flop_hold_s0_r0(void)
{
    sr_state_t state = {1};
    bool result = sr_flip_flop(false, false, &state);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_TRUE(state.Q);
}

/*==========================================================
 * UT-SRF-04
 * Test sr_flip_flop hold S=1 R=1
 *==========================================================*/
void test_sr_flip_flop_hold_s1_r1(void)
{
    sr_state_t state = {1};
    bool result = sr_flip_flop(true, true, &state);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_TRUE(state.Q);
}

/*==========================================================
 * UT-SRF-05
 * Test sr_flip_flop null state
 *==========================================================*/
void test_sr_flip_flop_null_state(void)
{
    bool result = sr_flip_flop(true, false, NULL);
    TEST_ASSERT_FALSE(result);
}

/*==========================================================
 * UT-STM-06
 * Test standstill_management with specific condition
 *==========================================================*/
void test_standstill_management_setvalue_c3_true(void)
{
    // First call to reset state
    standstill_management(false, false, true, true);
    // AutoStopActive=true, DoorStatus=true, SeatbeltStatus=false -> setValue = true && (false || true) = true
    // resetValue = false && true && false = false
    // Q = previous Q (false), SafeStop returns Q_d
    bool result = standstill_management(true, false, true, false);
    TEST_ASSERT_TRUE(result); // Should be true since sr_flip_flop holds state when Q=false, S=true, R=false leads to Q=true
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

    RUN_TEST(test_standstill_management_initial_false);
    RUN_TEST(test_standstill_management_set_condition);
    RUN_TEST(test_standstill_management_reset_condition);
    RUN_TEST(test_standstill_management_reset_after_set);
    RUN_TEST(test_standstill_management_hold_state);

    RUN_TEST(test_drivecycle_memory_block_ignition_off);
    RUN_TEST(test_drivecycle_memory_block_engine_stop_request);
    RUN_TEST(test_drivecycle_memory_block_high_speed);
    RUN_TEST(test_drivecycle_memory_block_memory_effect);
    RUN_TEST(test_drivecycle_memory_block_reset_memory);
    RUN_TEST(test_drivecycle_memory_block_low_speed);

    RUN_TEST(test_sr_flip_flop_set);
    RUN_TEST(test_sr_flip_flop_reset);
    RUN_TEST(test_sr_flip_flop_hold_s0_r0);
    RUN_TEST(test_sr_flip_flop_hold_s1_r1);
    RUN_TEST(test_sr_flip_flop_null_state);

    RUN_TEST(test_standstill_management_setvalue_c3_true);

    return UNITY_END();
}
