#include "unity.h"
#include "ss_operation.h"
#include "ss_types.h"

static SsOperationInputs_t in;
static SsOperationOutputs_t out;

// Helper functions to set up specific input conditions for the tests
static void set_default_inputs(void)
{
    in.ignition_on = false;
    in.ss_enable = false;
    in.autostop_allowed = false;
    in.brake_pressed = false;
    in.accelerator_pressed = false;
    in.gear_position = SS_GEAR_P;
    in.vehicle_speed_kmh = 0.0f;
    in.autostop_timeout_reached = false;
}

// Helper functions to set up specific input conditions for the tests
static void set_valid_stop_inputs(void)
{
    in.ignition_on = true;
    in.ss_enable = true;
    in.autostop_allowed = true;
    in.brake_pressed = true;
    in.accelerator_pressed = false;
    in.gear_position = SS_GEAR_D;
    in.vehicle_speed_kmh = 0.0f;
    in.autostop_timeout_reached = false;
}

// Helper functions to set up specific input conditions for the tests - Engine running state
static void go_to_engine_running(void)
{
    SS_Operation_Init();
    set_default_inputs();

    in.ignition_on = true;

    SS_Operation_Run10ms(&in, &out, false);
}

// Helper functions to set up specific input conditions for the tests - Autostop active state
static void go_to_autostop_active(void)
{
    go_to_engine_running();
    set_valid_stop_inputs();

    SS_Operation_Run10ms(&in, &out, false);
}

/*==========================================================
 * Unity Hooks
 *==========================================================*/
void setUp(void)
{
    in = (SsOperationInputs_t){0};
    out = (SsOperationOutputs_t){0};
}

void tearDown(void)
{
}

/*==========================================================
 * UT-OP-01
 * Covers: SW-LL-FR-01
 *==========================================================*/
void test_SS_Operation_requests_engine_stop_when_stop_conditions_are_met(void)
{
    go_to_engine_running();
    set_valid_stop_inputs();

    SS_Operation_Run10ms(&in, &out, false);

    TEST_ASSERT_TRUE(out.engine_stop_request);
    TEST_ASSERT_FALSE(out.engine_restart_request);
    TEST_ASSERT_TRUE(out.autostop_active);
    TEST_ASSERT_EQUAL(SS_OP_STATE_AUTOSTOP_ACTIVE, out.state);
}

/*==========================================================
 * UT-OP-02
 * Covers: SW-LL-FR-07
 *==========================================================*/
void test_SS_Operation_requests_restart_when_accelerator_pressed(void)
{
    go_to_autostop_active();

    in.accelerator_pressed = true;

    SS_Operation_Run10ms(&in, &out, false);

    TEST_ASSERT_FALSE(out.engine_stop_request);
    TEST_ASSERT_TRUE(out.engine_restart_request);
    TEST_ASSERT_FALSE(out.autostop_active);
    TEST_ASSERT_EQUAL(SS_OP_STATE_ENGINE_RUNNING, out.state);
}

/*==========================================================
 * UT-OP-03
 * Covers: SW-LL-FR-08
 *==========================================================*/
void test_SS_Operation_requests_restart_when_brake_released(void)
{
    go_to_autostop_active();

    in.brake_pressed = false;

    SS_Operation_Run10ms(&in, &out, false);

    TEST_ASSERT_TRUE(out.engine_restart_request);
    TEST_ASSERT_FALSE(out.autostop_active);
    TEST_ASSERT_EQUAL(SS_OP_STATE_ENGINE_RUNNING, out.state);
}

/*==========================================================
 * UT-OP-04
 * Covers: SW-LL-FR-09
 *==========================================================*/
void test_SS_Operation_requests_restart_when_gear_changes(void)
{
    go_to_autostop_active();

    in.gear_position = SS_GEAR_N;

    SS_Operation_Run10ms(&in, &out, false);

    TEST_ASSERT_TRUE(out.engine_restart_request);
    TEST_ASSERT_FALSE(out.autostop_active);
    TEST_ASSERT_EQUAL(SS_OP_STATE_ENGINE_RUNNING, out.state);
}

/*==========================================================
 * UT-OP-05
 * Covers: SW-LL-FR-10
 *==========================================================*/
void test_SS_Operation_requests_restart_when_timeout_is_reached(void)
{
    go_to_autostop_active();

    in.autostop_timeout_reached = true;

    SS_Operation_Run10ms(&in, &out, false);

    TEST_ASSERT_TRUE(out.engine_restart_request);
    TEST_ASSERT_FALSE(out.autostop_active);
    TEST_ASSERT_EQUAL(SS_OP_STATE_ENGINE_RUNNING, out.state);
}

/*==========================================================
 * UT-OP-06
 * Covers: SW-LL-FR-31
 *==========================================================*/
void test_SS_Operation_requests_restart_when_ss_is_disabled(void)
{
    go_to_autostop_active();

    in.ss_enable = false;

    SS_Operation_Run10ms(&in, &out, false);

    TEST_ASSERT_TRUE(out.engine_restart_request);
    TEST_ASSERT_FALSE(out.autostop_active);
    TEST_ASSERT_EQUAL(SS_OP_STATE_ENGINE_RUNNING, out.state);
}

/*==========================================================
 * UT-OP-07
 * Supporting test
 *==========================================================*/
void test_SS_Operation_Init_sets_vehicle_off(void)
{
    SS_Operation_Init();
    set_default_inputs();

    SS_Operation_Run10ms(&in, &out, false);

    TEST_ASSERT_EQUAL(SS_OP_STATE_VEHICLE_OFF, out.state);
    TEST_ASSERT_FALSE(out.engine_stop_request);
    TEST_ASSERT_FALSE(out.engine_restart_request);
    TEST_ASSERT_FALSE(out.autostop_active);
}

/*==========================================================
 * UT-OP-08
 * Supporting test
 *==========================================================*/
void test_SS_Operation_goes_to_engine_running_when_ignition_turns_on(void)
{
    SS_Operation_Init();
    set_default_inputs();

    in.ignition_on = true;

    SS_Operation_Run10ms(&in, &out, false);

    TEST_ASSERT_EQUAL(SS_OP_STATE_ENGINE_RUNNING, out.state);
    TEST_ASSERT_FALSE(out.engine_stop_request);
    TEST_ASSERT_FALSE(out.engine_restart_request);
    TEST_ASSERT_FALSE(out.autostop_active);
}

/*==========================================================
 * UT-OP-09
 * Supporting test
 *==========================================================*/
void test_SS_Operation_returns_to_vehicle_off_when_ignition_turns_off(void)
{
    go_to_engine_running();
    set_default_inputs();

    in.ignition_on = false;

    SS_Operation_Run10ms(&in, &out, false);

    TEST_ASSERT_EQUAL(SS_OP_STATE_VEHICLE_OFF, out.state);
    TEST_ASSERT_FALSE(out.engine_stop_request);
    TEST_ASSERT_FALSE(out.engine_restart_request);
    TEST_ASSERT_FALSE(out.autostop_active);
}

/*==========================================================
 * UT-OP-10
 * Supporting test
 *==========================================================*/
void test_SS_Operation_blocks_restart_when_safe_stop_is_active(void)
{
    go_to_autostop_active();

    in.accelerator_pressed = true;

    SS_Operation_Run10ms(&in, &out, true);

    TEST_ASSERT_FALSE(out.engine_restart_request);
    TEST_ASSERT_TRUE(out.autostop_active);
    TEST_ASSERT_EQUAL(SS_OP_STATE_AUTOSTOP_ACTIVE, out.state);
}

/*==========================================================
 * UT-OP-11
 * Supporting test
 *==========================================================*/
void test_SS_Operation_does_not_request_stop_when_conditions_are_not_met(void)
{
    go_to_engine_running();
    set_valid_stop_inputs();

    /* invalida uma condição */
    in.brake_pressed = false;

    SS_Operation_Run10ms(&in, &out, false);

    TEST_ASSERT_FALSE(out.engine_stop_request);
    TEST_ASSERT_FALSE(out.engine_restart_request);
    TEST_ASSERT_FALSE(out.autostop_active);
    TEST_ASSERT_EQUAL(SS_OP_STATE_ENGINE_RUNNING, out.state);
}

/*==========================================================
 * UT-OP-12
 * Supporting test
 *==========================================================*/
void test_SS_Operation_handles_null_pointers(void)
{
    SS_Operation_Init();
    set_default_inputs();

    SS_Operation_Run10ms(NULL, &out, false);
    SS_Operation_Run10ms(&in, NULL, false);

    TEST_PASS();
}

/*==========================================================
 * UT-OP-13
 * Supporting test
 *==========================================================*/

void test_SS_Operation_does_not_stop_when_ignition_is_off(void)
{
    go_to_engine_running();
    set_valid_stop_inputs();

    in.ignition_on = false;

    SS_Operation_Run10ms(&in, &out, false);

    TEST_ASSERT_FALSE(out.engine_stop_request);
    TEST_ASSERT_EQUAL(SS_OP_STATE_VEHICLE_OFF, out.state);
}

/*==========================================================
 * UT-OP-14
 * Supporting test
 *==========================================================*/

void test_SS_Operation_does_not_stop_when_ss_is_disabled_before_autostop(void)
{
    go_to_engine_running();
    set_valid_stop_inputs();

    in.ss_enable = false;

    SS_Operation_Run10ms(&in, &out, false);

    TEST_ASSERT_FALSE(out.engine_stop_request);
    TEST_ASSERT_EQUAL(SS_OP_STATE_ENGINE_RUNNING, out.state);
}

/*==========================================================
 * UT-OP-15
 * Supporting test
 *==========================================================*/

void test_SS_Operation_does_not_stop_when_autostop_is_not_allowed(void)
{
    go_to_engine_running();
    set_valid_stop_inputs();

    in.autostop_allowed = false;

    SS_Operation_Run10ms(&in, &out, false);

    TEST_ASSERT_FALSE(out.engine_stop_request);
    TEST_ASSERT_EQUAL(SS_OP_STATE_ENGINE_RUNNING, out.state);
}

/*==========================================================
 * UT-OP-16
 * Supporting test
 *==========================================================*/

void test_SS_Operation_does_not_stop_when_gear_is_not_drive(void)
{
    go_to_engine_running();
    set_valid_stop_inputs();

    in.gear_position = SS_GEAR_N;

    SS_Operation_Run10ms(&in, &out, false);

    TEST_ASSERT_FALSE(out.engine_stop_request);
    TEST_ASSERT_EQUAL(SS_OP_STATE_ENGINE_RUNNING, out.state);
}

/*==========================================================
 * UT-OP-17
 * Supporting test
 *==========================================================*/

void test_SS_Operation_does_not_stop_when_speed_is_above_limit(void)
{
    go_to_engine_running();
    set_valid_stop_inputs();

    in.vehicle_speed_kmh = 10.0f;

    SS_Operation_Run10ms(&in, &out, false);

    TEST_ASSERT_FALSE(out.engine_stop_request);
    TEST_ASSERT_EQUAL(SS_OP_STATE_ENGINE_RUNNING, out.state);
}

/*==========================================================
 * UT-OP-18
 * Supporting test
 *==========================================================*/

void test_SS_Operation_remains_in_autostop_when_no_restart_condition_is_met(void)
{
    go_to_autostop_active();

    in.accelerator_pressed = false;
    in.brake_pressed = true;
    in.gear_position = SS_GEAR_D;
    in.ss_enable = true;
    in.autostop_timeout_reached = false;

    SS_Operation_Run10ms(&in, &out, false);

    TEST_ASSERT_FALSE(out.engine_restart_request);
    TEST_ASSERT_TRUE(out.autostop_active);
    TEST_ASSERT_EQUAL(SS_OP_STATE_AUTOSTOP_ACTIVE, out.state);
}

/*==========================================================
 * UT-OP-19
 * Supporting test
 *==========================================================*/

void test_SS_Operation_remains_in_autostop_when_restart_condition_is_met_but_safe_stop_is_true(void)
{
    go_to_autostop_active();

    in.brake_pressed = false;

    SS_Operation_Run10ms(&in, &out, true);

    TEST_ASSERT_FALSE(out.engine_restart_request);
    TEST_ASSERT_TRUE(out.autostop_active);
    TEST_ASSERT_EQUAL(SS_OP_STATE_AUTOSTOP_ACTIVE, out.state);
}

/*==========================================================
 * UT-OP-20
 * Supporting test
 *==========================================================*/

void test_SS_Operation_goes_to_vehicle_off_when_ignition_turns_off_during_autostop(void)
{
    go_to_autostop_active();

    in.ignition_on = false;

    SS_Operation_Run10ms(&in, &out, false);

    TEST_ASSERT_FALSE(out.engine_restart_request);
    TEST_ASSERT_FALSE(out.autostop_active);
    TEST_ASSERT_EQUAL(SS_OP_STATE_VEHICLE_OFF, out.state);
}

/*==========================================================
 * UT-OP-21
 * Supporting test
 *==========================================================*/

void test_SS_Operation_GetState_returns_current_state(void)
{
    go_to_engine_running();

    TEST_ASSERT_EQUAL(SS_OP_STATE_ENGINE_RUNNING, SS_Operation_GetState());
}

/*==========================================================
 * UT-OP-22
 * Supporting test
 *==========================================================*/

void test_SS_Operation_IsAutoStopActiveState_returns_true_when_in_autostop(void)
{
    go_to_autostop_active();

    TEST_ASSERT_TRUE(SS_Operation_IsAutoStopActiveState());
}

/*==========================================================
 * UT-OP-23
 * Supporting test
 *==========================================================*/

void test_SS_Operation_IsAutoStopActiveState_returns_false_when_not_in_autostop(void)
{
    go_to_engine_running();

    TEST_ASSERT_FALSE(SS_Operation_IsAutoStopActiveState());
}

/*==========================================================
 * UT-OP-24
 * Supporting test
 *==========================================================*/

int main(void)
{
    UNITY_BEGIN();
    
    RUN_TEST(test_SS_Operation_requests_engine_stop_when_stop_conditions_are_met);
    RUN_TEST(test_SS_Operation_requests_restart_when_accelerator_pressed);
    RUN_TEST(test_SS_Operation_requests_restart_when_brake_released);
    RUN_TEST(test_SS_Operation_requests_restart_when_gear_changes);
    RUN_TEST(test_SS_Operation_requests_restart_when_timeout_is_reached);
    RUN_TEST(test_SS_Operation_requests_restart_when_ss_is_disabled);
    RUN_TEST(test_SS_Operation_Init_sets_vehicle_off);
    RUN_TEST(test_SS_Operation_goes_to_engine_running_when_ignition_turns_on);
    RUN_TEST(test_SS_Operation_returns_to_vehicle_off_when_ignition_turns_off);
    RUN_TEST(test_SS_Operation_blocks_restart_when_safe_stop_is_active);
    RUN_TEST(test_SS_Operation_does_not_request_stop_when_conditions_are_not_met);
    RUN_TEST(test_SS_Operation_handles_null_pointers);
	RUN_TEST(test_SS_Operation_does_not_stop_when_ignition_is_off);
	RUN_TEST(test_SS_Operation_does_not_stop_when_ss_is_disabled_before_autostop);
	RUN_TEST(test_SS_Operation_does_not_stop_when_autostop_is_not_allowed);
	RUN_TEST(test_SS_Operation_does_not_stop_when_gear_is_not_drive);
	RUN_TEST(test_SS_Operation_does_not_stop_when_speed_is_above_limit);
	RUN_TEST(test_SS_Operation_remains_in_autostop_when_no_restart_condition_is_met);
	RUN_TEST(test_SS_Operation_remains_in_autostop_when_restart_condition_is_met_but_safe_stop_is_true);
	RUN_TEST(test_SS_Operation_goes_to_vehicle_off_when_ignition_turns_off_during_autostop);
	RUN_TEST(test_SS_Operation_GetState_returns_current_state);
	RUN_TEST(test_SS_Operation_IsAutoStopActiveState_returns_true_when_in_autostop);
	RUN_TEST(test_SS_Operation_IsAutoStopActiveState_returns_false_when_not_in_autostop);
	
    return UNITY_END();
}
