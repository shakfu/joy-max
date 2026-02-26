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

#define DSP_MAX_INPUTS  4
#define DSP_MAX_NODES   256
#define DSP_ERR_BUF     256

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
    DSP_TRI,
    DSP_SAW,
    DSP_PULSE,
    /* filters */
    DSP_ONEPOLE,
    DSP_HP1,
    DSP_SVFLP,
    DSP_SVFHP,
    DSP_SVFBP,
    /* delay + utilities */
    DSP_DELAY,
    DSP_SAH,
    DSP_LATCH,
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
            double  y1;
            double  inv_sr;
        } onepole;                  /* DSP_ONEPOLE, DSP_HP1 */
        struct {
            double  ic1eq;
            double  ic2eq;
            double  inv_sr;
        } svf;                      /* DSP_SVFLP, DSP_SVFHP, DSP_SVFBP */
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
    } state;
} dsp_node;

/* ---- graph struct ---- */

typedef struct {
    dsp_node    nodes[DSP_MAX_NODES];
    int         node_count;
    dsp_node*   root;               /* output node */
    double*     buffer_pool;        /* single alloc for all buffers */
    long        vector_size;
    double      sample_rate;
} dsp_graph;

/* ---- API ---- */

/*
 * Compile a stack-based DSP expression into a node graph.
 * Returns NULL on failure (error written to err buffer).
 * Caller must free with dsp_graph_free().
 */
dsp_graph* dsp_compile(const char* expr, double sr, long vs,
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

#endif /* DSP_GRAPH_H */
