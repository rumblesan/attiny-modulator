#include <stdio.h>

#include "minunit.h"

#include "edge_detector.h"
#include "discrete_control.h"
#include "util.h"
#include "envelope.h"

#define test_header(text) \
    printf("\n\n"); \
    printf("***********\n"); \
    printf(text);

MU_TEST(edge_detector_tests) {
    EdgeDetector ed;
    edge_detector_init(ed, 0);
    mu_assert(!edge_detector_is_rising(ed), "edge detector isn't rising");
    mu_assert(!edge_detector_is_falling(ed), "edge detector isn't falling");

    edge_detector_update(ed, 1);
    mu_assert(edge_detector_is_rising(ed), "edge detector is rising");
    mu_assert(!edge_detector_is_falling(ed), "edge detector isn't falling");

    edge_detector_update(ed, 1);
    mu_assert(!edge_detector_is_rising(ed), "edge detector isn't rising");
    mu_assert(!edge_detector_is_falling(ed), "edge detector isn't falling");

    edge_detector_update(ed, 0);
    mu_assert(!edge_detector_is_rising(ed), "edge detector isn't rising");
    mu_assert(edge_detector_is_falling(ed), "edge detector is falling");

    edge_detector_update(ed, 0);
    mu_assert(!edge_detector_is_rising(ed), "edge detector isn't rising");
    mu_assert(!edge_detector_is_falling(ed), "edge detector isn't falling");
}

MU_TEST_SUITE(edge_detector_suite) {
    test_header("Edge Detector tests\n");
    MU_RUN_TEST(edge_detector_tests);
}

MU_TEST(discrete_control_basics) {
    DiscreteControl dc;
    discrete_control_init(&dc, 8, 80, 2);

    mu_assert_int_eq(0, discrete_control_get_value(&dc));

    discrete_control_update(&dc, 14);
    mu_assert_int_eq(1, discrete_control_get_value(&dc));

    discrete_control_update(&dc, 21);
    mu_assert_int_eq(1, discrete_control_get_value(&dc));

    discrete_control_update(&dc, 24);
    mu_assert_int_eq(2, discrete_control_get_value(&dc));

    discrete_control_update(&dc, 99);
    mu_assert_int_eq(9, discrete_control_get_value(&dc));
}

MU_TEST_SUITE(discrete_control_suite) {
    test_header("Discrete Control tests\n");
    MU_RUN_TEST(discrete_control_basics);
}

MU_TEST(util_map_tests) {
    mu_assert_int_eq(100, map(10, 0, 100, 0, 1000));
    mu_assert_int_eq(200, map(10, 0, 50, 0, 1000));
    uint16_t v = 900;
    uint16_t mmax = 1023;
    mu_assert_int_eq(14760000, map(v, 0, mmax, 0, 16777215));
}

MU_TEST_SUITE(util_suite) {
    test_header("Util tests\n");
    MU_RUN_TEST(util_map_tests);
}

MU_TEST(envelope_tests_basic) {
    Envelope env;
    envelope_init(&env);
    envelope_set_attack(&env, ENV_MAX_VALUE / 4);
    envelope_set_decay(&env, ENV_MAX_VALUE / 4);

    mu_assert(env.state == ENVELOPE_STOPPED, "envelope should be stopped");

    envelope_start(&env);
    mu_assert(env.state == ENVELOPE_RISING, "envelope should be rising");
    envelope_tick(&env);
    mu_assert(env.state == ENVELOPE_RISING, "envelope should be rising");
    mu_assert_int_eq(ENV_MAX_VALUE / 4, env.value);
    for (int i = 0; i < 3; i++) {
        envelope_tick(&env);
        mu_assert(env.state == ENVELOPE_RISING, "envelope should be rising");
    }
    envelope_tick(&env);
    mu_assert(env.state == ENVELOPE_HOLDING, "envelope should be holding");
    mu_assert_int_eq(ENV_MAX_VALUE, env.value);
    mu_assert_int_eq(255, envelope_8bit_value(&env));

    for (int i = 0; i < 3; i++) {
        envelope_tick(&env);
        mu_assert(env.state == ENVELOPE_HOLDING, "envelope should be holding");
    }

    envelope_release(&env);

    envelope_tick(&env);
    mu_assert(env.state == ENVELOPE_FALLING, "envelope should be falling");
    mu_assert_int_eq(ENV_MAX_VALUE - (ENV_MAX_VALUE / 4), env.value);
    mu_assert_int_eq(192, envelope_8bit_value(&env));

    envelope_tick(&env);
    mu_assert_int_eq(128, envelope_8bit_value(&env));
    envelope_tick(&env);
    mu_assert_int_eq(64, envelope_8bit_value(&env));

    envelope_tick(&env);
    mu_assert_int_eq(0, envelope_8bit_value(&env));

    envelope_tick(&env);
    mu_assert_int_eq(0, envelope_8bit_value(&env));

    mu_assert(env.state == ENVELOPE_STOPPED, "envelope should be stopped");
}

MU_TEST_SUITE(envelope_suite) {
    test_header("Envelope tests\n");
    MU_RUN_TEST(envelope_tests_basic);
}

int main(int argc, char *argv[]) {
    test_header("ATTiny Modulator tests\n");
    MU_RUN_SUITE(edge_detector_suite);
    MU_RUN_SUITE(discrete_control_suite);
    MU_RUN_SUITE(util_suite);
    MU_RUN_SUITE(envelope_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}
