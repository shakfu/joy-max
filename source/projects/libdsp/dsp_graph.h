/*
 *  dsp_graph.h -- Compiled DSP node graph for joy~
 *
 *  Compiles stack-based expressions ("440 sinosc 0.3 *") into a native
 *  C DSP node graph at message time. The audio thread pulls through
 *  the graph with zero allocation, zero interpretation, zero GC.
 *
 *  Self-contained: depends only on <math.h>, <string.h>, <stdlib.h>,
 *  <ctype.h>, <stdint.h>. Uses sysmem_newptr/sysmem_freeptr for
 *  allocation when available (Max environment).
 */

#ifndef DSP_GRAPH_H
#define DSP_GRAPH_H

#include <stdint.h>

#define DSP_MAX_INPUTS      6
#define DSP_MAX_NODES       256
#define DSP_MAX_OUTPUTS     8
#define DSP_MAX_FB_BUSES    4
#define DSP_ERR_BUF         256
#define DSP_MAX_FUNCTIONS   32
#define DSP_FUNC_NAME_LEN   64
#define DSP_FUNC_BODY_LEN   1024
#define DSP_MAX_CALL_DEPTH  8

/* ---- node types ---- */

typedef enum {
    DSP_CONST,
    DSP_INPUT,
    /* binary math */
    DSP_ADD,
    DSP_SUB,
    DSP_MUL,
    DSP_DIV,
    DSP_POW,
    DSP_MIN,
    DSP_MAX_OP,
    DSP_MOD,
    /* ternary math */
    DSP_CLIP,
    DSP_MIX,
    /* unary math */
    DSP_NEG,
    DSP_ABS,
    DSP_SIN,
    DSP_COS,
    DSP_TANH,
    DSP_EXP,
    DSP_LOG,
    DSP_SQRT,
    DSP_WRAP,
    DSP_FOLD,
    DSP_FLOOR,
    DSP_CEIL,
    DSP_ROUND,
    DSP_SIGN,
    DSP_DB2A,
    DSP_A2DB,
    DSP_MTOF,
    DSP_FTOM,
    /* stateful generators */
    DSP_SINOSC,
    DSP_PHASOR,
    DSP_NOISE,
    DSP_PINK,
    DSP_DUST,
    DSP_TRI,
    DSP_SAW,
    DSP_PULSE,
    /* envelope */
    DSP_DECAY,
    /* filters */
    DSP_ONEPOLE,
    DSP_HP1,
    DSP_LAG,
    DSP_SLEW,
    DSP_SVFLP,
    DSP_SVFHP,
    DSP_SVFBP,
    DSP_BIQUAD,
    /* delay + utilities */
    DSP_DELAY,
    DSP_SAH,
    DSP_LATCH,
    /* feedback */
    DSP_FBREAD,
    DSP_FBWRITE,
} dsp_node_type;

/* ---- node struct ---- */

struct dsp_node;
typedef void (*dsp_pull_fn)(struct dsp_node* node, long n);

typedef struct dsp_node {
    dsp_node_type   type;
    dsp_pull_fn     pull;
    double*         out;            /* pre-allocated output buffer */
    struct dsp_node* in[DSP_MAX_INPUTS];
    int             n_inputs;
    int             visited;        /* per-block re-pull guard */

    union {
        double      constant;       /* DSP_CONST */
        int         inlet_idx;      /* DSP_INPUT */
        struct {
            double  phase;
            double  inv_sr;
        } osc;                      /* DSP_SINOSC, DSP_PHASOR, DSP_TRI, DSP_SAW, DSP_PULSE */
        uint64_t    rng_state;      /* DSP_NOISE */
        struct {
            double  b0, b1, b2;
            uint64_t rng;
        } pink;                     /* DSP_PINK */
        struct {
            double  threshold;
            uint64_t rng;
            double  inv_sr;
        } dust;                     /* DSP_DUST */
        struct {
            double  env;
            double  prev_trig;
            double  inv_sr;
        } decay;                    /* DSP_DECAY */
        struct {
            double  y1;
            double  inv_sr;
        } onepole;                  /* DSP_ONEPOLE, DSP_HP1, DSP_LAG */
        struct {
            double  y1;
            double  inv_sr;
        } slew;                     /* DSP_SLEW */
        struct {
            double  ic1eq;
            double  ic2eq;
            double  inv_sr;
        } svf;                      /* DSP_SVFLP, DSP_SVFHP, DSP_SVFBP */
        struct {
            double  w1, w2;
        } biquad;                   /* DSP_BIQUAD */
        struct {
            double* buf;
            long    buf_len;
            long    write_pos;
        } delay;                    /* DSP_DELAY */
        struct {
            double  held;
            double  prev_trig;
        } sah;                      /* DSP_SAH */
        struct {
            double  held;
        } latch;                    /* DSP_LATCH */
        struct {
            int     bus_idx;
        } fb;                       /* DSP_FBREAD, DSP_FBWRITE */
    } state;
} dsp_node;

/* ---- graph struct ---- */

typedef struct {
    dsp_node    nodes[DSP_MAX_NODES];
    int         node_count;
    dsp_node*   root[DSP_MAX_OUTPUTS];  /* one root per output channel */
    int         num_roots;              /* how many outputs the expression produces */
    double*     buffer_pool;            /* single alloc for all buffers */
    long        vector_size;
    double      sample_rate;
    double*     fb_bufs[DSP_MAX_FB_BUSES];   /* feedback bus buffers */
    dsp_node*   fb_writers[DSP_MAX_FB_BUSES]; /* fbwrite nodes (pulled explicitly) */
    int         fb_writer_count;
} dsp_graph;

/* ---- compile-time function table ---- */

typedef struct {
    char name[DSP_FUNC_NAME_LEN];
    char body[DSP_FUNC_BODY_LEN];
} dsp_function;

typedef struct {
    dsp_function funcs[DSP_MAX_FUNCTIONS];
    int          count;
} dsp_func_table;

/* ---- API ---- */

/*
 * Compile a stack-based DSP expression into a node graph.
 * Returns NULL on failure (error written to err buffer).
 * Caller must free with dsp_graph_free().
 */
dsp_graph* dsp_compile(const char* expr, double sr, long vs,
                       const dsp_func_table* funcs,
                       char* err, int errlen);

/*
 * Process one block through the graph.
 * ins/outs are Max signal buffer arrays. n = sampleframes.
 */
void dsp_graph_process(dsp_graph* g,
                       double** ins, long numins,
                       double** outs, long numouts, long n);

/*
 * Free a compiled graph and all its buffers.
 */
void dsp_graph_free(dsp_graph* g);

/*
 * Compile-time function table management.
 */
void dsp_func_table_init(dsp_func_table* ft);
int  dsp_func_define(dsp_func_table* ft, const char* name, const char* body,
                     char* err, int errlen);
int  dsp_func_undef(dsp_func_table* ft, const char* name);
void dsp_func_clear(dsp_func_table* ft);

/*
 * Load definitions from a text buffer (file contents).
 * Each non-blank, non-comment line must be: def <name> <body...>
 * Lines starting with '#' (after optional whitespace) are comments.
 * Returns number of defs loaded, or -1 on error (message in err).
 */
int  dsp_func_load_text(dsp_func_table* ft, const char* text,
                        char* err, int errlen);

#endif /* DSP_GRAPH_H */
