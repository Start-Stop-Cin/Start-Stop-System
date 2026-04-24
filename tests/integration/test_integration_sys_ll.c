#include "unity.h"

#include "ss_operation.h"
#include "ss_timer.h"
#include "ss_inhibit_safety_conditions.h"
#include "ss_control.h"
#include "co2.h"
#include "ss_fuel.h"
#include "ss_types.h"
#include "ss_calib.h"

#include <stdbool.h>
#include <stdint.h>

/* =========================================================
 * Integration test outputs
 * ========================================================= */
typedef struct {
    bool ss_enable;
    bool autostop_allowed;
    bool safe_stop;
    bool autostop_active;
    bool engine_stop_request;
    bool engine_restart_request;
    SsOperationState_t state;

    unsigned int autostop_event_time_ms;
    bool timeout_reached;

    unsigned int co2_time_ms;
    float co2_avoided_g;
    bool display_co2;
    bool hide_co2;

    unsigned int fuel_time_ms;
    float fuel_saved_l;
    bool display_fuel;
    bool hide_fuel;
} IntegrationOutputs_t;

static bool g_prev_engine_stop_request_it = false;

/* =========================================================
 * Reset all integrated modules
 * ========================================================= */
static void Integration_Init(void)
{
    SS_Init();
    SS_Operation_Init();
    SS_Timer_Init();
    Co2_Init();
    SS_Fuel_Init();

    g_prev_engine_stop_request_it = false;
}

/* =========================================================
 * Integration runner using already processed signals
 *
 * This mimics the integration flow used in SIL, but without
 * treating ss_app.c as a functional module.
 * ========================================================= */
static void Integration_Run10ms(
    bool ignition_on,
    bool brake_pressed,
    bool accelerator_pressed,
    SsGearPosition_t gear_position,
    float vehicle_speed_kmh,
    bool door_closed,
    bool seatbelt_fastened,
    bool button_input,
    float inclination_angle_rad,
    IntegrationOutputs_t *out)
{
    SsOperationInputs_t op_in = {0};
    SsOperationOutputs_t op_out = {0};

    bool safety_ok = false;
    bool drivecycle_ok = false;
    bool autostop_allowed_local = false;
    bool safe_stop_local = false;

    /* Start/Stop enable control */
    g_SS_Inputs.ButtonInput = button_input;
    g_SS_Inputs.IgnitionStatus = ignition_on;
    SS_Step();

    bool ss_enable_local = g_SS_Outputs.SS_Enabled_Final;

    /* Inhibit + drive cycle memory */
    safety_ok = inhibit_safety_conditions(
        inclination_angle_rad,
        door_closed,
        seatbelt_fastened,
        gear_position,
        ss_enable_local);

    drivecycle_ok = drivecycle_memory_block(
        ignition_on,
        g_prev_engine_stop_request_it,
        vehicle_speed_kmh);

    autostop_allowed_local = safety_ok && drivecycle_ok;

    /* SafeStop is generated before operation consumes it */
    safe_stop_local = standstill_management(
        SS_Operation_IsAutoStopActiveState(),
        brake_pressed,
        door_closed,
        seatbelt_fastened);

    /* Operation inputs */
    op_in.ignition_on = ignition_on;
    op_in.ss_enable = ss_enable_local;
    op_in.autostop_allowed = autostop_allowed_local;
    op_in.brake_pressed = brake_pressed;
    op_in.accelerator_pressed = accelerator_pressed;
    op_in.gear_position = gear_position;
    op_in.vehicle_speed_kmh = vehicle_speed_kmh;
    op_in.autostop_timeout_reached = SS_Timer_IsAutostopTimeoutReached();

    SS_Operation_Run10ms(&op_in, &op_out, safe_stop_local);

    /* Consumers of AutoStop state */
    SS_Timer_Run10ms(op_out.autostop_active);
    Co2_Run10ms(op_in.ignition_on, op_out.autostop_active);
    SS_Fuel_Run10ms(op_in.ignition_on, op_out.autostop_active);

    g_prev_engine_stop_request_it = op_out.engine_stop_request;

    if (out != NULL) {
        out->ss_enable = ss_enable_local;
        out->autostop_allowed = autostop_allowed_local;
        out->safe_stop = safe_stop_local;
        out->autostop_active = op_out.autostop_active;
        out->engine_stop_request = op_out.engine_stop_request;
        out->engine_restart_request = op_out.engine_restart_request;
        out->state = op_out.state;

        out->autostop_event_time_ms = SS_Timer_GetAutostopEventTimeMs();
        out->timeout_reached = SS_Timer_IsAutostopTimeoutReached();

        out->co2_time_ms = Co2_GetAccumulatedAutoStopTimeMs();
        out->co2_avoided_g = Co2_GetAvoidedGrams();
        out->display_co2 = Co2_DisplayEnabled();
        out->hide_co2 = Co2_HideEnabled();

        out->fuel_time_ms = SS_Fuel_GetAccumulatedAutoStopTimeMs();
        out->fuel_saved_l = SS_Fuel_GetSavedLiters();
        out->display_fuel = SS_Fuel_DisplayEnabled();
        out->hide_fuel = SS_Fuel_HideEnabled();
    }
}

static void Integration_SatisfyDriveCycleMemory(void)
{
    IntegrationOutputs_t out;

    Integration_Run10ms(
        true,
        false,
        false,
        SS_GEAR_D,
        10.0f,
        true,
        true,
        false,
        0.0f,
        &out);
}

static void Integration_GoToAutoStop(IntegrationOutputs_t *out)
{
    Integration_SatisfyDriveCycleMemory();

    Integration_Run10ms(
        true,
        true,
        false,
        SS_GEAR_D,
        0.0f,
        true,
        true,
        false,
        0.0f,
        out);
}

void setUp(void)
{
    Integration_Init();
}

void tearDown(void)
{
}

/* =========================================================
 * IT-SYS-LL-01
 * Requirements: SYS-LL-FR-01, SYS-LL-FR-07, SYS-LL-FR-09,
 *               SYS-LL-FR-11, SYS-LL-FR-13
 * Modules: ss_inhibit_safety_conditions + ss_operation
 *          + ss_timer + ss_co2 + ss_fuel
 * ========================================================= */
void test_integration_autostop_enables_estimation_modules(void)
{
    IntegrationOutputs_t out;

    Integration_GoToAutoStop(&out);

    TEST_ASSERT_TRUE(out.autostop_allowed);
    TEST_ASSERT_TRUE(out.engine_stop_request);
    TEST_ASSERT_TRUE(out.autostop_active);
    TEST_ASSERT_EQUAL(SS_OP_STATE_AUTOSTOP_ACTIVE, out.state);

    TEST_ASSERT_GREATER_THAN_UINT32(0U, out.autostop_event_time_ms);

    TEST_ASSERT_GREATER_THAN_UINT32(0U, out.co2_time_ms);
    TEST_ASSERT_TRUE(out.co2_avoided_g > 0.0f);
    TEST_ASSERT_TRUE(out.display_co2);
    TEST_ASSERT_FALSE(out.hide_co2);

    TEST_ASSERT_GREATER_THAN_UINT32(0U, out.fuel_time_ms);
    TEST_ASSERT_TRUE(out.fuel_saved_l > 0.0f);
    TEST_ASSERT_TRUE(out.display_fuel);
    TEST_ASSERT_FALSE(out.hide_fuel);
}

/* =========================================================
 * IT-SYS-LL-02
 * Requirements: SYS-LL-FR-02, SYS-LL-FR-08, SYS-LL-FR-12
 * Modules: ss_operation + ss_co2 + ss_fuel
 * ========================================================= */
void test_integration_restart_disables_autostop_estimations(void)
{
    IntegrationOutputs_t out;

    Integration_GoToAutoStop(&out);

    Integration_Run10ms(
        true,
        true,
        true,
        SS_GEAR_D,
        0.0f,
        true,
        true,
        false,
        0.0f,
        &out);

    TEST_ASSERT_TRUE(out.engine_restart_request);
    TEST_ASSERT_FALSE(out.autostop_active);
    TEST_ASSERT_EQUAL(SS_OP_STATE_ENGINE_RUNNING, out.state);

    TEST_ASSERT_FALSE(out.display_co2);
    TEST_ASSERT_TRUE(out.hide_co2);

    TEST_ASSERT_FALSE(out.display_fuel);
    TEST_ASSERT_TRUE(out.hide_fuel);
}

/* =========================================================
 * IT-SYS-LL-03
 * Requirements: SYS-LL-FR-10, SYS-LL-FR-14
 * Modules: ss_operation + ss_timer + ss_co2 + ss_fuel
 * ========================================================= */
void test_integration_ignition_off_resets_estimation_outputs(void)
{
    IntegrationOutputs_t out;

    Integration_GoToAutoStop(&out);

    TEST_ASSERT_TRUE(out.co2_avoided_g > 0.0f);
    TEST_ASSERT_TRUE(out.fuel_saved_l > 0.0f);

    Integration_Run10ms(
        false,
        false,
        false,
        SS_GEAR_P,
        0.0f,
        true,
        true,
        false,
        0.0f,
        &out);

    TEST_ASSERT_EQUAL(SS_OP_STATE_VEHICLE_OFF, out.state);
    TEST_ASSERT_FALSE(out.autostop_active);

    TEST_ASSERT_EQUAL_UINT32(0U, out.co2_time_ms);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.0f, out.co2_avoided_g);

    TEST_ASSERT_EQUAL_UINT32(0U, out.fuel_time_ms);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.0f, out.fuel_saved_l);
}

/* =========================================================
 * IT-SYS-LL-04
 * Requirement: Derived SW-LL SafeStop
 * Modules: ss_inhibit_safety_conditions + ss_operation
 * ========================================================= */
void test_integration_safe_stop_blocks_restart(void)
{
    IntegrationOutputs_t out;

    Integration_GoToAutoStop(&out);

    /* Cycle 1: create SafeStop condition */
    Integration_Run10ms(
        true,
        true,
        false,
        SS_GEAR_D,
        0.0f,
        false, /* door opened */
        true,
        false,
        0.0f,
        &out);

    /* Cycle 2: SafeStop should now be active, apply restart trigger */
    Integration_Run10ms(
        true,
        true,
        true,  /* accelerator pressed */
        SS_GEAR_D,
        0.0f,
        false, /* door still opened */
        true,
        false,
        0.0f,
        &out);

    TEST_ASSERT_TRUE(out.safe_stop);
    TEST_ASSERT_FALSE(out.engine_restart_request);
    TEST_ASSERT_TRUE(out.autostop_active);
    TEST_ASSERT_EQUAL(SS_OP_STATE_AUTOSTOP_ACTIVE, out.state);
}

/* =========================================================
 * IT-SYS-LL-05
 * Requirements: Derived SW-LL Drive Cycle Memory, SYS-LL-FR-01
 * Modules: ss_inhibit_safety_conditions + ss_operation
 * ========================================================= */
void test_integration_drive_cycle_memory_controls_autostop_permission(void)
{
    IntegrationOutputs_t out;

    /* Attempt AutoStop before drive cycle memory is satisfied */
    Integration_Run10ms(
        true,
        true,
        false,
        SS_GEAR_D,
        0.0f,
        true,
        true,
        false,
        0.0f,
        &out);

    TEST_ASSERT_FALSE(out.autostop_allowed);
    TEST_ASSERT_FALSE(out.engine_stop_request);
    TEST_ASSERT_FALSE(out.autostop_active);

    /* Satisfy drive cycle memory */
    Integration_Run10ms(
        true,
        false,
        false,
        SS_GEAR_D,
        10.0f,
        true,
        true,
        false,
        0.0f,
        &out);

    /* Return to standstill and request AutoStop */
    Integration_Run10ms(
        true,
        true,
        false,
        SS_GEAR_D,
        0.0f,
        true,
        true,
        false,
        0.0f,
        &out);

    TEST_ASSERT_TRUE(out.autostop_allowed);
    TEST_ASSERT_TRUE(out.engine_stop_request);
    TEST_ASSERT_TRUE(out.autostop_active);
}

/* =========================================================
 * IT-SYS-LL-06
 * Requirements: SYS-LL-FR-03, SYS-LL-FR-04, SYS-LL-FR-05
 * Modules: ss_control + ss_operation
 * ========================================================= */
void test_integration_ss_enable_toggle_affects_operation_behavior(void)
{
    IntegrationOutputs_t out;

    /* Debounced button press: keep button pressed for enough cycles */
    for (uint8_t i = 0U; i < 6U; i++) {
        Integration_Run10ms(
            true,
            false,
            false,
            SS_GEAR_D,
            10.0f,
            true,
            true,
            true,
            0.0f,
            &out);
    }

    /* Release button */
    Integration_Run10ms(
        true,
        false,
        false,
        SS_GEAR_D,
        10.0f,
        true,
        true,
        false,
        0.0f,
        &out);

    TEST_ASSERT_FALSE(out.ss_enable);

    /* Try AutoStop with SS disabled */
    Integration_Run10ms(
        true,
        true,
        false,
        SS_GEAR_D,
        0.0f,
        true,
        true,
        false,
        0.0f,
        &out);

    TEST_ASSERT_FALSE(out.autostop_allowed);
    TEST_ASSERT_FALSE(out.engine_stop_request);
    TEST_ASSERT_FALSE(out.autostop_active);
}

/* =========================================================
 * IT-SYS-LL-07
 * Requirement: SYS-LL-FR-06
 * Modules: ss_inhibit_safety_conditions + ss_operation
 * ========================================================= */
void test_integration_inhibit_conditions_block_autostop(void)
{
    IntegrationOutputs_t out;

    Integration_SatisfyDriveCycleMemory();

    Integration_Run10ms(
        true,
        true,
        false,
        SS_GEAR_D,
        0.0f,
        true,
        true,
        false,
        0.20f, /* inclination above threshold */
        &out);

    TEST_ASSERT_FALSE(out.autostop_allowed);
    TEST_ASSERT_FALSE(out.engine_stop_request);
    TEST_ASSERT_FALSE(out.autostop_active);
}

/* =========================================================
 * Main
 * ========================================================= */
int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_integration_autostop_enables_estimation_modules);
    RUN_TEST(test_integration_restart_disables_autostop_estimations);
    RUN_TEST(test_integration_ignition_off_resets_estimation_outputs);
    RUN_TEST(test_integration_safe_stop_blocks_restart);
    RUN_TEST(test_integration_drive_cycle_memory_controls_autostop_permission);
    RUN_TEST(test_integration_ss_enable_toggle_affects_operation_behavior);
    RUN_TEST(test_integration_inhibit_conditions_block_autostop);

    return UNITY_END();
}
