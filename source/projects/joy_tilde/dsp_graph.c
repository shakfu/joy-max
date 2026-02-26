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
            DSP_FREE(g); return NULL; \
        } \
        stack[sp++] = (nd); \
    } while(0)

    #define POP_NODE(dst) do { \
        if (sp <= 0) { \
            set_err(err, errlen, "stack underflow at '%s'", tok.symbol); \
            DSP_FREE(g); return NULL; \
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
            DSP_FREE(g);
            return NULL;
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
                DSP_FREE(g);
                return NULL;
            }
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_INPUT;
            nd->pull = pull_input;
            nd->n_inputs = 0;
            nd->state.inlet_idx = idx;
            PUSH_NODE(nd);
            continue;
        }

        /* binary operators */
        if (strcmp(sym, "+") == 0 || strcmp(sym, "add") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_ADD; nd->pull = pull_add; nd->n_inputs = 2;
            POP_NODE(nd->in[1]); POP_NODE(nd->in[0]);
            PUSH_NODE(nd);
        } else if (strcmp(sym, "-") == 0 || strcmp(sym, "sub") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_SUB; nd->pull = pull_sub; nd->n_inputs = 2;
            POP_NODE(nd->in[1]); POP_NODE(nd->in[0]);
            PUSH_NODE(nd);
        } else if (strcmp(sym, "*") == 0 || strcmp(sym, "mul") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_MUL; nd->pull = pull_mul; nd->n_inputs = 2;
            POP_NODE(nd->in[1]); POP_NODE(nd->in[0]);
            PUSH_NODE(nd);
        } else if (strcmp(sym, "/") == 0 || strcmp(sym, "div") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_DIV; nd->pull = pull_div; nd->n_inputs = 2;
            POP_NODE(nd->in[1]); POP_NODE(nd->in[0]);
            PUSH_NODE(nd);
        } else if (strcmp(sym, "pow") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_POW; nd->pull = pull_pow; nd->n_inputs = 2;
            POP_NODE(nd->in[1]); POP_NODE(nd->in[0]);
            PUSH_NODE(nd);
        } else if (strcmp(sym, "min") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_MIN; nd->pull = pull_min; nd->n_inputs = 2;
            POP_NODE(nd->in[1]); POP_NODE(nd->in[0]);
            PUSH_NODE(nd);
        } else if (strcmp(sym, "max") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_MAX_OP; nd->pull = pull_max; nd->n_inputs = 2;
            POP_NODE(nd->in[1]); POP_NODE(nd->in[0]);
            PUSH_NODE(nd);

        /* unary operators */
        } else if (strcmp(sym, "neg") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_NEG; nd->pull = pull_neg; nd->n_inputs = 1;
            POP_NODE(nd->in[0]);
            PUSH_NODE(nd);
        } else if (strcmp(sym, "abs") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_ABS; nd->pull = pull_abs; nd->n_inputs = 1;
            POP_NODE(nd->in[0]);
            PUSH_NODE(nd);
        } else if (strcmp(sym, "sin") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_SIN; nd->pull = pull_sin; nd->n_inputs = 1;
            POP_NODE(nd->in[0]);
            PUSH_NODE(nd);
        } else if (strcmp(sym, "cos") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_COS; nd->pull = pull_cos; nd->n_inputs = 1;
            POP_NODE(nd->in[0]);
            PUSH_NODE(nd);
        } else if (strcmp(sym, "tanh") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_TANH; nd->pull = pull_tanh_fn; nd->n_inputs = 1;
            POP_NODE(nd->in[0]);
            PUSH_NODE(nd);
        } else if (strcmp(sym, "exp") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_EXP; nd->pull = pull_exp; nd->n_inputs = 1;
            POP_NODE(nd->in[0]);
            PUSH_NODE(nd);
        } else if (strcmp(sym, "log") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_LOG; nd->pull = pull_log_fn; nd->n_inputs = 1;
            POP_NODE(nd->in[0]);
            PUSH_NODE(nd);
        } else if (strcmp(sym, "sqrt") == 0) {
            dsp_node* nd = ALLOC_NODE();
            nd->type = DSP_SQRT; nd->pull = pull_sqrt_fn; nd->n_inputs = 1;
            POP_NODE(nd->in[0]);
            PUSH_NODE(nd);

        /* stateful generators */
        } else if (strcmp(sym, "sinosc") == 0) {
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
            /* seed with something non-zero */
            nd->state.rng_state = 0x12345678ABCDEF01ULL;
            PUSH_NODE(nd);

        } else {
            set_err(err, errlen, "unknown token '%s'", sym);
            DSP_FREE(g);
            return NULL;
        }
    }

    #undef PUSH_NODE
    #undef POP_NODE
    #undef ALLOC_NODE

    /* must have exactly one value on compile stack */
    if (sp != 1) {
        set_err(err, errlen, "expression leaves %d values on stack (expected 1)", sp);
        DSP_FREE(g);
        return NULL;
    }

    g->root = stack[0];

    /* allocate all output buffers in a single pool */
    long pool_size = (long)g->node_count * vs;
    g->buffer_pool = (double*)DSP_ALLOC(pool_size * sizeof(double));
    if (!g->buffer_pool) {
        set_err(err, errlen, "buffer pool allocation failed");
        DSP_FREE(g);
        return NULL;
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
    if (g->buffer_pool)
        DSP_FREE(g->buffer_pool);
    DSP_FREE(g);
}
