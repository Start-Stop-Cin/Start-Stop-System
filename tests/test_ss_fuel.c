#include "unity.h"
#include "ss_fuel.h"
#include "ss_calib.h"

void setUp(void) { SS_Fuel_Init(); }
void tearDown(void) {}

/* --- Fuel_Init --- */

/*==========================================================
 * UT-FUEL-08
 * Covers: supporting testing
 *==========================================================*/

void test_fuel_init_resets_accumulated_time(void) {
    SS_Fuel_Run10ms(true, true);
    SS_Fuel_Init();
    TEST_ASSERT_EQUAL_UINT32(0U, SS_Fuel_GetAccumulatedAutoStopTimeMs());
}

/*==========================================================
 * UT-FUEL-09
 * Covers: supporting testing
 *==========================================================*/

void test_fuel_init_resets_saved_liters(void) {
    SS_Fuel_Run10ms(true, true);
    SS_Fuel_Init();
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.0f, SS_Fuel_GetSavedLiters());
}

/*==========================================================
 * UT-FUEL-10
 * Covers: supporting testing
 *==========================================================*/

void test_fuel_init_display_disabled(void) {
    SS_Fuel_Run10ms(true, true);
    SS_Fuel_Init();
    TEST_ASSERT_FALSE(SS_Fuel_DisplayEnabled());
}

/*==========================================================
 * UT-FUEL-11
 * Covers: supporting testing
 *==========================================================*/

void test_fuel_init_hide_enabled(void) {
    SS_Fuel_Run10ms(true, true);
    SS_Fuel_Init();
    TEST_ASSERT_TRUE(SS_Fuel_HideEnabled());
}

/*==========================================================
 * UT-FUEL-03
 * Covers: SW-LL-FR-24
 *==========================================================*/

void test_fuel_ignition_off_resets_time(void) {
    SS_Fuel_Run10ms(true, true);
    SS_Fuel_Run10ms(false, false);
    TEST_ASSERT_EQUAL_UINT32(0U, SS_Fuel_GetAccumulatedAutoStopTimeMs());
}

/*==========================================================
 * UT-FUEL-07
 * Covers: SW-LL-FR-36
 *==========================================================*/

void test_fuel_ignition_off_resets_saved_liters(void) {
    SS_Fuel_Run10ms(true, true);
    SS_Fuel_Run10ms(false, false);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.0f, SS_Fuel_GetSavedLiters());
}

/*==========================================================
 * UT-FUEL-13
 * Covers: supporting testing
 *==========================================================*/

void test_fuel_ignition_off_disables_display(void) {
    SS_Fuel_Run10ms(true, true);
    SS_Fuel_Run10ms(false, false);
    TEST_ASSERT_FALSE(SS_Fuel_DisplayEnabled());
}

/*==========================================================
 * UT-FUEL-14
 * Covers: supporting testing
 *==========================================================*/

void test_fuel_ignition_off_enables_hide(void) {
    SS_Fuel_Run10ms(true, true);
    SS_Fuel_Run10ms(false, false);
    TEST_ASSERT_TRUE(SS_Fuel_HideEnabled());
}

/*==========================================================
 * UT-FUEL-15
 * Covers: supporting testing
 *==========================================================*/

void test_fuel_autostop_inactive_does_not_increment_time(void) {
    SS_Fuel_Run10ms(true, false);
    TEST_ASSERT_EQUAL_UINT32(0U, SS_Fuel_GetAccumulatedAutoStopTimeMs());
}

/*==========================================================
 * UT-FUEL-12
 * Covers: supporting testing
 *==========================================================*/

void test_fuel_autostop_inactive_disables_display(void) {
    SS_Fuel_Run10ms(true, false);
    TEST_ASSERT_FALSE(SS_Fuel_DisplayEnabled());
}

/*==========================================================
 * UT-FUEL-06
 * Covers: SW-LL-FR-34
 *==========================================================*/

void test_fuel_autostop_inactive_enables_hide(void) {
    SS_Fuel_Run10ms(true, false);
    TEST_ASSERT_TRUE(SS_Fuel_HideEnabled());
}

/*==========================================================
 * UT-FUEL-01
 * Covers: SW-LL-FR-23
 *==========================================================*/

void test_fuel_autostop_active_increments_time_by_task_period(void) {
    SS_Fuel_Run10ms(true, true);
    TEST_ASSERT_EQUAL_UINT32(SS_TASK_PERIOD_MS, SS_Fuel_GetAccumulatedAutoStopTimeMs());
}

/*==========================================================
 * UT-FUEL-02
 * Covers: SW-LL-FR-23
 *==========================================================*/

void test_fuel_autostop_active_increments_time_multiple_calls(void) {
    SS_Fuel_Run10ms(true, true);
    SS_Fuel_Run10ms(true, true);
    SS_Fuel_Run10ms(true, true);
    TEST_ASSERT_EQUAL_UINT32(3U * SS_TASK_PERIOD_MS, SS_Fuel_GetAccumulatedAutoStopTimeMs());
}

/*==========================================================
 * UT-FUEL-04
 * Covers: SW-LL-FR-32
 *==========================================================*/

void test_fuel_autostop_active_calculates_saved_liters(void) {
    SS_Fuel_Run10ms(true, true);
    float expected = ((float)SS_TASK_PERIOD_MS / 1000.0f) * SS_FUEL_IDLE_RATE_LPS;
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, expected, SS_Fuel_GetSavedLiters());
}

/*==========================================================
 * UT-FUEL-05
 * Covers: SW-LL-FR-33
 *==========================================================*/

void test_fuel_autostop_active_enables_display(void) {
    SS_Fuel_Run10ms(true, true);
    TEST_ASSERT_TRUE(SS_Fuel_DisplayEnabled());
}

/*==========================================================
 * UT-FUEL-17
 * Covers: Supporting testing
 *==========================================================*/

void test_fuel_autostop_active_disables_hide(void) {
    SS_Fuel_Run10ms(true, true);
    TEST_ASSERT_FALSE(SS_Fuel_HideEnabled());
}

/*==========================================================
 * UT-FUEL-16
 * Covers: Supporting testing
 *==========================================================*/

void test_fuel_saved_liters_zero_when_autostop_inactive(void) {
    SS_Fuel_Run10ms(true, false);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.0f, SS_Fuel_GetSavedLiters());
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_fuel_init_resets_accumulated_time);
    RUN_TEST(test_fuel_init_resets_saved_liters);
    RUN_TEST(test_fuel_init_display_disabled);
    RUN_TEST(test_fuel_init_hide_enabled);

    RUN_TEST(test_fuel_ignition_off_resets_time);
    RUN_TEST(test_fuel_ignition_off_resets_saved_liters);
    RUN_TEST(test_fuel_ignition_off_disables_display);
    RUN_TEST(test_fuel_ignition_off_enables_hide);

    RUN_TEST(test_fuel_autostop_inactive_does_not_increment_time);
    RUN_TEST(test_fuel_autostop_inactive_disables_display);
    RUN_TEST(test_fuel_autostop_inactive_enables_hide);

    RUN_TEST(test_fuel_autostop_active_increments_time_by_task_period);
    RUN_TEST(test_fuel_autostop_active_increments_time_multiple_calls);
    RUN_TEST(test_fuel_autostop_active_calculates_saved_liters);
    RUN_TEST(test_fuel_autostop_active_enables_display);
    RUN_TEST(test_fuel_autostop_active_disables_hide);

    RUN_TEST(test_fuel_saved_liters_zero_when_autostop_inactive);

    return UNITY_END();
}
