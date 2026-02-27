/*
 *  test_dsp_graph.c -- Unit tests for dsp_graph
 *
 *  Standalone test harness. Compile and run directly:
 *    cc -std=c11 -Wall -Wextra -o test_dsp_graph test_dsp_graph.c dsp_graph.c -lm
 *    ./test_dsp_graph
 */

#include "dsp_graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) \
    static void test_##name(void); \
    static void run_test_##name(void) { \
        tests_run++; \
        printf("  %-50s ", #name); \
        test_##name(); \
    } \
    static void test_##name(void)

#define ASSERT(cond) do { \
    if (!(cond)) { \
        printf("FAIL\n    %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        tests_failed++; \
        return; \
    } \
} while(0)

#define ASSERT_NEAR(a, b, eps) do { \
    double _a = (a), _b = (b), _e = (eps); \
    if (fabs(_a - _b) > _e) { \
        printf("FAIL\n    %s:%d: %g != %g (eps=%g)\n", __FILE__, __LINE__, _a, _b, _e); \
        tests_failed++; \
        return; \
    } \
} while(0)

#define PASS() do { printf("ok\n"); tests_passed++; } while(0)

#define SR 44100.0
#define VS 64

/* helper: compile, process one block, return graph (caller frees) */
static dsp_graph* compile_and_process(const char* expr, double** outs, long numouts)
{
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile(expr, SR, VS, NULL, err, DSP_ERR_BUF);
    if (!g) {
        printf("FAIL\n    compile error: %s\n", err);
        return NULL;
    }
    dsp_graph_process(g, NULL, 0, outs, numouts, VS);
    return g;
}

/* ---- compilation tests ---- */

TEST(compile_const) {
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("42", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    ASSERT(g->num_roots == 1);
    ASSERT(g->node_count == 1);
    dsp_graph_free(g);
    PASS();
}

TEST(compile_empty_fails) {
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g == NULL);
    PASS();
}

TEST(compile_unknown_token_fails) {
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("440 fizzbuzz", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g == NULL);
    ASSERT(strstr(err, "unknown token") != NULL);
    PASS();
}

TEST(compile_stack_underflow) {
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("+", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g == NULL);
    ASSERT(strstr(err, "underflow") != NULL);
    PASS();
}

/* ---- basic math processing ---- */

TEST(process_const) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("42", outs, 1);
    ASSERT(g != NULL);
    for (long i = 0; i < VS; i++)
        ASSERT_NEAR(out_buf[i], 42.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(process_add) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("10 20 +", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 30.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(process_sub) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("50 20 -", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 30.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(process_mul) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("6 7 *", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 42.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(process_div) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("84 2 /", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 42.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(process_div_by_zero) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("42 0 /", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 0.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(process_neg_literal) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("-5", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], -5.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(process_pow) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("2 10 pow", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 1024.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(process_mod) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("17 5 mod", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 2.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(process_min_max) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("3 7 min", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 3.0, 1e-10);
    dsp_graph_free(g);

    g = compile_and_process("3 7 max", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 7.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(process_clip) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("5 0 3 clip", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 3.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(process_mix) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("0 10 0.5 mix", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 5.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

/* ---- unary math ---- */

TEST(process_abs) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("-7 abs", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 7.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(process_neg) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("5 neg", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], -5.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(process_floor_ceil_round) {
    double out_buf[VS];
    double* outs[] = { out_buf };

    dsp_graph* g = compile_and_process("3.7 floor", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 3.0, 1e-10);
    dsp_graph_free(g);

    g = compile_and_process("3.2 ceil", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 4.0, 1e-10);
    dsp_graph_free(g);

    g = compile_and_process("3.5 round", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 4.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(process_sign) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("-3 sign", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], -1.0, 1e-10);
    dsp_graph_free(g);

    g = compile_and_process("5 sign", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 1.0, 1e-10);
    dsp_graph_free(g);

    g = compile_and_process("0 sign", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 0.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(process_trig) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("0 sin", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 0.0, 1e-10);
    dsp_graph_free(g);

    g = compile_and_process("0 cos", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 1.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(process_wrap) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("2.7 wrap", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 0.7, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(process_mtof_ftom) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("69 mtof", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 440.0, 1e-6);
    dsp_graph_free(g);

    g = compile_and_process("440 ftom", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 69.0, 1e-6);
    dsp_graph_free(g);
    PASS();
}

TEST(process_db2a_a2db) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("0 db2a", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 1.0, 1e-10);
    dsp_graph_free(g);

    g = compile_and_process("1 a2db", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 0.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

/* ---- stack manipulation ---- */

TEST(stack_dup) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("5 dup +", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 10.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(stack_swap) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("10 3 swap -", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], -7.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(stack_pop) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("42 99 pop", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 42.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(stack_over) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("3 4 over + +", outs, 1);
    ASSERT(g != NULL);
    /* over: 3 4 -> 3 4 3, then 3+4=7, 7+3=10 */
    ASSERT_NEAR(out_buf[0], 10.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(stack_rot) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    /* 1 2 3 rot -> 2 3 1, then 3-1=2, 2-2=0 */
    dsp_graph* g = compile_and_process("1 2 3 rot - swap pop", outs, 1);
    ASSERT(g != NULL);
    /* after rot: 2 3 1. sub: 2 (3-1=2). then swap pop leaves 2 */
    ASSERT_NEAR(out_buf[0], 2.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

/* ---- generators ---- */

TEST(process_sinosc) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("440 sinosc", outs, 1);
    ASSERT(g != NULL);
    /* first sample: sin(0) = 0 */
    ASSERT_NEAR(out_buf[0], 0.0, 1e-10);
    /* output should be bounded [-1,1] */
    for (long i = 0; i < VS; i++) {
        ASSERT(out_buf[i] >= -1.0 && out_buf[i] <= 1.0);
    }
    dsp_graph_free(g);
    PASS();
}

TEST(process_phasor) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("100 phasor", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 0.0, 1e-10);
    /* should be monotonically increasing for these few samples */
    for (long i = 1; i < VS; i++) {
        ASSERT(out_buf[i] > out_buf[i-1]);
    }
    dsp_graph_free(g);
    PASS();
}

TEST(process_noise) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("noise", outs, 1);
    ASSERT(g != NULL);
    /* should be bounded [-1,1] and not all the same */
    int all_same = 1;
    for (long i = 0; i < VS; i++) {
        ASSERT(out_buf[i] >= -1.0 && out_buf[i] <= 1.0);
        if (i > 0 && out_buf[i] != out_buf[0]) all_same = 0;
    }
    ASSERT(!all_same);
    dsp_graph_free(g);
    PASS();
}

TEST(process_pink) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("pink", outs, 1);
    ASSERT(g != NULL);
    /* should produce varying output bounded roughly [-1,1] */
    int all_same = 1;
    for (long i = 0; i < VS; i++) {
        ASSERT(out_buf[i] >= -2.0 && out_buf[i] <= 2.0);
        if (i > 0 && out_buf[i] != out_buf[0]) all_same = 0;
    }
    ASSERT(!all_same);
    dsp_graph_free(g);
    PASS();
}

TEST(process_dust) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    /* high density should produce some impulses */
    dsp_graph* g = compile_and_process("10000 dust", outs, 1);
    ASSERT(g != NULL);
    int has_impulse = 0;
    for (long i = 0; i < VS; i++) {
        ASSERT(out_buf[i] == 0.0 || out_buf[i] == 1.0);
        if (out_buf[i] == 1.0) has_impulse = 1;
    }
    ASSERT(has_impulse);
    dsp_graph_free(g);
    PASS();
}

TEST(process_tri) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("440 tri", outs, 1);
    ASSERT(g != NULL);
    /* first sample: 4*|0 - 0.5| - 1 = 4*0.5 - 1 = 1.0 */
    ASSERT_NEAR(out_buf[0], 1.0, 1e-10);
    for (long i = 0; i < VS; i++) {
        ASSERT(out_buf[i] >= -1.001 && out_buf[i] <= 1.001);
    }
    dsp_graph_free(g);
    PASS();
}

TEST(process_saw) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("440 saw", outs, 1);
    ASSERT(g != NULL);
    /* first sample: 2*0 - 1 = -1.0 */
    ASSERT_NEAR(out_buf[0], -1.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(process_pulse) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("440 0.5 pulse", outs, 1);
    ASSERT(g != NULL);
    /* each sample should be +1 or -1 */
    for (long i = 0; i < VS; i++) {
        ASSERT(out_buf[i] == 1.0 || out_buf[i] == -1.0);
    }
    dsp_graph_free(g);
    PASS();
}

/* ---- decay envelope ---- */

TEST(process_decay) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    /* trigger=1, time=0.01s -> decaying envelope */
    dsp_graph* g = compile_and_process("1 0.01 decay", outs, 1);
    ASSERT(g != NULL);
    /* first sample should be 1.0 (trigger fires) */
    ASSERT_NEAR(out_buf[0], 1.0, 1e-10);
    /* should be decreasing */
    for (long i = 1; i < VS; i++) {
        ASSERT(out_buf[i] < out_buf[i-1]);
        ASSERT(out_buf[i] >= 0.0);
    }
    dsp_graph_free(g);
    PASS();
}

/* ---- filters ---- */

TEST(process_onepole) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    /* DC input through lowpass should converge to input value */
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("1 1000 onepole", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    /* process many blocks to let filter settle */
    for (int block = 0; block < 100; block++) {
        for (int i = 0; i < g->node_count; i++)
            g->nodes[i].visited = 0;
        dsp_graph_process(g, NULL, 0, outs, 1, VS);
    }
    ASSERT_NEAR(out_buf[VS-1], 1.0, 0.01);
    dsp_graph_free(g);
    PASS();
}

TEST(process_lag) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("1 0.001 lag", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    for (int block = 0; block < 100; block++) {
        for (int i = 0; i < g->node_count; i++)
            g->nodes[i].visited = 0;
        dsp_graph_process(g, NULL, 0, outs, 1, VS);
    }
    ASSERT_NEAR(out_buf[VS-1], 1.0, 0.01);
    dsp_graph_free(g);
    PASS();
}

TEST(process_slew) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    /* slew with very fast rise/fall should track closely */
    dsp_graph* g = compile_and_process("1 0.0001 0.0001 slew", outs, 1);
    ASSERT(g != NULL);
    /* should approach 1.0 within the block */
    ASSERT(out_buf[VS-1] > 0.5);
    dsp_graph_free(g);
    PASS();
}

TEST(process_biquad) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    /* unity pass-through biquad: b0=1, b1=0, b2=0, a1=0, a2=0 */
    dsp_graph* g = compile_and_process("42 1 0 0 0 0 biquad", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 42.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

/* ---- delay ---- */

TEST(process_delay) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    /* signal=1, delay=0 -> output=1 immediately */
    dsp_graph* g = compile_and_process("1 0 delay", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 1.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

/* ---- input routing ---- */

TEST(process_input) {
    double in_buf[VS];
    double out_buf[VS];
    for (long i = 0; i < VS; i++) in_buf[i] = 3.14;
    double* ins[] = { in_buf };
    double* outs[] = { out_buf };

    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("in1 2 *", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    dsp_graph_process(g, ins, 1, outs, 1, VS);
    ASSERT_NEAR(out_buf[0], 6.28, 1e-10);
    dsp_graph_free(g);
    PASS();
}

/* ---- multi-output ---- */

TEST(multi_output_two) {
    double out1[VS], out2[VS];
    double* outs[] = { out1, out2 };

    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("440 sinosc dup 0.3 *", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    ASSERT(g->num_roots == 2);
    dsp_graph_process(g, NULL, 0, outs, 2, VS);

    /* output 1 = full sine, output 2 = 0.3x sine */
    for (long i = 0; i < VS; i++) {
        ASSERT_NEAR(out2[i], out1[i] * 0.3, 1e-10);
    }
    dsp_graph_free(g);
    PASS();
}

TEST(multi_output_three) {
    double out1[VS], out2[VS], out3[VS];
    double* outs[] = { out1, out2, out3 };

    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("1 2 3", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    ASSERT(g->num_roots == 3);
    dsp_graph_process(g, NULL, 0, outs, 3, VS);

    ASSERT_NEAR(out1[0], 1.0, 1e-10);
    ASSERT_NEAR(out2[0], 2.0, 1e-10);
    ASSERT_NEAR(out3[0], 3.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(multi_output_silence_extra) {
    double out1[VS], out2[VS];
    double* outs[] = { out1, out2 };
    for (long i = 0; i < VS; i++) out2[i] = 99.0;

    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("42", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    ASSERT(g->num_roots == 1);
    dsp_graph_process(g, NULL, 0, outs, 2, VS);

    ASSERT_NEAR(out1[0], 42.0, 1e-10);
    /* outlet 2 should be silenced */
    ASSERT_NEAR(out2[0], 0.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(multi_output_too_many_fails) {
    char err[DSP_ERR_BUF];
    /* 9 values on stack exceeds DSP_MAX_OUTPUTS=8 */
    dsp_graph* g = dsp_compile("1 2 3 4 5 6 7 8 9", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g == NULL);
    ASSERT(strstr(err, "max") != NULL);
    PASS();
}

/* ---- feedback ---- */

TEST(feedback_basic) {
    double out_buf[VS];
    double* outs[] = { out_buf };

    char err[DSP_ERR_BUF];
    /* fbread1 starts at 0, add 1, write back. After first block,
       fb bus should contain 1.0 */
    dsp_graph* g = dsp_compile("fbread1 1 + dup fbwrite1", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);

    /* first block: fbread=0, +1=1 for all samples */
    dsp_graph_process(g, NULL, 0, outs, 1, VS);
    ASSERT_NEAR(out_buf[0], 1.0, 1e-10);
    ASSERT_NEAR(out_buf[VS-1], 1.0, 1e-10);

    /* second block: fbread=1 (from previous block's write), +1=2 */
    for (int i = 0; i < g->node_count; i++) g->nodes[i].visited = 0;
    dsp_graph_process(g, NULL, 0, outs, 1, VS);
    ASSERT_NEAR(out_buf[0], 2.0, 1e-10);

    dsp_graph_free(g);
    PASS();
}

TEST(feedback_compile_check) {
    char err[DSP_ERR_BUF];
    /* fbwrite is a sink -- should reduce stack by 1 */
    dsp_graph* g = dsp_compile("1 dup fbwrite1", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    ASSERT(g->num_roots == 1);
    ASSERT(g->fb_writer_count == 1);
    dsp_graph_free(g);
    PASS();
}

/* ---- let bindings ---- */

TEST(let_basic) {
    double out_buf[VS];
    double* outs[] = { out_buf };

    dsp_graph* g = compile_and_process("21 let x  x x +", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 42.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(let_multi_output) {
    double out1[VS], out2[VS];
    double* outs[] = { out1, out2 };

    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("440 sinosc let s  s  s 0.3 *", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    ASSERT(g->num_roots == 2);
    dsp_graph_process(g, NULL, 0, outs, 2, VS);

    for (long i = 0; i < VS; i++) {
        ASSERT_NEAR(out2[i], out1[i] * 0.3, 1e-10);
    }
    dsp_graph_free(g);
    PASS();
}

TEST(let_multiple_bindings) {
    double out_buf[VS];
    double* outs[] = { out_buf };

    dsp_graph* g = compile_and_process("10 let a  20 let b  a b +", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 30.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(let_shadow) {
    double out_buf[VS];
    double* outs[] = { out_buf };

    dsp_graph* g = compile_and_process("5 let x  10 let x  x", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 10.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(let_underflow_fails) {
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("let x", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g == NULL);
    ASSERT(strstr(err, "underflow") != NULL);
    PASS();
}

TEST(let_missing_name_fails) {
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("42 let", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g == NULL);
    ASSERT(strstr(err, "name") != NULL);
    PASS();
}

/* ---- constant folding ---- */

TEST(fold_basic_math) {
    char err[DSP_ERR_BUF];
    /* 21 21 + should fold to a single const node */
    dsp_graph* g = dsp_compile("21 21 +", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    /* all 3 nodes (two consts + add) should fold, leaving the add as const */
    ASSERT(g->root[0]->type == DSP_CONST);
    ASSERT_NEAR(g->root[0]->state.constant, 42.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(fold_chain) {
    char err[DSP_ERR_BUF];
    /* 2 3 + 4 * = 20, should fold through the chain */
    dsp_graph* g = dsp_compile("2 3 + 4 *", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    ASSERT(g->root[0]->type == DSP_CONST);
    ASSERT_NEAR(g->root[0]->state.constant, 20.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(fold_mtof) {
    char err[DSP_ERR_BUF];
    /* 69 mtof should fold to 440 */
    dsp_graph* g = dsp_compile("69 mtof", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    ASSERT(g->root[0]->type == DSP_CONST);
    ASSERT_NEAR(g->root[0]->state.constant, 440.0, 1e-6);
    dsp_graph_free(g);
    PASS();
}

TEST(fold_no_fold_stateful) {
    char err[DSP_ERR_BUF];
    /* 440 sinosc -- sinosc is stateful, should NOT fold */
    dsp_graph* g = dsp_compile("440 sinosc", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    ASSERT(g->root[0]->type == DSP_SINOSC);
    dsp_graph_free(g);
    PASS();
}

TEST(fold_partial) {
    char err[DSP_ERR_BUF];
    /* 440 1.003 * sinosc -- the multiply should fold, sinosc should not */
    dsp_graph* g = dsp_compile("440 1.003 * sinosc", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    ASSERT(g->root[0]->type == DSP_SINOSC);
    /* the sinosc's input should have been folded to const */
    ASSERT(g->root[0]->in[0]->type == DSP_CONST);
    ASSERT_NEAR(g->root[0]->in[0]->state.constant, 441.32, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(fold_let_reuse) {
    char err[DSP_ERR_BUF];
    /* 21 let x  x x + should fold to 42 */
    dsp_graph* g = dsp_compile("21 let x  x x +", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    ASSERT(g->root[0]->type == DSP_CONST);
    ASSERT_NEAR(g->root[0]->state.constant, 42.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

/* ---- aliases ---- */

TEST(alias_add) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("10 20 add", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 30.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(alias_lerp) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("0 10 0.5 lerp", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 5.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(alias_drop) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("42 99 drop", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 42.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

/* ---- edge cases ---- */

TEST(process_sah) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    /* constant signal 42, constant trigger 1 -> should hold 42 */
    dsp_graph* g = compile_and_process("42 1 sah", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 42.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(process_latch) {
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_and_process("5 latch", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 5.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(graph_free_null) {
    /* should not crash */
    dsp_graph_free(NULL);
    PASS();
}

/* ---- compile-time functions ---- */

/* helper: compile with func table, process one block, return graph */
static dsp_graph* compile_with_funcs_and_process(const char* expr,
                                                  const dsp_func_table* funcs,
                                                  double** outs, long numouts)
{
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile(expr, SR, VS, funcs, err, DSP_ERR_BUF);
    if (!g) {
        printf("FAIL\n    compile error: %s\n", err);
        return NULL;
    }
    dsp_graph_process(g, NULL, 0, outs, numouts, VS);
    return g;
}

TEST(func_basic) {
    dsp_func_table ft;
    dsp_func_table_init(&ft);
    dsp_func_define(&ft, "double", "dup +", NULL, 0);

    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_with_funcs_and_process("21 double", &ft, outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 42.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(func_nested) {
    dsp_func_table ft;
    dsp_func_table_init(&ft);
    dsp_func_define(&ft, "a", "2 *", NULL, 0);
    dsp_func_define(&ft, "b", "a 3 +", NULL, 0);

    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_with_funcs_and_process("10 b", &ft, outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 23.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(func_with_let) {
    dsp_func_table ft;
    dsp_func_table_init(&ft);
    dsp_func_define(&ft, "osc2", "let f f sinosc f 1.003 * sinosc + 0.5 *", NULL, 0);

    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_with_funcs_and_process("440 osc2", &ft, outs, 1);
    ASSERT(g != NULL);
    /* output should be bounded */
    for (long i = 0; i < VS; i++) {
        ASSERT(out_buf[i] >= -1.1 && out_buf[i] <= 1.1);
    }
    ASSERT(g->num_roots == 1);
    dsp_graph_free(g);
    PASS();
}

TEST(func_undefined_fails) {
    dsp_func_table ft;
    dsp_func_table_init(&ft);

    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("21 nope", SR, VS, &ft, err, DSP_ERR_BUF);
    ASSERT(g == NULL);
    ASSERT(strstr(err, "unknown token") != NULL);
    PASS();
}

TEST(func_recursion_depth) {
    dsp_func_table ft;
    dsp_func_table_init(&ft);
    dsp_func_define(&ft, "r", "r", NULL, 0);

    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("r", SR, VS, &ft, err, DSP_ERR_BUF);
    ASSERT(g == NULL);
    ASSERT(strstr(err, "too deep") != NULL);
    PASS();
}

TEST(func_table_helpers) {
    dsp_func_table ft;
    dsp_func_table_init(&ft);
    ASSERT(ft.count == 0);

    /* define */
    int ok = dsp_func_define(&ft, "foo", "1 +", NULL, 0);
    ASSERT(ok == 1);
    ASSERT(ft.count == 1);

    /* overwrite */
    ok = dsp_func_define(&ft, "foo", "2 +", NULL, 0);
    ASSERT(ok == 1);
    ASSERT(ft.count == 1);
    ASSERT(strcmp(ft.funcs[0].body, "2 +") == 0);

    /* define second */
    ok = dsp_func_define(&ft, "bar", "3 *", NULL, 0);
    ASSERT(ok == 1);
    ASSERT(ft.count == 2);

    /* undef */
    ok = dsp_func_undef(&ft, "foo");
    ASSERT(ok == 1);
    ASSERT(ft.count == 1);
    ASSERT(strcmp(ft.funcs[0].name, "bar") == 0);

    /* undef non-existent */
    ok = dsp_func_undef(&ft, "nope");
    ASSERT(ok == 0);

    /* clear */
    dsp_func_clear(&ft);
    ASSERT(ft.count == 0);

    PASS();
}

/* ---- main ---- */

int main(void)
{
    printf("dsp_graph tests:\n\n");

    /* compilation */
    run_test_compile_const();
    run_test_compile_empty_fails();
    run_test_compile_unknown_token_fails();
    run_test_compile_stack_underflow();

    /* basic math */
    run_test_process_const();
    run_test_process_add();
    run_test_process_sub();
    run_test_process_mul();
    run_test_process_div();
    run_test_process_div_by_zero();
    run_test_process_neg_literal();
    run_test_process_pow();
    run_test_process_mod();
    run_test_process_min_max();
    run_test_process_clip();
    run_test_process_mix();

    /* unary math */
    run_test_process_abs();
    run_test_process_neg();
    run_test_process_floor_ceil_round();
    run_test_process_sign();
    run_test_process_trig();
    run_test_process_wrap();
    run_test_process_mtof_ftom();
    run_test_process_db2a_a2db();

    /* stack */
    run_test_stack_dup();
    run_test_stack_swap();
    run_test_stack_pop();
    run_test_stack_over();
    run_test_stack_rot();

    /* generators */
    run_test_process_sinosc();
    run_test_process_phasor();
    run_test_process_noise();
    run_test_process_pink();
    run_test_process_dust();
    run_test_process_tri();
    run_test_process_saw();
    run_test_process_pulse();

    /* envelope */
    run_test_process_decay();

    /* filters */
    run_test_process_onepole();
    run_test_process_lag();
    run_test_process_slew();
    run_test_process_biquad();

    /* delay */
    run_test_process_delay();

    /* input */
    run_test_process_input();

    /* multi-output */
    run_test_multi_output_two();
    run_test_multi_output_three();
    run_test_multi_output_silence_extra();
    run_test_multi_output_too_many_fails();

    /* feedback */
    run_test_feedback_basic();
    run_test_feedback_compile_check();

    /* let bindings */
    run_test_let_basic();
    run_test_let_multi_output();
    run_test_let_multiple_bindings();
    run_test_let_shadow();
    run_test_let_underflow_fails();
    run_test_let_missing_name_fails();

    /* constant folding */
    run_test_fold_basic_math();
    run_test_fold_chain();
    run_test_fold_mtof();
    run_test_fold_no_fold_stateful();
    run_test_fold_partial();
    run_test_fold_let_reuse();

    /* aliases */
    run_test_alias_add();
    run_test_alias_lerp();
    run_test_alias_drop();

    /* edge cases */
    run_test_process_sah();
    run_test_process_latch();
    run_test_graph_free_null();

    /* compile-time functions */
    run_test_func_basic();
    run_test_func_nested();
    run_test_func_with_let();
    run_test_func_undefined_fails();
    run_test_func_recursion_depth();
    run_test_func_table_helpers();

    printf("\n%d tests: %d passed, %d failed\n",
           tests_run, tests_passed, tests_failed);

    return tests_failed > 0 ? 1 : 0;
}
