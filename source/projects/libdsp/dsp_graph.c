/*
 *  dsp_graph.c -- Tokenizer, compiler, and pull functions for DSP graphs
 *
 *  Compiles stack-based expressions into a pull-based DSP node graph.
 *  No Joy headers, no GC, no linked-list allocation in the hot path.
 */

#include "dsp_graph.h"

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define TWO_PI (2.0 * M_PI)

/* Use Max allocator when available, else stdlib */
#ifdef SYSMEM_NEWPTR
#define DSP_ALLOC(sz)    sysmem_newptr((long)(sz))
#define DSP_FREE(p)      sysmem_freeptr(p)
#else
#define DSP_ALLOC(sz)    malloc(sz)
#define DSP_FREE(p)      free(p)
#endif

/* ---- tokenizer ---- */

typedef enum {
    TOK_NUMBER,
    TOK_SYMBOL,
    TOK_END,
} dsp_tok_type;

typedef struct {
    dsp_tok_type type;
    double       number;
    char         symbol[64];
} dsp_token;

typedef struct {
    const char* src;
    int         pos;
} dsp_tokenizer;

static void tok_init(dsp_tokenizer* t, const char* src)
{
    t->src = src;
    t->pos = 0;
}

static void tok_skip_ws(dsp_tokenizer* t)
{
    while (t->src[t->pos] && isspace((unsigned char)t->src[t->pos]))
        t->pos++;
}

static dsp_token tok_next(dsp_tokenizer* t)
{
    dsp_token tok;
    memset(&tok, 0, sizeof(tok));

    tok_skip_ws(t);
    if (!t->src[t->pos]) {
        tok.type = TOK_END;
        return tok;
    }

    const char* s = t->src + t->pos;

    /* negative literal: '-' followed by digit, at start or after whitespace */
    if (s[0] == '-' && s[1] && (isdigit((unsigned char)s[1]) || s[1] == '.')) {
        char* end;
        tok.number = strtod(s, &end);
        tok.type = TOK_NUMBER;
        t->pos += (int)(end - s);
        return tok;
    }

    /* number */
    if (isdigit((unsigned char)s[0]) || (s[0] == '.' && isdigit((unsigned char)s[1]))) {
        char* end;
        tok.number = strtod(s, &end);
        tok.type = TOK_NUMBER;
        t->pos += (int)(end - s);
        return tok;
    }

    /* symbol: alphanumeric or single-char operator */
    if (isalpha((unsigned char)s[0]) || s[0] == '_') {
        int i = 0;
        while (s[i] && (isalnum((unsigned char)s[i]) || s[i] == '_') && i < 63) {
            tok.symbol[i] = s[i];
            i++;
        }
        tok.symbol[i] = '\0';
        tok.type = TOK_SYMBOL;
        t->pos += i;
        return tok;
    }

    /* single-char operators: + - * / */
    tok.symbol[0] = s[0];
    tok.symbol[1] = '\0';
    tok.type = TOK_SYMBOL;
    t->pos += 1;
    return tok;
}

/* ---- pull functions ---- */

static inline void dsp_pull(dsp_node* node, long n)
{
    if (node->visited) return;
    node->visited = 1;
    node->pull(node, n);
}

static void pull_const(dsp_node* node, long n)
{
    double v = node->state.constant;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = v;
}

static void pull_input(dsp_node* node, long n)
{
    /* out pointer is set to Max inlet buffer each block -- nothing to do */
    (void)node;
    (void)n;
}

/* --- binary math --- */

static void pull_add(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    dsp_pull(node->in[1], n);
    double* a = node->in[0]->out;
    double* b = node->in[1]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = a[i] + b[i];
}

static void pull_sub(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    dsp_pull(node->in[1], n);
    double* a = node->in[0]->out;
    double* b = node->in[1]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = a[i] - b[i];
}

static void pull_mul(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    dsp_pull(node->in[1], n);
    double* a = node->in[0]->out;
    double* b = node->in[1]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = a[i] * b[i];
}

static void pull_div(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    dsp_pull(node->in[1], n);
    double* a = node->in[0]->out;
    double* b = node->in[1]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = (b[i] != 0.0) ? a[i] / b[i] : 0.0;
}

static void pull_pow(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    dsp_pull(node->in[1], n);
    double* a = node->in[0]->out;
    double* b = node->in[1]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = pow(a[i], b[i]);
}

static void pull_min(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    dsp_pull(node->in[1], n);
    double* a = node->in[0]->out;
    double* b = node->in[1]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = (a[i] < b[i]) ? a[i] : b[i];
}

static void pull_max(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    dsp_pull(node->in[1], n);
    double* a = node->in[0]->out;
    double* b = node->in[1]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = (a[i] > b[i]) ? a[i] : b[i];
}

static void pull_mod(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    dsp_pull(node->in[1], n);
    double* a = node->in[0]->out;
    double* b = node->in[1]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = (b[i] != 0.0) ? fmod(a[i], b[i]) : 0.0;
}

/* --- ternary math --- */

static void pull_clip(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    dsp_pull(node->in[1], n);
    dsp_pull(node->in[2], n);
    double* val = node->in[0]->out;
    double* lo  = node->in[1]->out;
    double* hi  = node->in[2]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++) {
        double v = val[i];
        if (v < lo[i]) v = lo[i];
        if (v > hi[i]) v = hi[i];
        out[i] = v;
    }
}

static void pull_mix(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    dsp_pull(node->in[1], n);
    dsp_pull(node->in[2], n);
    double* a = node->in[0]->out;
    double* b = node->in[1]->out;
    double* t = node->in[2]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = a[i] + (b[i] - a[i]) * t[i];
}

/* --- unary math --- */

static void pull_neg(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    double* a = node->in[0]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = -a[i];
}

static void pull_abs(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    double* a = node->in[0]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = fabs(a[i]);
}

static void pull_sin(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    double* a = node->in[0]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = sin(a[i]);
}

static void pull_cos(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    double* a = node->in[0]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = cos(a[i]);
}

static void pull_tanh_fn(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    double* a = node->in[0]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = tanh(a[i]);
}

static void pull_exp(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    double* a = node->in[0]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = exp(a[i]);
}

static void pull_log_fn(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    double* a = node->in[0]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = (a[i] > 0.0) ? log(a[i]) : 0.0;
}

static void pull_sqrt_fn(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    double* a = node->in[0]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = (a[i] >= 0.0) ? sqrt(a[i]) : 0.0;
}

static void pull_wrap(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    double* a = node->in[0]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = a[i] - floor(a[i]);
}

static void pull_fold(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    double* a = node->in[0]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++) {
        /* triangle-fold into [0,1): phase in [0,2) then fold back */
        double v = a[i] - floor(a[i]);  /* wrap to [0,1) */
        v *= 2.0;                        /* scale to [0,2) */
        if (v > 1.0) v = 2.0 - v;       /* fold back */
        out[i] = v;
    }
}

static void pull_floor(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    double* a = node->in[0]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = floor(a[i]);
}

static void pull_ceil(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    double* a = node->in[0]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = ceil(a[i]);
}

static void pull_round(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    double* a = node->in[0]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = round(a[i]);
}

static void pull_sign(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    double* a = node->in[0]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = (a[i] > 0.0) ? 1.0 : (a[i] < 0.0) ? -1.0 : 0.0;
}

static void pull_db2a(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    double* a = node->in[0]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = pow(10.0, a[i] / 20.0);
}

static void pull_a2db(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    double* a = node->in[0]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++) {
        double v = fabs(a[i]);
        out[i] = (v > 0.0) ? 20.0 * log10(v) : -120.0;
    }
}

static void pull_mtof(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    double* a = node->in[0]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = 440.0 * pow(2.0, (a[i] - 69.0) / 12.0);
}

static void pull_ftom(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    double* a = node->in[0]->out;
    double* out = node->out;
    for (long i = 0; i < n; i++)
        out[i] = (a[i] > 0.0) ? 69.0 + 12.0 * log2(a[i] / 440.0) : 0.0;
}

/* --- stateful generators --- */

static void pull_sinosc(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    double* freq = node->in[0]->out;
    double* out = node->out;
    double phase = node->state.osc.phase;
    double inv_sr = node->state.osc.inv_sr;

    for (long i = 0; i < n; i++) {
        out[i] = sin(TWO_PI * phase);
        phase += freq[i] * inv_sr;
        /* keep phase in [0,1) to avoid float precision loss */
        phase -= (double)(long)phase;
        if (phase < 0.0) phase += 1.0;
    }
    node->state.osc.phase = phase;
}

static void pull_phasor(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    double* freq = node->in[0]->out;
    double* out = node->out;
    double phase = node->state.osc.phase;
    double inv_sr = node->state.osc.inv_sr;

    for (long i = 0; i < n; i++) {
        out[i] = phase;
        phase += freq[i] * inv_sr;
        phase -= (double)(long)phase;
        if (phase < 0.0) phase += 1.0;
    }
    node->state.osc.phase = phase;
}

/* xorshift64 RNG */
static inline uint64_t xorshift64(uint64_t* s)
{
    uint64_t x = *s;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    *s = x;
    return x;
}

static void pull_noise(dsp_node* node, long n)
{
    double* out = node->out;
    uint64_t rng = node->state.rng_state;
    for (long i = 0; i < n; i++) {
        uint64_t r = xorshift64(&rng);
        /* map to [-1, 1] */
        out[i] = (double)(int64_t)r / (double)INT64_MAX;
    }
    node->state.rng_state = rng;
}

static void pull_tri(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    double* freq = node->in[0]->out;
    double* out = node->out;
    double phase = node->state.osc.phase;
    double inv_sr = node->state.osc.inv_sr;

    for (long i = 0; i < n; i++) {
        /* triangle: 4*|phase - 0.5| - 1, where phase in [0,1) */
        out[i] = 4.0 * fabs(phase - 0.5) - 1.0;
        phase += freq[i] * inv_sr;
        phase -= (double)(long)phase;
        if (phase < 0.0) phase += 1.0;
    }
    node->state.osc.phase = phase;
}

static void pull_saw(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    double* freq = node->in[0]->out;
    double* out = node->out;
    double phase = node->state.osc.phase;
    double inv_sr = node->state.osc.inv_sr;

    for (long i = 0; i < n; i++) {
        out[i] = 2.0 * phase - 1.0;
        phase += freq[i] * inv_sr;
        phase -= (double)(long)phase;
        if (phase < 0.0) phase += 1.0;
    }
    node->state.osc.phase = phase;
}

static void pull_pulse(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    dsp_pull(node->in[1], n);
    double* freq = node->in[0]->out;
    double* duty = node->in[1]->out;
    double* out = node->out;
    double phase = node->state.osc.phase;
    double inv_sr = node->state.osc.inv_sr;

    for (long i = 0; i < n; i++) {
        out[i] = (phase < duty[i]) ? 1.0 : -1.0;
        phase += freq[i] * inv_sr;
        phase -= (double)(long)phase;
        if (phase < 0.0) phase += 1.0;
    }
    node->state.osc.phase = phase;
}

/* --- filters --- */

static void pull_onepole(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    dsp_pull(node->in[1], n);
    double* sig = node->in[0]->out;
    double* fc  = node->in[1]->out;
    double* out = node->out;
    double y1 = node->state.onepole.y1;
    double inv_sr = node->state.onepole.inv_sr;

    for (long i = 0; i < n; i++) {
        double coeff = 1.0 - exp(-TWO_PI * fc[i] * inv_sr);
        y1 += coeff * (sig[i] - y1);
        out[i] = y1;
    }
    node->state.onepole.y1 = y1;
}

static void pull_hp1(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    dsp_pull(node->in[1], n);
    double* sig = node->in[0]->out;
    double* fc  = node->in[1]->out;
    double* out = node->out;
    double y1 = node->state.onepole.y1;
    double inv_sr = node->state.onepole.inv_sr;

    for (long i = 0; i < n; i++) {
        double coeff = 1.0 - exp(-TWO_PI * fc[i] * inv_sr);
        y1 += coeff * (sig[i] - y1);
        out[i] = sig[i] - y1;
    }
    node->state.onepole.y1 = y1;
}

/* SVF (Zavalishin/Simper topology) -- shared tick, different output taps */
static inline void svf_tick(double input, double cutoff_hz, double q,
                            double inv_sr,
                            double* ic1eq, double* ic2eq,
                            double* lp, double* bp, double* hp)
{
    double g = tan(M_PI * cutoff_hz * inv_sr);
    double k = 1.0 / q;
    double a1 = 1.0 / (1.0 + g * (g + k));
    double a2 = g * a1;
    double a3 = g * a2;

    double v3 = input - *ic2eq;
    double v1 = a1 * *ic1eq + a2 * v3;
    double v2 = *ic2eq + a2 * *ic1eq + a3 * v3;

    *ic1eq = 2.0 * v1 - *ic1eq;
    *ic2eq = 2.0 * v2 - *ic2eq;

    *lp = v2;
    *bp = v1;
    *hp = input - k * v1 - v2;
}

static void pull_svflp(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    dsp_pull(node->in[1], n);
    dsp_pull(node->in[2], n);
    double* sig = node->in[0]->out;
    double* fc  = node->in[1]->out;
    double* qp  = node->in[2]->out;
    double* out = node->out;
    double ic1 = node->state.svf.ic1eq;
    double ic2 = node->state.svf.ic2eq;
    double inv_sr = node->state.svf.inv_sr;

    for (long i = 0; i < n; i++) {
        double lp, bp, hp;
        svf_tick(sig[i], fc[i], qp[i], inv_sr, &ic1, &ic2, &lp, &bp, &hp);
        out[i] = lp;
    }
    node->state.svf.ic1eq = ic1;
    node->state.svf.ic2eq = ic2;
}

static void pull_svfhp(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    dsp_pull(node->in[1], n);
    dsp_pull(node->in[2], n);
    double* sig = node->in[0]->out;
    double* fc  = node->in[1]->out;
    double* qp  = node->in[2]->out;
    double* out = node->out;
    double ic1 = node->state.svf.ic1eq;
    double ic2 = node->state.svf.ic2eq;
    double inv_sr = node->state.svf.inv_sr;

    for (long i = 0; i < n; i++) {
        double lp, bp, hp;
        svf_tick(sig[i], fc[i], qp[i], inv_sr, &ic1, &ic2, &lp, &bp, &hp);
        out[i] = hp;
    }
    node->state.svf.ic1eq = ic1;
    node->state.svf.ic2eq = ic2;
}

static void pull_svfbp(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    dsp_pull(node->in[1], n);
    dsp_pull(node->in[2], n);
    double* sig = node->in[0]->out;
    double* fc  = node->in[1]->out;
    double* qp  = node->in[2]->out;
    double* out = node->out;
    double ic1 = node->state.svf.ic1eq;
    double ic2 = node->state.svf.ic2eq;
    double inv_sr = node->state.svf.inv_sr;

    for (long i = 0; i < n; i++) {
        double lp, bp, hp;
        svf_tick(sig[i], fc[i], qp[i], inv_sr, &ic1, &ic2, &lp, &bp, &hp);
        out[i] = bp;
    }
    node->state.svf.ic1eq = ic1;
    node->state.svf.ic2eq = ic2;
}

/* --- delay --- */

static void pull_delay(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    dsp_pull(node->in[1], n);
    double* sig  = node->in[0]->out;
    double* time = node->in[1]->out;
    double* out  = node->out;
    double* buf  = node->state.delay.buf;
    long buf_len = node->state.delay.buf_len;
    long wp      = node->state.delay.write_pos;

    for (long i = 0; i < n; i++) {
        buf[wp] = sig[i];
        long delay_samps = (long)time[i];
        if (delay_samps < 0) delay_samps = 0;
        if (delay_samps >= buf_len) delay_samps = buf_len - 1;
        long rp = wp - delay_samps;
        if (rp < 0) rp += buf_len;
        out[i] = buf[rp];
        wp++;
        if (wp >= buf_len) wp = 0;
    }
    node->state.delay.write_pos = wp;
}

/* --- utilities --- */

static void pull_sah(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    dsp_pull(node->in[1], n);
    double* sig  = node->in[0]->out;
    double* trig = node->in[1]->out;
    double* out  = node->out;
    double held  = node->state.sah.held;
    double prev  = node->state.sah.prev_trig;

    for (long i = 0; i < n; i++) {
        /* rising edge: previous <= 0, current > 0 */
        if (trig[i] > 0.0 && prev <= 0.0)
            held = sig[i];
        prev = trig[i];
        out[i] = held;
    }
    node->state.sah.held = held;
    node->state.sah.prev_trig = prev;
}

static void pull_latch(dsp_node* node, long n)
{
    dsp_pull(node->in[0], n);
    double* sig = node->in[0]->out;
    double* out = node->out;
    double held = node->state.latch.held;

    for (long i = 0; i < n; i++) {
        if (sig[i] != 0.0)
            held = sig[i];
        out[i] = held;
    }
    node->state.latch.held = held;
}

/* ---- op table for stateless ops ---- */

typedef struct {
    const char*    name;
    const char*    alias;     /* NULL if none */
    dsp_node_type  type;
    dsp_pull_fn    pull;
    int            n_inputs;
} dsp_op_entry;

static const dsp_op_entry dsp_op_table[] = {
    /* binary math */
    { "+",     "add",   DSP_ADD,    pull_add,      2 },
    { "-",     "sub",   DSP_SUB,    pull_sub,      2 },
    { "*",     "mul",   DSP_MUL,    pull_mul,      2 },
    { "/",     "div",   DSP_DIV,    pull_div,      2 },
    { "pow",   NULL,    DSP_POW,    pull_pow,      2 },
    { "min",   NULL,    DSP_MIN,    pull_min,      2 },
    { "max",   NULL,    DSP_MAX_OP, pull_max,      2 },
    { "mod",   NULL,    DSP_MOD,    pull_mod,      2 },
    /* ternary math */
    { "clip",  NULL,    DSP_CLIP,   pull_clip,     3 },
    { "mix",   "lerp",  DSP_MIX,    pull_mix,      3 },
    /* unary math */
    { "neg",   NULL,    DSP_NEG,    pull_neg,      1 },
    { "abs",   NULL,    DSP_ABS,    pull_abs,      1 },
    { "sin",   NULL,    DSP_SIN,    pull_sin,      1 },
    { "cos",   NULL,    DSP_COS,    pull_cos,      1 },
    { "tanh",  NULL,    DSP_TANH,   pull_tanh_fn,  1 },
    { "exp",   NULL,    DSP_EXP,    pull_exp,      1 },
    { "log",   NULL,    DSP_LOG,    pull_log_fn,   1 },
    { "sqrt",  NULL,    DSP_SQRT,   pull_sqrt_fn,  1 },
    { "wrap",  NULL,    DSP_WRAP,   pull_wrap,     1 },
    { "fold",  NULL,    DSP_FOLD,   pull_fold,     1 },
    { "floor", NULL,    DSP_FLOOR,  pull_floor,    1 },
    { "ceil",  NULL,    DSP_CEIL,   pull_ceil,     1 },
    { "round", NULL,    DSP_ROUND,  pull_round,    1 },
    { "sign",  NULL,    DSP_SIGN,   pull_sign,     1 },
    { "db2a",  NULL,    DSP_DB2A,   pull_db2a,     1 },
    { "a2db",  NULL,    DSP_A2DB,   pull_a2db,     1 },
    { "mtof",  NULL,    DSP_MTOF,   pull_mtof,     1 },
    { "ftom",  NULL,    DSP_FTOM,   pull_ftom,     1 },
};

#define DSP_OP_TABLE_LEN (sizeof(dsp_op_table) / sizeof(dsp_op_table[0]))

/* ---- compiler helpers ---- */

static void set_err(char* err, int errlen, const char* fmt, ...)
{
    if (!err || errlen <= 0) return;
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(err, errlen, fmt, ap);
    va_end(ap);
}

/* ---- compiler ---- */

dsp_graph* dsp_compile(const char* expr, double sr, long vs,
                       char* err, int errlen)
{
    if (!expr || !expr[0]) {
        set_err(err, errlen, "empty expression");
        return NULL;
    }

    dsp_graph* g = (dsp_graph*)DSP_ALLOC(sizeof(dsp_graph));
    if (!g) {
        set_err(err, errlen, "allocation failed");
        return NULL;
    }
    memset(g, 0, sizeof(dsp_graph));
    g->sample_rate = sr;
    g->vector_size = vs;

    /* compile-time stack of node pointers */
    dsp_node* stack[DSP_MAX_NODES];
    int sp = 0;

    dsp_tokenizer tokenizer;
    tok_init(&tokenizer, expr);

    #define PUSH_NODE(nd) do { \
        if (sp >= DSP_MAX_NODES) { \
            set_err(err, errlen, "stack overflow"); \
            goto fail; \
        } \
        stack[sp++] = (nd); \
    } while(0)

    #define POP_NODE(dst) do { \
        if (sp <= 0) { \
            set_err(err, errlen, "stack underflow at '%s'", tok.symbol); \
            goto fail; \
        } \
        (dst) = stack[--sp]; \
    } while(0)

    #define ALLOC_NODE() (&g->nodes[g->node_count++])

    for (;;) {
        dsp_token tok = tok_next(&tokenizer);

        if (tok.type == TOK_END)
            break;

        if (g->node_count >= DSP_MAX_NODES) {
            set_err(err, errlen, "too many nodes (max %d)", DSP_MAX_NODES);
            goto fail;
        }

        if (tok.type == TOK_NUMBER) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_CONST;
            nd->pull = pull_const;
            nd->n_inputs = 0;
            nd->state.constant = tok.number;
            PUSH_NODE(nd);
            continue;
        }

        /* TOK_SYMBOL */
        const char* sym = tok.symbol;

        /* input references: in1..in8 */
        if (sym[0] == 'i' && sym[1] == 'n' && isdigit((unsigned char)sym[2]) && sym[3] == '\0') {
            int idx = sym[2] - '1';  /* in1 -> 0, in2 -> 1, etc. */
            if (idx < 0 || idx > 7) {
                set_err(err, errlen, "invalid input '%s' (use in1..in8)", sym);
                goto fail;
            }
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_INPUT;
            nd->pull = pull_input;
            nd->n_inputs = 0;
            nd->state.inlet_idx = idx;
            PUSH_NODE(nd);
            continue;
        }

        /* compile-time stack manipulation (no nodes allocated) */
        if (strcmp(sym, "dup") == 0) {
            if (sp < 1) {
                set_err(err, errlen, "stack underflow at 'dup'");
                goto fail;
            }
            dsp_node* top = stack[sp - 1];
            PUSH_NODE(top);
            continue;
        }
        if (strcmp(sym, "swap") == 0) {
            if (sp < 2) {
                set_err(err, errlen, "stack underflow at 'swap'");
                goto fail;
            }
            dsp_node* tmp = stack[sp - 1];
            stack[sp - 1] = stack[sp - 2];
            stack[sp - 2] = tmp;
            continue;
        }
        if (strcmp(sym, "pop") == 0 || strcmp(sym, "drop") == 0) {
            if (sp < 1) {
                set_err(err, errlen, "stack underflow at '%s'", sym);
                goto fail;
            }
            sp--;
            continue;
        }
        if (strcmp(sym, "over") == 0) {
            if (sp < 2) {
                set_err(err, errlen, "stack underflow at 'over'");
                goto fail;
            }
            dsp_node* second = stack[sp - 2];
            PUSH_NODE(second);
            continue;
        }
        if (strcmp(sym, "rot") == 0) {
            if (sp < 3) {
                set_err(err, errlen, "stack underflow at 'rot'");
                goto fail;
            }
            /* a b c -> b c a */
            dsp_node* a = stack[sp - 3];
            stack[sp - 3] = stack[sp - 2];
            stack[sp - 2] = stack[sp - 1];
            stack[sp - 1] = a;
            continue;
        }

        /* look up in stateless op table */
        int found = 0;
        for (int ti = 0; ti < (int)DSP_OP_TABLE_LEN; ti++) {
            const dsp_op_entry* e = &dsp_op_table[ti];
            if (strcmp(sym, e->name) == 0 ||
                (e->alias && strcmp(sym, e->alias) == 0)) {
                dsp_node* nd = ALLOC_NODE();
                nd->type = e->type;
                nd->pull = e->pull;
                nd->n_inputs = e->n_inputs;
                /* pop inputs in reverse order */
                for (int k = e->n_inputs - 1; k >= 0; k--) {
                    POP_NODE(nd->in[k]);
                }
                PUSH_NODE(nd);
                found = 1;
                break;
            }
        }
        if (found) continue;

        /* stateful generators (need custom state init) */
        if (strcmp(sym, "sinosc") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_SINOSC; nd->pull = pull_sinosc; nd->n_inputs = 1;
            nd->state.osc.phase = 0.0;
            nd->state.osc.inv_sr = 1.0 / sr;
            POP_NODE(nd->in[0]);
            PUSH_NODE(nd);
        } else if (strcmp(sym, "phasor") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_PHASOR; nd->pull = pull_phasor; nd->n_inputs = 1;
            nd->state.osc.phase = 0.0;
            nd->state.osc.inv_sr = 1.0 / sr;
            POP_NODE(nd->in[0]);
            PUSH_NODE(nd);
        } else if (strcmp(sym, "noise") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_NOISE; nd->pull = pull_noise; nd->n_inputs = 0;
            nd->state.rng_state = 0x12345678ABCDEF01ULL;
            PUSH_NODE(nd);
        } else if (strcmp(sym, "tri") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_TRI; nd->pull = pull_tri; nd->n_inputs = 1;
            nd->state.osc.phase = 0.0;
            nd->state.osc.inv_sr = 1.0 / sr;
            POP_NODE(nd->in[0]);
            PUSH_NODE(nd);
        } else if (strcmp(sym, "saw") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_SAW; nd->pull = pull_saw; nd->n_inputs = 1;
            nd->state.osc.phase = 0.0;
            nd->state.osc.inv_sr = 1.0 / sr;
            POP_NODE(nd->in[0]);
            PUSH_NODE(nd);
        } else if (strcmp(sym, "pulse") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_PULSE; nd->pull = pull_pulse; nd->n_inputs = 2;
            nd->state.osc.phase = 0.0;
            nd->state.osc.inv_sr = 1.0 / sr;
            POP_NODE(nd->in[1]);
            POP_NODE(nd->in[0]);
            PUSH_NODE(nd);

        /* filters */
        } else if (strcmp(sym, "onepole") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_ONEPOLE; nd->pull = pull_onepole; nd->n_inputs = 2;
            nd->state.onepole.y1 = 0.0;
            nd->state.onepole.inv_sr = 1.0 / sr;
            POP_NODE(nd->in[1]);
            POP_NODE(nd->in[0]);
            PUSH_NODE(nd);
        } else if (strcmp(sym, "hp1") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_HP1; nd->pull = pull_hp1; nd->n_inputs = 2;
            nd->state.onepole.y1 = 0.0;
            nd->state.onepole.inv_sr = 1.0 / sr;
            POP_NODE(nd->in[1]);
            POP_NODE(nd->in[0]);
            PUSH_NODE(nd);
        } else if (strcmp(sym, "svflp") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_SVFLP; nd->pull = pull_svflp; nd->n_inputs = 3;
            nd->state.svf.ic1eq = 0.0;
            nd->state.svf.ic2eq = 0.0;
            nd->state.svf.inv_sr = 1.0 / sr;
            POP_NODE(nd->in[2]);
            POP_NODE(nd->in[1]);
            POP_NODE(nd->in[0]);
            PUSH_NODE(nd);
        } else if (strcmp(sym, "svfhp") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_SVFHP; nd->pull = pull_svfhp; nd->n_inputs = 3;
            nd->state.svf.ic1eq = 0.0;
            nd->state.svf.ic2eq = 0.0;
            nd->state.svf.inv_sr = 1.0 / sr;
            POP_NODE(nd->in[2]);
            POP_NODE(nd->in[1]);
            POP_NODE(nd->in[0]);
            PUSH_NODE(nd);
        } else if (strcmp(sym, "svfbp") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_SVFBP; nd->pull = pull_svfbp; nd->n_inputs = 3;
            nd->state.svf.ic1eq = 0.0;
            nd->state.svf.ic2eq = 0.0;
            nd->state.svf.inv_sr = 1.0 / sr;
            POP_NODE(nd->in[2]);
            POP_NODE(nd->in[1]);
            POP_NODE(nd->in[0]);
            PUSH_NODE(nd);

        /* delay */
        } else if (strcmp(sym, "delay") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_DELAY; nd->pull = pull_delay; nd->n_inputs = 2;
            long max_delay = (long)(4.0 * sr);
            if (max_delay < 1) max_delay = 1;
            nd->state.delay.buf = (double*)DSP_ALLOC(max_delay * sizeof(double));
            if (!nd->state.delay.buf) {
                set_err(err, errlen, "delay buffer allocation failed");
                goto fail;
            }
            memset(nd->state.delay.buf, 0, max_delay * sizeof(double));
            nd->state.delay.buf_len = max_delay;
            nd->state.delay.write_pos = 0;
            POP_NODE(nd->in[1]);
            POP_NODE(nd->in[0]);
            PUSH_NODE(nd);

        /* utilities */
        } else if (strcmp(sym, "sah") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_SAH; nd->pull = pull_sah; nd->n_inputs = 2;
            nd->state.sah.held = 0.0;
            nd->state.sah.prev_trig = 0.0;
            POP_NODE(nd->in[1]);
            POP_NODE(nd->in[0]);
            PUSH_NODE(nd);
        } else if (strcmp(sym, "latch") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_LATCH; nd->pull = pull_latch; nd->n_inputs = 1;
            nd->state.latch.held = 0.0;
            POP_NODE(nd->in[0]);
            PUSH_NODE(nd);

        } else {
            set_err(err, errlen, "unknown token '%s'", sym);
            goto fail;
        }
    }

    #undef PUSH_NODE
    #undef POP_NODE
    #undef ALLOC_NODE

    /* must have exactly one value on compile stack */
    if (sp != 1) {
        set_err(err, errlen, "expression leaves %d values on stack (expected 1)", sp);
        goto fail;
    }

    g->root = stack[0];

    /* allocate all output buffers in a single pool */
    long pool_size = (long)g->node_count * vs;
    g->buffer_pool = (double*)DSP_ALLOC(pool_size * sizeof(double));
    if (!g->buffer_pool) {
        set_err(err, errlen, "buffer pool allocation failed");
        goto fail;
    }
    memset(g->buffer_pool, 0, pool_size * sizeof(double));

    /* assign each node its buffer slice (except DSP_INPUT, which uses zero-copy) */
    for (int i = 0; i < g->node_count; i++) {
        if (g->nodes[i].type == DSP_INPUT) {
            g->nodes[i].out = NULL; /* set per-block */
        } else {
            g->nodes[i].out = g->buffer_pool + (long)i * vs;
        }
    }

    return g;

fail:
    /* free any delay buffers that were allocated before failure */
    for (int i = 0; i < g->node_count; i++) {
        if (g->nodes[i].type == DSP_DELAY && g->nodes[i].state.delay.buf)
            DSP_FREE(g->nodes[i].state.delay.buf);
    }
    if (g->buffer_pool)
        DSP_FREE(g->buffer_pool);
    DSP_FREE(g);
    return NULL;
}

/* ---- process ---- */

void dsp_graph_process(dsp_graph* g,
                       double** ins, long numins,
                       double** outs, long numouts, long n)
{
    if (!g || !g->root) return;

    /* clamp n to vector_size (buffers are sized to vector_size) */
    if (n > g->vector_size) n = g->vector_size;

    /* reset visited flags */
    for (int i = 0; i < g->node_count; i++)
        g->nodes[i].visited = 0;

    /* point DSP_INPUT nodes at Max inlet buffers (zero-copy) */
    for (int i = 0; i < g->node_count; i++) {
        if (g->nodes[i].type == DSP_INPUT) {
            int idx = g->nodes[i].state.inlet_idx;
            if (idx < numins && ins[idx]) {
                g->nodes[i].out = ins[idx];
            } else {
                /* input not connected -- use a zeroed buffer from the pool */
                g->nodes[i].out = g->buffer_pool + (long)i * g->vector_size;
                memset(g->nodes[i].out, 0, n * sizeof(double));
            }
        }
    }

    /* pull from root */
    dsp_pull(g->root, n);

    /* copy root output to outlet 0 */
    if (numouts > 0 && outs[0] && g->root->out) {
        memcpy(outs[0], g->root->out, n * sizeof(double));
    }

    /* silence remaining outlets */
    for (long ch = 1; ch < numouts; ch++) {
        if (outs[ch])
            memset(outs[ch], 0, n * sizeof(double));
    }
}

/* ---- free ---- */

void dsp_graph_free(dsp_graph* g)
{
    if (!g) return;
    /* free delay buffers */
    for (int i = 0; i < g->node_count; i++) {
        if (g->nodes[i].type == DSP_DELAY && g->nodes[i].state.delay.buf)
            DSP_FREE(g->nodes[i].state.delay.buf);
    }
    if (g->buffer_pool)
        DSP_FREE(g->buffer_pool);
    DSP_FREE(g);
}
