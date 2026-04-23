#include "unity.h"
#include "co2.h"
#include "ss_calib.h"

void setUp(void) { Co2_Init(); }
void tearDown(void) {}

/* =============================================================================
 *  Co2_Init
 * ============================================================================= */

/*
 * UT-CO2-11
 * Requirement ID: N/A
 * Category: Initialization
 * Requirement Summary: CO2 module initialization behavior
 * Test Objective: Verify initialization resets accumulated AutoStop time to zero
 */
void test_co2_init_resets_accumulated_time(void) {
    Co2_Run10ms(true, true);
    Co2_Init();
    TEST_ASSERT_EQUAL_UINT32(0U, Co2_GetAccumulatedAutoStopTimeMs());
}

/*
 * UT-CO2-12
 * Requirement ID: N/A
 * Category: Initialization
 * Requirement Summary: CO2 module initialization behavior
 * Test Objective: Verify initialization resets CO2 avoided grams to zero
 */
void test_co2_init_resets_avoided_grams(void) {
    Co2_Run10ms(true, true);
    Co2_Init();
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.0f, Co2_GetAvoidedGrams());
}

/*
 * UT-CO2-13
 * Requirement ID: N/A
 * Category: Initialization
 * Requirement Summary: CO2 module initialization behavior
 * Test Objective: Verify initialization disables display flag
 */
void test_co2_init_display_disabled(void) {
    Co2_Run10ms(true, true);
    Co2_Init();
    TEST_ASSERT_FALSE(Co2_DisplayEnabled());
}

/*
 * UT-CO2-14
 * Requirement ID: N/A
 * Category: Initialization
 * Requirement Summary: CO2 module initialization behavior
 * Test Objective: Verify initialization enables hide flag
 */
void test_co2_init_hide_enabled(void) {
    Co2_Run10ms(true, true);
    Co2_Init();
    TEST_ASSERT_TRUE(Co2_HideEnabled());
}

/* =============================================================================
 *  Co2_Run10ms: ignition OFF (MC/DC condition 1 = false)
 * ============================================================================= */

/*
 * UT-CO2-03
 * Requirement ID: SW-LL-FR-24
 * Category: CO2 / Fuel Estimation
 * Requirement Summary: The software must reset TIME_AUTO_STOP = 0 when IgnitionOFF == TRUE
 * Test Objective: Verify that accumulated AutoStop time resets to zero when ignition is turned OFF
 */
void test_co2_ignition_off_resets_time(void) {
    Co2_Run10ms(true, true);
    Co2_Run10ms(false, false);
    TEST_ASSERT_EQUAL_UINT32(0U, Co2_GetAccumulatedAutoStopTimeMs());
}

/*
 * UT-CO2-08
 * Requirement ID: SW-LL-FR-35
 * Category: CO2 Estimation
 * Requirement Summary: The software must set CO2_AVOIDED = 0 when IgnitionStatus == FALSE
 * Test Objective: Verify CO2 avoided value resets to zero when ignition is OFF
 */
void test_co2_ignition_off_resets_avoided_grams(void) {
    Co2_Run10ms(true, true);
    Co2_Run10ms(false, false);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.0f, Co2_GetAvoidedGrams());
}

/*
 * UT-CO2-09
 * Requirement ID: SW-LL-FR-35
 * Category: CO2 Estimation
 * Requirement Summary: The software must set CO2_AVOIDED = 0 when IgnitionStatus == FALSE
 * Test Objective: Verify display flag is disabled when ignition is OFF
 */
void test_co2_ignition_off_disables_display(void) {
    Co2_Run10ms(true, true);
    Co2_Run10ms(false, false);
    TEST_ASSERT_FALSE(Co2_DisplayEnabled());
}

/*
 * UT-CO2-10
 * Requirement ID: SW-LL-FR-35
 * Category: CO2 Estimation
 * Requirement Summary: The software must set CO2_AVOIDED = 0 when IgnitionStatus == FALSE
 * Test Objective: Verify hide flag is enabled when ignition is OFF
 */
void test_co2_ignition_off_enables_hide(void) {
    Co2_Run10ms(true, true);
    Co2_Run10ms(false, false);
    TEST_ASSERT_TRUE(Co2_HideEnabled());
}

/* =============================================================================
 *  Co2_Run10ms: ignition ON + autostop OFF (MC/DC condition 1 = true, condition 2 = false)
 * ============================================================================= */

/*
 * UT-CO2-15
 * Requirement ID: N/A
 * Category: Negative Case
 * Requirement Summary: CO2 accumulation must not occur when AutoStop is inactive
 * Test Objective: Verify accumulated time does not increase when AutoStop is inactive
 */
void test_co2_autostop_inactive_does_not_increment_time(void) {
    Co2_Run10ms(true, false);
    TEST_ASSERT_EQUAL_UINT32(0U, Co2_GetAccumulatedAutoStopTimeMs());
}

/*
 * UT-CO2-07
 * Requirement ID: SW-LL-FR-27
 * Category: CO2 Estimation
 * Requirement Summary: The software must command the cluster to hide CO2_AVOIDED when AutostopActive == FALSE
 * Test Objective: Verify display flag is disabled when AutoStop is inactive
 */
void test_co2_autostop_inactive_disables_display(void) {
    Co2_Run10ms(true, false);
    TEST_ASSERT_FALSE(Co2_DisplayEnabled());
}

/*
 * UT-CO2-06
 * Requirement ID: SW-LL-FR-27
 * Category: CO2 Estimation
 * Requirement Summary: The software must command the cluster to hide CO2_AVOIDED when AutostopActive == FALSE
 * Test Objective: Verify hide flag is enabled when AutoStop is inactive
 */
void test_co2_autostop_inactive_enables_hide(void) {
    Co2_Run10ms(true, false);
    TEST_ASSERT_TRUE(Co2_HideEnabled());
}

/* =============================================================================
 *  Co2_Run10ms: ignition ON + autostop ON (MC/DC condition 1 = true, condition 2 = true)
 * ============================================================================= */

/*
 * UT-CO2-01
 * Requirement ID: SW-LL-FR-23
 * Category: CO2 / Fuel Estimation
 * Requirement Summary: The software must increment TIME_AUTO_STOP by SS_TASK_PERIOD_MS every control cycle when AutostopActive == TRUE
 * Test Objective: Verify that accumulated AutoStop time is incremented by one task period (10ms) when AutoStop is active
 */
void test_co2_autostop_active_increments_time_by_task_period(void) {
    Co2_Run10ms(true, true);
    TEST_ASSERT_EQUAL_UINT32(SS_TASK_PERIOD_MS, Co2_GetAccumulatedAutoStopTimeMs());
}

/*
 * UT-CO2-02
 * Requirement ID: SW-LL-FR-23
 * Category: CO2 / Fuel Estimation
 * Requirement Summary: The software must increment TIME_AUTO_STOP by SS_TASK_PERIOD_MS every control cycle when AutostopActive == TRUE
 * Test Objective: Verify accumulated AutoStop time over multiple consecutive active cycles
 */
void test_co2_autostop_active_increments_time_multiple_calls(void) {
    Co2_Run10ms(true, true);
    Co2_Run10ms(true, true);
    Co2_Run10ms(true, true);
    TEST_ASSERT_EQUAL_UINT32(3U * SS_TASK_PERIOD_MS, Co2_GetAccumulatedAutoStopTimeMs());
}

/*
 * UT-CO2-04
 * Requirement ID: SW-LL-FR-25
 * Category: CO2 Estimation
 * Requirement Summary: The software must compute CO2_AVOIDED = TIME_AUTO_STOP x WLTP_IDLE_FACTOR when AutostopActive == TRUE
 * Test Objective: Verify CO2 avoided calculation based on accumulated AutoStop time and WLTP idle factor
 */
void test_co2_autostop_active_calculates_avoided_grams(void) {
    Co2_Run10ms(true, true);
    float expected = ((float)SS_TASK_PERIOD_MS / 1000.0f) * SS_CO2_WLTP_IDLE_FACTOR;
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, expected, Co2_GetAvoidedGrams());
}

/*
 * UT-CO2-05
 * Requirement ID: SW-LL-FR-26
 * Category: CO2 Estimation
 * Requirement Summary: The software must command the cluster to display CO2_AVOIDED when AutostopActive == TRUE
 * Test Objective: Verify display flag is enabled when AutoStop is active
 */
void test_co2_autostop_active_enables_display(void) {
    Co2_Run10ms(true, true);
    TEST_ASSERT_TRUE(Co2_DisplayEnabled());
}

/*
 * UT-CO2-17
 * Requirement ID: N/A
 * Category: Display Logic
 * Requirement Summary: Hide flag behavior when AutoStop is active
 * Test Objective: Verify hide flag is disabled when AutoStop is active
 */
void test_co2_autostop_active_disables_hide(void) {
    Co2_Run10ms(true, true);
    TEST_ASSERT_FALSE(Co2_HideEnabled());
}

/* =============================================================================
 *  CO2 avoided = 0 while ignition ON + autostop OFF
 * ============================================================================= */

/*
 * UT-CO2-16
 * Requirement ID: N/A
 * Category: Negative Case
 * Requirement Summary: CO2 avoided must remain zero when AutoStop is inactive
 * Test Objective: Verify CO2 avoided remains zero while AutoStop is inactive
 */
void test_co2_avoided_grams_zero_when_autostop_inactive(void) {
    Co2_Run10ms(true, false);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.0f, Co2_GetAvoidedGrams());
}

/* =============================================================================
 *  Testes extras: Sequence, Equivalence Partition, Boundary
 * ============================================================================= */

/*
 * UT-CO2-18
 * Requirement ID: SW-LL-FR-23
 * Category: CO2 / Fuel Estimation
 * Requirement Summary: The software must increment TIME_AUTO_STOP by SS_TASK_PERIOD_MS every control cycle when AutostopActive == TRUE
 * Test Objective: Verify accumulated time is preserved across active -> inactive -> active transitions
 */
void test_co2_transition_active_inactive_active_preserves_time(void) {
    Co2_Run10ms(true, true);
    Co2_Run10ms(true, true);
    Co2_Run10ms(true, false);
    Co2_Run10ms(true, true);
    TEST_ASSERT_EQUAL_UINT32(3U * SS_TASK_PERIOD_MS, Co2_GetAccumulatedAutoStopTimeMs());
}

/*
 * UT-CO2-19
 * Requirement ID: SW-LL-FR-24
 * Category: CO2 / Fuel Estimation
 * Requirement Summary: The software must reset TIME_AUTO_STOP = 0 when IgnitionOFF == TRUE
 * Test Objective: Verify full ignition cycle resets all state and module restarts from zero
 */
void test_co2_ignition_cycle_resets_and_restarts_clean(void) {
    Co2_Run10ms(true, true);
    Co2_Run10ms(true, true);
    Co2_Run10ms(false, false);
    Co2_Run10ms(true, true);
    TEST_ASSERT_EQUAL_UINT32(SS_TASK_PERIOD_MS, Co2_GetAccumulatedAutoStopTimeMs());
    float expected = ((float)SS_TASK_PERIOD_MS / 1000.0f) * SS_CO2_WLTP_IDLE_FACTOR;
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, expected, Co2_GetAvoidedGrams());
}

/*
 * UT-CO2-20
 * Requirement ID: SW-LL-FR-25
 * Category: CO2 Estimation
 * Requirement Summary: The software must compute CO2_AVOIDED = TIME_AUTO_STOP x WLTP_IDLE_FACTOR when AutostopActive == TRUE
 * Test Objective: Verify CO2 avoided calculation accuracy after multiple consecutive active cycles
 */
void test_co2_avoided_grams_accumulates_over_multiple_cycles(void) {
    Co2_Run10ms(true, true);
    Co2_Run10ms(true, true);
    Co2_Run10ms(true, true);
    Co2_Run10ms(true, true);
    Co2_Run10ms(true, true);
    float expected = ((float)(5U * SS_TASK_PERIOD_MS) / 1000.0f) * SS_CO2_WLTP_IDLE_FACTOR;
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, expected, Co2_GetAvoidedGrams());
}

/*
 * UT-CO2-21
 * Requirement ID: SW-LL-FR-24 / SW-LL-FR-35
 * Category: CO2 / Fuel Estimation
 * Requirement Summary: The software must reset TIME_AUTO_STOP = 0 and CO2_AVOIDED = 0 when IgnitionOFF == TRUE
 * Test Objective: Verify ignition OFF resets all state even when autostop_active is TRUE (equivalence partition)
 */
void test_co2_ignition_off_with_autostop_true_still_resets(void) {
    Co2_Run10ms(true, true);
    Co2_Run10ms(true, true);
    Co2_Run10ms(false, true);
    TEST_ASSERT_EQUAL_UINT32(0U, Co2_GetAccumulatedAutoStopTimeMs());
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.0f, Co2_GetAvoidedGrams());
    TEST_ASSERT_FALSE(Co2_DisplayEnabled());
    TEST_ASSERT_TRUE(Co2_HideEnabled());
}

/*
 * UT-CO2-22
 * Requirement ID: SW-LL-FR-23 / SW-LL-FR-25
 * Category: CO2 / Fuel Estimation
 * Requirement Summary: The software must increment TIME_AUTO_STOP and compute CO2_AVOIDED correctly over sustained operation
 * Test Objective: Verify accumulated time and CO2 calculation remain accurate after 1000 consecutive active cycles (boundary/stress)
 */
void test_co2_boundary_long_accumulation_1000_cycles(void) {
    for (uint32_t i = 0; i < 1000U; i++) {
        Co2_Run10ms(true, true);
    }
    TEST_ASSERT_EQUAL_UINT32(1000U * SS_TASK_PERIOD_MS, Co2_GetAccumulatedAutoStopTimeMs());
    float expected = ((float)(1000U * SS_TASK_PERIOD_MS) / 1000.0f) * SS_CO2_WLTP_IDLE_FACTOR;
    TEST_ASSERT_FLOAT_WITHIN(0.01f, expected, Co2_GetAvoidedGrams());
}

/* =============================================================================
 *  Main
 * ============================================================================= */

int main(void) {
    UNITY_BEGIN();

    /* Initialization */
    RUN_TEST(test_co2_init_resets_accumulated_time);
    RUN_TEST(test_co2_init_resets_avoided_grams);
    RUN_TEST(test_co2_init_display_disabled);
    RUN_TEST(test_co2_init_hide_enabled);

    /* Ignition OFF */
    RUN_TEST(test_co2_ignition_off_resets_time);
    RUN_TEST(test_co2_ignition_off_resets_avoided_grams);
    RUN_TEST(test_co2_ignition_off_disables_display);
    RUN_TEST(test_co2_ignition_off_enables_hide);

    /* Ignition ON + AutoStop inactive */
    RUN_TEST(test_co2_autostop_inactive_does_not_increment_time);
    RUN_TEST(test_co2_autostop_inactive_disables_display);
    RUN_TEST(test_co2_autostop_inactive_enables_hide);

    /* Ignition ON + AutoStop active */
    RUN_TEST(test_co2_autostop_active_increments_time_by_task_period);
    RUN_TEST(test_co2_autostop_active_increments_time_multiple_calls);
    RUN_TEST(test_co2_autostop_active_calculates_avoided_grams);
    RUN_TEST(test_co2_autostop_active_enables_display);
    RUN_TEST(test_co2_autostop_active_disables_hide);

    /* Negative case */
    RUN_TEST(test_co2_avoided_grams_zero_when_autostop_inactive);

    /* Sequence / Equivalence Partition / Boundary */
    RUN_TEST(test_co2_transition_active_inactive_active_preserves_time);
    RUN_TEST(test_co2_ignition_cycle_resets_and_restarts_clean);
    RUN_TEST(test_co2_avoided_grams_accumulates_over_multiple_cycles);
    RUN_TEST(test_co2_ignition_off_with_autostop_true_still_resets);
    RUN_TEST(test_co2_boundary_long_accumulation_1000_cycles);

    return UNITY_END();
}
