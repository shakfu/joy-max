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

TEST(process_comparisons) {
    double out_buf[VS];
    double* outs[] = { out_buf };

    /* gt */
    dsp_graph* g = compile_and_process("5 3 gt", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 1.0, 1e-10);
    dsp_graph_free(g);

    g = compile_and_process("3 5 gt", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 0.0, 1e-10);
    dsp_graph_free(g);

    /* lt */
    g = compile_and_process("3 5 lt", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 1.0, 1e-10);
    dsp_graph_free(g);

    g = compile_and_process("5 3 lt", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 0.0, 1e-10);
    dsp_graph_free(g);

    /* gte */
    g = compile_and_process("5 5 gte", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 1.0, 1e-10);
    dsp_graph_free(g);

    g = compile_and_process("3 5 gte", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 0.0, 1e-10);
    dsp_graph_free(g);

    /* lte */
    g = compile_and_process("5 5 lte", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 1.0, 1e-10);
    dsp_graph_free(g);

    g = compile_and_process("5 3 lte", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 0.0, 1e-10);
    dsp_graph_free(g);

    /* eq */
    g = compile_and_process("7 7 eq", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 1.0, 1e-10);
    dsp_graph_free(g);

    g = compile_and_process("7 8 eq", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 0.0, 1e-10);
    dsp_graph_free(g);

    /* neq */
    g = compile_and_process("7 8 neq", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 1.0, 1e-10);
    dsp_graph_free(g);

    g = compile_and_process("7 7 neq", outs, 1);
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

TEST(process_noise_independent) {
    double out1[VS], out2[VS];
    double* outs[] = { out1, out2 };

    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("noise noise", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    ASSERT(g->num_roots == 2);
    dsp_graph_process(g, NULL, 0, outs, 2, VS);

    int all_same = 1;
    for (long i = 0; i < VS; i++) {
        if (out1[i] != out2[i]) { all_same = 0; break; }
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

TEST(process_pink_independent) {
    /* Two pink nodes in the same graph must have independent counters.
       Before the fix, a static counter was shared across all instances,
       causing identical output sequences when nodes were pulled in order. */
    double out1[VS], out2[VS];
    double* outs[] = { out1, out2 };

    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("pink pink", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    ASSERT(g->num_roots == 2);
    dsp_graph_process(g, NULL, 0, outs, 2, VS);

    /* The two pink nodes use different RNG seeds (same initial seed but
       pulled independently), so their outputs must differ. */
    int all_same = 1;
    for (long i = 0; i < VS; i++) {
        if (out1[i] != out2[i]) { all_same = 0; break; }
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

TEST(process_dust_independent) {
    /* Two dust nodes at high density should produce different impulse patterns */
    double out1[VS], out2[VS];
    double* outs[] = { out1, out2 };

    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("10000 dust 10000 dust", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    ASSERT(g->num_roots == 2);
    dsp_graph_process(g, NULL, 0, outs, 2, VS);

    int all_same = 1;
    for (long i = 0; i < VS; i++) {
        if (out1[i] != out2[i]) { all_same = 0; break; }
    }
    ASSERT(!all_same);
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

TEST(process_line) {
    /* trigger=1, time=0.01s -> ramp from 0 to 1 */
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("1 0.01 line", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);

    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph_process(g, NULL, 0, outs, 1, VS);

    /* first sample should be 0.0 (ramp starts at 0, output before increment) */
    ASSERT_NEAR(out_buf[0], 0.0, 1e-10);
    /* should be monotonically increasing */
    for (long i = 1; i < VS; i++) {
        ASSERT(out_buf[i] >= out_buf[i - 1]);
        ASSERT(out_buf[i] >= 0.0 && out_buf[i] <= 1.0);
    }
    /* at 44100 Hz, 0.01s = 441 samples. After 64 samples we should be
       partway through (~14.5%), well above 0 */
    ASSERT(out_buf[VS - 1] > 0.1);
    ASSERT(out_buf[VS - 1] < 1.0);

    /* run enough blocks to complete the ramp, then verify it reaches 1.0 */
    for (int blk = 0; blk < 10; blk++)
        dsp_graph_process(g, NULL, 0, outs, 1, VS);
    ASSERT_NEAR(out_buf[VS - 1], 1.0, 1e-10);

    dsp_graph_free(g);
    PASS();
}

TEST(process_ar) {
    /* trigger=1, attack=0.005s, release=0.005s */
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("1 0.005 0.005 ar", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);

    double out_buf[VS];
    double* outs[] = { out_buf };

    /* run enough blocks to complete the full AR cycle (~441 samples total) */
    double peak = 0.0;
    int found_rise = 0, found_fall = 0;
    for (int blk = 0; blk < 10; blk++) {
        dsp_graph_process(g, NULL, 0, outs, 1, VS);
        for (long i = 0; i < VS; i++) {
            ASSERT(out_buf[i] >= -1e-10 && out_buf[i] <= 1.0 + 1e-10);
            if (out_buf[i] > peak + 1e-10) found_rise = 1;
            if (out_buf[i] < peak - 1e-10) found_fall = 1;
            if (out_buf[i] > peak) peak = out_buf[i];
        }
    }
    /* envelope must have risen to ~1.0, then fallen back toward 0 */
    ASSERT_NEAR(peak, 1.0, 0.01);
    ASSERT(found_rise);
    ASSERT(found_fall);
    /* after full cycle, should be back at 0 */
    ASSERT_NEAR(out_buf[VS - 1], 0.0, 1e-10);

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

TEST(process_hp1) {
    /* DC input through a highpass should converge to 0 (DC is blocked) */
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("1 1000 hp1", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);

    double out_buf[VS];
    double* outs[] = { out_buf };
    for (int blk = 0; blk < 100; blk++)
        dsp_graph_process(g, NULL, 0, outs, 1, VS);

    /* after settling, DC should be fully rejected */
    ASSERT_NEAR(out_buf[VS - 1], 0.0, 1e-4);
    dsp_graph_free(g);
    PASS();
}

TEST(process_svflp_atten) {
    /* A lowpass at 100 Hz fed with a 10 kHz sine should heavily attenuate.
       We generate 10 kHz by using sinosc and check peak amplitude. */
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("10000 sinosc 100 0.707 svflp", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);

    double out_buf[VS];
    double* outs[] = { out_buf };
    /* let filter settle */
    for (int blk = 0; blk < 50; blk++)
        dsp_graph_process(g, NULL, 0, outs, 1, VS);

    /* find peak amplitude -- should be heavily attenuated */
    double peak = 0.0;
    for (long i = 0; i < VS; i++) {
        double a = fabs(out_buf[i]);
        if (a > peak) peak = a;
    }
    /* 10 kHz is ~40 dB above 100 Hz cutoff; expect < 0.05 amplitude */
    ASSERT(peak < 0.05);
    dsp_graph_free(g);
    PASS();
}

TEST(process_svfhp_atten) {
    /* A highpass at 10 kHz fed with a 100 Hz sine should heavily attenuate. */
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("100 sinosc 10000 0.707 svfhp", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);

    double out_buf[VS];
    double* outs[] = { out_buf };
    for (int blk = 0; blk < 50; blk++)
        dsp_graph_process(g, NULL, 0, outs, 1, VS);

    double peak = 0.0;
    for (long i = 0; i < VS; i++) {
        double a = fabs(out_buf[i]);
        if (a > peak) peak = a;
    }
    ASSERT(peak < 0.05);
    dsp_graph_free(g);
    PASS();
}

TEST(process_svfbp_atten) {
    /* A bandpass centered at 10 kHz fed with a 100 Hz sine should attenuate. */
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("100 sinosc 10000 0.707 svfbp", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);

    double out_buf[VS];
    double* outs[] = { out_buf };
    for (int blk = 0; blk < 50; blk++)
        dsp_graph_process(g, NULL, 0, outs, 1, VS);

    double peak = 0.0;
    for (long i = 0; i < VS; i++) {
        double a = fabs(out_buf[i]);
        if (a > peak) peak = a;
    }
    ASSERT(peak < 0.05);
    dsp_graph_free(g);
    PASS();
}

TEST(process_svfnotch) {
    double out_buf[VS];
    double* outs[] = { out_buf };

    /* A notch filter at 1000 Hz should pass DC (0 Hz) through.
       Feed a constant (DC) signal; after settling, output ~= input. */
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("1 1000 0.707 svfnotch", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);

    /* run several blocks to let the filter settle */
    for (int blk = 0; blk < 100; blk++)
        dsp_graph_process(g, NULL, 0, outs, 1, VS);

    /* DC should pass through a notch (notch only rejects the center freq) */
    ASSERT_NEAR(out_buf[VS - 1], 1.0, 1e-4);
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

TEST(process_delay_multi_sample) {
    /* Feed an impulse, delay by 10 samples.
       Sample 0..9 should be 0, sample 10 should be 1. */
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("in1 10 delay", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);

    double in_buf[VS], out_buf[VS];
    double* ins[]  = { in_buf };
    double* outs[] = { out_buf };
    memset(in_buf, 0, sizeof(in_buf));
    in_buf[0] = 1.0;  /* impulse at sample 0 */

    dsp_graph_process(g, ins, 1, outs, 1, VS);

    for (long i = 0; i < 10; i++)
        ASSERT_NEAR(out_buf[i], 0.0, 1e-10);
    ASSERT_NEAR(out_buf[10], 1.0, 1e-10);
    /* samples after impulse should be 0 again */
    for (long i = 11; i < VS; i++)
        ASSERT_NEAR(out_buf[i], 0.0, 1e-10);

    dsp_graph_free(g);
    PASS();
}

TEST(process_delayf_integer) {
    /* At integer delay times, delayf should behave like delay */
    double out_buf[VS];
    double* outs[] = { out_buf };

    dsp_graph* g = compile_and_process("1 0 delayf", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 1.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(process_delayf_fractional) {
    /* Feed an impulse (1 at sample 0, then 0), delay by 0.5 samples.
       At sample 0: write 1, read at pos -0.5 -> lerp(buf[0], buf[-1]) = lerp(1, 0) at frac=0.5 -> 0.5
       At sample 1: write 0, read at pos 0.5  -> lerp(buf[1], buf[0]) = lerp(0, 1) at frac=0.5 -> 0.5
       At sample 2: write 0, read at pos 1.5  -> lerp(buf[2], buf[1]) = lerp(0, 0) at frac=0.5 -> 0.0 */
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("in1 0.5 delayf", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);

    double in_buf[VS], out_buf[VS];
    double* ins[]  = { in_buf };
    double* outs[] = { out_buf };
    memset(in_buf, 0, sizeof(in_buf));
    in_buf[0] = 1.0;  /* impulse */

    dsp_graph_process(g, ins, 1, outs, 1, VS);

    ASSERT_NEAR(out_buf[0], 0.5, 1e-10);
    ASSERT_NEAR(out_buf[1], 0.5, 1e-10);
    ASSERT_NEAR(out_buf[2], 0.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(delay_one_sample) {
    /* delay by 1 sample: impulse at sample 0 should appear at sample 1 */
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("in1 1 delay", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);

    double in_buf[VS], out_buf[VS];
    double* ins[]  = { in_buf };
    double* outs[] = { out_buf };
    memset(in_buf, 0, sizeof(in_buf));
    in_buf[0] = 1.0;

    dsp_graph_process(g, ins, 1, outs, 1, VS);

    ASSERT_NEAR(out_buf[0], 0.0, 1e-10);
    ASSERT_NEAR(out_buf[1], 1.0, 1e-10);
    for (long i = 2; i < VS; i++)
        ASSERT_NEAR(out_buf[i], 0.0, 1e-10);

    dsp_graph_free(g);
    PASS();
}

TEST(delay_near_max) {
    /* delay value exceeding buf_len should be clamped to buf_len-1.
       buf_len = 4*SR = 176400. Request 200000 samples delay.
       Feed a constant 1.0 -- on first block, all reads are from zeroed buffer. */
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("in1 200000 delay", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);

    double in_buf[VS], out_buf[VS];
    double* ins[]  = { in_buf };
    double* outs[] = { out_buf };
    for (long i = 0; i < VS; i++) in_buf[i] = 1.0;

    dsp_graph_process(g, ins, 1, outs, 1, VS);

    /* all output should be 0 since we're reading from zeroed buffer at max delay */
    for (long i = 0; i < VS; i++)
        ASSERT_NEAR(out_buf[i], 0.0, 1e-10);

    dsp_graph_free(g);
    PASS();
}

TEST(delayf_near_max) {
    /* delayf clamps to buf_len-2. Request a huge value.
       Feed constant 1.0 -- first block reads from zeroed buffer. */
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("in1 200000.5 delayf", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);

    double in_buf[VS], out_buf[VS];
    double* ins[]  = { in_buf };
    double* outs[] = { out_buf };
    for (long i = 0; i < VS; i++) in_buf[i] = 1.0;

    dsp_graph_process(g, ins, 1, outs, 1, VS);

    /* all output should be 0 since we're reading from zeroed buffer at clamped max */
    for (long i = 0; i < VS; i++)
        ASSERT_NEAR(out_buf[i], 0.0, 1e-10);

    dsp_graph_free(g);
    PASS();
}

TEST(delayc_integer) {
    /* At integer delay times, delayc should pass through like delay/delayf */
    double out_buf[VS];
    double* outs[] = { out_buf };

    dsp_graph* g = compile_and_process("1 0 delayc", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 1.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(delayc_fractional) {
    /* Feed an impulse (1 at sample 0, then 0), delay by 0.5 samples.
       Cubic Hermite uses 4 points: ym1, y0, y1, y2.
       The result should differ from linear interpolation's simple 0.5. */
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("in1 0.5 delayc", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);

    double in_buf[VS], out_buf[VS];
    double* ins[]  = { in_buf };
    double* outs[] = { out_buf };
    memset(in_buf, 0, sizeof(in_buf));
    in_buf[0] = 1.0;  /* impulse */

    dsp_graph_process(g, ins, 1, outs, 1, VS);

    /* Cubic Hermite with 4 points around each read position.
       Sample 0: ym1=0, y0=1, y1=0, y2=0; frac=0.5
         c0=1, c1=0, c2=-2.5, c3=1.5 -> 0.5625
       Sample 1: ym1=0, y0=0, y1=1, y2=0; frac=0.5
         c0=0, c1=0.5, c2=2, c3=-1.5 -> 0.5625
       Sample 2: ym1=0, y0=0, y1=0, y2=1; frac=0.5
         c0=0, c1=0, c2=-0.5, c3=0.5 -> -0.0625 */
    ASSERT_NEAR(out_buf[0], 0.5625, 1e-10);
    ASSERT_NEAR(out_buf[1], 0.5625, 1e-10);
    ASSERT_NEAR(out_buf[2], -0.0625, 1e-10);
    /* Verify cubic differs from linear (linear gives 0.5, 0.5, 0.0) */
    ASSERT(fabs(out_buf[0] - 0.5) > 0.01);

    dsp_graph_free(g);
    PASS();
}

TEST(delayc_near_max) {
    /* delayc clamps to buf_len-3. Request a huge value.
       Feed constant 1.0 -- first block reads from zeroed buffer. */
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("in1 200000 delayc", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);

    double in_buf[VS], out_buf[VS];
    double* ins[]  = { in_buf };
    double* outs[] = { out_buf };
    for (long i = 0; i < VS; i++) in_buf[i] = 1.0;

    dsp_graph_process(g, ins, 1, outs, 1, VS);

    /* all output should be 0 since we're reading from zeroed buffer at clamped max */
    for (long i = 0; i < VS; i++)
        ASSERT_NEAR(out_buf[i], 0.0, 1e-10);

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

/* ---- multi-name let ---- */

TEST(let_multi_basic) {
    double out_buf[VS];
    double* outs[] = { out_buf };

    dsp_graph* g = compile_and_process("10 20 let | a b | a b +", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 30.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(let_multi_three) {
    /* 440 110 200 let | d m c | -> d=200(top), m=110, c=440 */
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("440 110 200 let | d m c | m sinosc d * c + sinosc",
                               SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    dsp_graph_free(g);
    PASS();
}

TEST(let_multi_single_bar) {
    double out_buf[VS];
    double* outs[] = { out_buf };

    dsp_graph* g = compile_and_process("42 let | x | x", outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 42.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(let_multi_missing_bar) {
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("10 20 let | a b", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g == NULL);
    PASS();
}

TEST(let_multi_underflow) {
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("10 let | a b |", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g == NULL);
    ASSERT(strstr(err, "underflow") != NULL);
    PASS();
}

TEST(let_multi_empty) {
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("let | |", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g == NULL);
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

TEST(fold_comparisons) {
    char err[DSP_ERR_BUF];

    /* 5 3 gt -> 1.0 */
    dsp_graph* g = dsp_compile("5 3 gt", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    ASSERT(g->root[0]->type == DSP_CONST);
    ASSERT_NEAR(g->root[0]->state.constant, 1.0, 1e-10);
    dsp_graph_free(g);

    /* 3 5 gt -> 0.0 */
    g = dsp_compile("3 5 gt", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    ASSERT(g->root[0]->type == DSP_CONST);
    ASSERT_NEAR(g->root[0]->state.constant, 0.0, 1e-10);
    dsp_graph_free(g);

    /* 3 3 eq -> 1.0 */
    g = dsp_compile("3 3 eq", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    ASSERT(g->root[0]->type == DSP_CONST);
    ASSERT_NEAR(g->root[0]->state.constant, 1.0, 1e-10);
    dsp_graph_free(g);

    /* 3 4 neq -> 1.0 */
    g = dsp_compile("3 4 neq", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    ASSERT(g->root[0]->type == DSP_CONST);
    ASSERT_NEAR(g->root[0]->state.constant, 1.0, 1e-10);
    dsp_graph_free(g);

    /* 5 5 lte -> 1.0 */
    g = dsp_compile("5 5 lte", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    ASSERT(g->root[0]->type == DSP_CONST);
    ASSERT_NEAR(g->root[0]->state.constant, 1.0, 1e-10);
    dsp_graph_free(g);

    /* 3 5 gte -> 0.0 */
    g = dsp_compile("3 5 gte", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);
    ASSERT(g->root[0]->type == DSP_CONST);
    ASSERT_NEAR(g->root[0]->state.constant, 0.0, 1e-10);
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

TEST(process_sah_rising_edge) {
    /* sah should only capture on rising edge (trig goes from <=0 to >0).
       Feed changing signal with a trigger that fires at a specific sample. */
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("in1 in2 sah", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);

    double sig_buf[VS], trig_buf[VS], out_buf[VS];
    double* ins[]  = { sig_buf, trig_buf };
    double* outs[] = { out_buf };

    /* signal ramps 0,1,2,...; trigger fires at sample 5 only */
    for (long i = 0; i < VS; i++) {
        sig_buf[i] = (double)i;
        trig_buf[i] = (i == 5) ? 1.0 : 0.0;
    }

    dsp_graph_process(g, ins, 2, outs, 1, VS);

    /* before trigger: held value is 0 (initial) */
    for (long i = 0; i < 5; i++)
        ASSERT_NEAR(out_buf[i], 0.0, 1e-10);
    /* at sample 5 and after: held value should be 5.0 */
    for (long i = 5; i < VS; i++)
        ASSERT_NEAR(out_buf[i], 5.0, 1e-10);

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

TEST(process_latch_zero_hold) {
    /* latch should hold the last non-zero value when input becomes 0 */
    char err[DSP_ERR_BUF];
    dsp_graph* g = dsp_compile("in1 latch", SR, VS, NULL, err, DSP_ERR_BUF);
    ASSERT(g != NULL);

    double in_buf[VS], out_buf[VS];
    double* ins[]  = { in_buf };
    double* outs[] = { out_buf };

    /* first 10 samples = 7.0, then zeros */
    for (long i = 0; i < VS; i++)
        in_buf[i] = (i < 10) ? 7.0 : 0.0;

    dsp_graph_process(g, ins, 1, outs, 1, VS);

    /* all samples should output 7.0: first 10 because input=7,
       rest because latch holds the last non-zero value */
    for (long i = 0; i < VS; i++)
        ASSERT_NEAR(out_buf[i], 7.0, 1e-10);

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

TEST(func_body_too_long) {
    dsp_func_table ft;
    dsp_func_table_init(&ft);
    char err[DSP_ERR_BUF];

    /* build a body string longer than DSP_FUNC_BODY_LEN (1024) */
    char long_body[2048];
    memset(long_body, 'x', sizeof(long_body));
    long_body[sizeof(long_body) - 1] = '\0';

    int ok = dsp_func_define(&ft, "big", long_body, err, DSP_ERR_BUF);
    ASSERT(ok == 0);
    ASSERT(strstr(err, "body too long") != NULL);
    ASSERT(ft.count == 0);
    PASS();
}

TEST(func_name_too_long) {
    dsp_func_table ft;
    dsp_func_table_init(&ft);
    char err[DSP_ERR_BUF];

    /* build a name string longer than DSP_FUNC_NAME_LEN (64) */
    char long_name[128];
    memset(long_name, 'a', sizeof(long_name));
    long_name[sizeof(long_name) - 1] = '\0';

    int ok = dsp_func_define(&ft, long_name, "1 +", err, DSP_ERR_BUF);
    ASSERT(ok == 0);
    ASSERT(strstr(err, "name too long") != NULL);
    ASSERT(ft.count == 0);
    PASS();
}

/* ---- file loading (dsp_func_load_text) ---- */

TEST(load_text_basic) {
    dsp_func_table ft;
    dsp_func_table_init(&ft);

    const char* text =
        "def double dup +\n"
        "def triple dup dup + +\n";

    char err[DSP_ERR_BUF];
    int n = dsp_func_load_text(&ft, text, err, DSP_ERR_BUF);
    ASSERT(n == 2);
    ASSERT(ft.count == 2);

    /* verify both definitions work */
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_with_funcs_and_process("21 double", &ft, outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 42.0, 1e-10);
    dsp_graph_free(g);

    g = compile_with_funcs_and_process("10 triple", &ft, outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 30.0, 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(load_text_comments) {
    dsp_func_table ft;
    dsp_func_table_init(&ft);

    const char* text =
        "# this is a comment\n"
        "\n"
        "   # indented comment\n"
        "def double dup +\n"
        "\n"
        "   \n"
        "# another comment\n"
        "def negate neg\n";

    char err[DSP_ERR_BUF];
    int n = dsp_func_load_text(&ft, text, err, DSP_ERR_BUF);
    ASSERT(n == 2);
    ASSERT(ft.count == 2);
    PASS();
}

TEST(load_text_bad_line) {
    dsp_func_table ft;
    dsp_func_table_init(&ft);

    const char* text =
        "def double dup +\n"
        "this is not a def\n";

    char err[DSP_ERR_BUF];
    int n = dsp_func_load_text(&ft, text, err, DSP_ERR_BUF);
    ASSERT(n == -1);
    ASSERT(strstr(err, "line 2") != NULL);
    ASSERT(strstr(err, "def") != NULL);
    PASS();
}

TEST(load_text_missing_body) {
    dsp_func_table ft;
    dsp_func_table_init(&ft);

    const char* text = "def foo\n";

    char err[DSP_ERR_BUF];
    int n = dsp_func_load_text(&ft, text, err, DSP_ERR_BUF);
    ASSERT(n == -1);
    ASSERT(strstr(err, "missing body") != NULL);
    PASS();
}

TEST(load_text_integration) {
    dsp_func_table ft;
    dsp_func_table_init(&ft);

    const char* text =
        "# prelude\n"
        "def double dup +\n"
        "def softclip tanh\n";

    char err[DSP_ERR_BUF];
    int n = dsp_func_load_text(&ft, text, err, DSP_ERR_BUF);
    ASSERT(n == 2);

    /* use loaded defs in a compiled expression */
    double out_buf[VS];
    double* outs[] = { out_buf };
    dsp_graph* g = compile_with_funcs_and_process("21 double", &ft, outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 42.0, 1e-10);
    dsp_graph_free(g);

    g = compile_with_funcs_and_process("0.5 softclip", &ft, outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], tanh(0.5), 1e-10);
    dsp_graph_free(g);
    PASS();
}

TEST(load_text_crlf) {
    dsp_func_table ft;
    dsp_func_table_init(&ft);

    const char* text = "def double dup +\r\ndef negate neg\r\n";

    char err[DSP_ERR_BUF];
    int n = dsp_func_load_text(&ft, text, err, DSP_ERR_BUF);
    ASSERT(n == 2);
    ASSERT(ft.count == 2);
    PASS();
}

TEST(load_text_prelude) {
    dsp_func_table ft;
    dsp_func_table_init(&ft);

    const char* prelude =
        "# prelude.dsp -- common DSP function definitions\n"
        "#\n"
        "# Load via: [dsp_graph~ 1 2 prelude.dsp] or \"load prelude.dsp\"\n"
        "\n"
        "# ------- math utilities -------\n"
        "def double dup +\n"
        "def half 0.5 *\n"
        "def square dup *\n"
        "def reciprocal 1 swap /\n"
        "def clamp01 0 1 clip\n"
        "def bipolar 2 * 1 -\n"
        "def unipolar 1 + 0.5 *\n"
        "\n"
        "# ------- conversions -------\n"
        "def semi2ratio 12 / 2 swap pow\n"
        "def bpm2hz 60 /\n"
        "\n"
        "# ------- waveshaping -------\n"
        "def softclip tanh\n"
        "def hardclip -1 1 clip\n"
        "def saturate dup abs 1 + /\n"
        "\n"
        "# ------- oscillators -------\n"
        "def osc2 dup 1.003 * sinosc swap sinosc + 0.5 *\n"
        "def subosc let | f | f sinosc f 0.5 * sinosc + 0.5 *\n"
        "def fmosc let | d m c | m sinosc d * c + sinosc\n"
        "def ring sinosc *\n"
        "\n"
        "# ------- filters -------\n"
        "def lp 0.707 svflp\n"
        "def hp 0.707 svfhp\n"
        "def dcblock 20 hp1\n"
        "\n"
        "# ------- conditionals -------\n"
        "def select mix\n"
        "def gate let | t s | s s t gt *\n"
        "\n"
        "# ------- dynamics -------\n"
        "def noiseburst decay noise *\n"
        "def burst let | t | t 0.002 0.05 ar noise *\n"
        "\n"
        "# ------- effects -------\n"
        "def flange let | r s | s s r sinosc 20 * 40 + delayf + 0.5 *\n"
        "\n"
        "# ------- stereo -------\n"
        "def pan let | p s | s 1 p - * s p *\n"
        "def spread let | f | f 1.003 * sinosc f 0.997 * sinosc\n";

    char err[DSP_ERR_BUF];
    int n = dsp_func_load_text(&ft, prelude, err, DSP_ERR_BUF);
    ASSERT(n == 26);
    ASSERT(ft.count == 26);

    double out_buf[VS];
    double* outs[] = { out_buf };

    /* double */
    dsp_graph* g = compile_with_funcs_and_process("21 double", &ft, outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 42.0, 1e-10);
    dsp_graph_free(g);

    /* half */
    g = compile_with_funcs_and_process("10 half", &ft, outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 5.0, 1e-10);
    dsp_graph_free(g);

    /* square */
    g = compile_with_funcs_and_process("7 square", &ft, outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 49.0, 1e-10);
    dsp_graph_free(g);

    /* reciprocal */
    g = compile_with_funcs_and_process("4 reciprocal", &ft, outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 0.25, 1e-10);
    dsp_graph_free(g);

    /* clamp01 */
    g = compile_with_funcs_and_process("1.5 clamp01", &ft, outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 1.0, 1e-10);
    dsp_graph_free(g);

    /* bipolar: 0.5 -> 0.0 */
    g = compile_with_funcs_and_process("0.5 bipolar", &ft, outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 0.0, 1e-10);
    dsp_graph_free(g);

    /* unipolar: 0.0 -> 0.5 */
    g = compile_with_funcs_and_process("0 unipolar", &ft, outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 0.5, 1e-10);
    dsp_graph_free(g);

    /* semi2ratio: 12 semitones = octave = 2.0 */
    g = compile_with_funcs_and_process("12 semi2ratio", &ft, outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 2.0, 1e-6);
    dsp_graph_free(g);

    /* bpm2hz: 120 BPM = 2 Hz */
    g = compile_with_funcs_and_process("120 bpm2hz", &ft, outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 2.0, 1e-10);
    dsp_graph_free(g);

    /* hardclip */
    g = compile_with_funcs_and_process("5 hardclip", &ft, outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 1.0, 1e-10);
    dsp_graph_free(g);

    /* saturate: 0.5/(1+0.5) = 1/3 */
    g = compile_with_funcs_and_process("0.5 saturate", &ft, outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 0.5 / 1.5, 1e-10);
    dsp_graph_free(g);

    /* softclip: tanh(1) */
    g = compile_with_funcs_and_process("1 softclip", &ft, outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], tanh(1.0), 1e-10);
    dsp_graph_free(g);

    /* osc2 compiles and produces bounded output */
    g = compile_with_funcs_and_process("440 osc2", &ft, outs, 1);
    ASSERT(g != NULL);
    for (long i = 0; i < VS; i++)
        ASSERT(out_buf[i] >= -1.1 && out_buf[i] <= 1.1);
    dsp_graph_free(g);

    /* subosc compiles and produces bounded output */
    g = compile_with_funcs_and_process("440 subosc", &ft, outs, 1);
    ASSERT(g != NULL);
    for (long i = 0; i < VS; i++)
        ASSERT(out_buf[i] >= -1.1 && out_buf[i] <= 1.1);
    dsp_graph_free(g);

    /* fmosc compiles */
    g = compile_with_funcs_and_process("440 110 200 fmosc", &ft, outs, 1);
    ASSERT(g != NULL);
    dsp_graph_free(g);

    /* lp compiles (needs signal + freq) */
    g = compile_with_funcs_and_process("noise 2000 lp", &ft, outs, 1);
    ASSERT(g != NULL);
    dsp_graph_free(g);

    /* hp compiles */
    g = compile_with_funcs_and_process("noise 200 hp", &ft, outs, 1);
    ASSERT(g != NULL);
    dsp_graph_free(g);

    /* dcblock compiles */
    g = compile_with_funcs_and_process("noise dcblock", &ft, outs, 1);
    ASSERT(g != NULL);
    dsp_graph_free(g);

    /* ring compiles */
    g = compile_with_funcs_and_process("440 sinosc 110 ring", &ft, outs, 1);
    ASSERT(g != NULL);
    dsp_graph_free(g);

    /* select: 10 20 1 select -> 20 (cond=1 picks second) */
    g = compile_with_funcs_and_process("10 20 1 select", &ft, outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 20.0, 1e-10);
    dsp_graph_free(g);

    /* select: 10 20 0 select -> 10 (cond=0 picks first) */
    g = compile_with_funcs_and_process("10 20 0 select", &ft, outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 10.0, 1e-10);
    dsp_graph_free(g);

    /* gate: 0.8 0.5 gate -> 0.8 (above threshold) */
    g = compile_with_funcs_and_process("0.8 0.5 gate", &ft, outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 0.8, 1e-10);
    dsp_graph_free(g);

    /* gate: 0.3 0.5 gate -> 0 (below threshold) */
    g = compile_with_funcs_and_process("0.3 0.5 gate", &ft, outs, 1);
    ASSERT(g != NULL);
    ASSERT_NEAR(out_buf[0], 0.0, 1e-10);
    dsp_graph_free(g);

    /* burst compiles (trigger + ar + noise) */
    g = compile_with_funcs_and_process("1 burst", &ft, outs, 1);
    ASSERT(g != NULL);
    dsp_graph_free(g);

    /* flange compiles (signal + lfo rate + delayf) */
    g = compile_with_funcs_and_process("440 sinosc 0.5 flange", &ft, outs, 1);
    ASSERT(g != NULL);
    dsp_graph_free(g);

    /* pan produces 2 outputs */
    double out2[VS];
    double* outs_stereo[] = { out_buf, out2 };
    char perr[DSP_ERR_BUF];
    g = dsp_compile("440 sinosc 0.3 * 0.25 pan", SR, VS, &ft, perr, DSP_ERR_BUF);
    ASSERT(g != NULL);
    ASSERT(g->num_roots == 2);
    dsp_graph_process(g, NULL, 0, outs_stereo, 2, VS);
    /* pos=0.25: L = signal*0.75, R = signal*0.25 -> L > R */
    ASSERT(fabs(out_buf[1]) > fabs(out2[1]));
    dsp_graph_free(g);

    /* spread produces 2 outputs */
    g = dsp_compile("440 spread", SR, VS, &ft, perr, DSP_ERR_BUF);
    ASSERT(g != NULL);
    ASSERT(g->num_roots == 2);
    dsp_graph_process(g, NULL, 0, outs_stereo, 2, VS);
    dsp_graph_free(g);

    PASS();
}

TEST(load_text_empty) {
    dsp_func_table ft;
    dsp_func_table_init(&ft);

    char err[DSP_ERR_BUF];
    int n = dsp_func_load_text(&ft, "", err, DSP_ERR_BUF);
    ASSERT(n == 0);
    ASSERT(ft.count == 0);

    n = dsp_func_load_text(&ft, "# only comments\n\n", err, DSP_ERR_BUF);
    ASSERT(n == 0);
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
    run_test_process_comparisons();
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
    run_test_process_noise_independent();
    run_test_process_pink();
    run_test_process_pink_independent();
    run_test_process_dust();
    run_test_process_dust_independent();
    run_test_process_tri();
    run_test_process_saw();
    run_test_process_pulse();

    /* envelope */
    run_test_process_decay();
    run_test_process_line();
    run_test_process_ar();

    /* filters */
    run_test_process_onepole();
    run_test_process_lag();
    run_test_process_slew();
    run_test_process_biquad();
    run_test_process_hp1();
    run_test_process_svflp_atten();
    run_test_process_svfhp_atten();
    run_test_process_svfbp_atten();
    run_test_process_svfnotch();

    /* delay */
    run_test_process_delay();
    run_test_process_delay_multi_sample();
    run_test_process_delayf_integer();
    run_test_process_delayf_fractional();
    run_test_delay_one_sample();
    run_test_delay_near_max();
    run_test_delayf_near_max();
    run_test_delayc_integer();
    run_test_delayc_fractional();
    run_test_delayc_near_max();

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

    /* multi-name let */
    run_test_let_multi_basic();
    run_test_let_multi_three();
    run_test_let_multi_single_bar();
    run_test_let_multi_missing_bar();
    run_test_let_multi_underflow();
    run_test_let_multi_empty();

    /* constant folding */
    run_test_fold_basic_math();
    run_test_fold_chain();
    run_test_fold_mtof();
    run_test_fold_no_fold_stateful();
    run_test_fold_partial();
    run_test_fold_let_reuse();
    run_test_fold_comparisons();

    /* aliases */
    run_test_alias_add();
    run_test_alias_lerp();
    run_test_alias_drop();

    /* edge cases */
    run_test_process_sah();
    run_test_process_sah_rising_edge();
    run_test_process_latch();
    run_test_process_latch_zero_hold();
    run_test_graph_free_null();

    /* compile-time functions */
    run_test_func_basic();
    run_test_func_nested();
    run_test_func_with_let();
    run_test_func_undefined_fails();
    run_test_func_recursion_depth();
    run_test_func_table_helpers();
    run_test_func_body_too_long();
    run_test_func_name_too_long();

    /* file loading (dsp_func_load_text) */
    run_test_load_text_basic();
    run_test_load_text_comments();
    run_test_load_text_bad_line();
    run_test_load_text_missing_body();
    run_test_load_text_integration();
    run_test_load_text_prelude();
    run_test_load_text_crlf();
    run_test_load_text_empty();

    printf("\n%d tests: %d passed, %d failed\n",
           tests_run, tests_passed, tests_failed);

    return tests_failed > 0 ? 1 : 0;
}
