/*
 *  joy_tilde.c -- Joy language DSP external for Max/MSP
 *
 *  Joy's interpreter uses a GC-managed heap with per-operation
 *  allocation, making it fundamentally unsafe for audio threads.
 *  Instead, Joy evaluates on the message thread and produces a
 *  sample buffer. The audio callback reads from this buffer with
 *  no Joy calls.
 *
 *  Architecture:
 *    Message thread: Joy code runs here (perform, eval, bang)
 *      -> produces double* output buffers per channel
 *    Audio thread: perform64 reads from cached buffers
 *      -> zero allocation, zero interpretation, zero GC
 *
 *  The output buffer is a waveform that cycles continuously.
 *  Send new perform code or bang to recompute.
 *
 *  Two modes:
 *    perform <expr>       -- tries DSP compile first, falls back to Joy eval
 *    dsp <expr>           -- compile RPN expression to native DSP graph (no fallback)
 *    stop                 -- silence (both modes)
 *
 *  Usage:
 *    [joy~ 1 1]                                  -- 1 in, 1 out
 *    perform 0.0 6.283185 512 vlinspace [sin] map -- sin wavetable
 *    dsp 440 sinosc 0.3 *                         -- compiled sine osc
 *    dsp in1 0.5 *                                -- scale input signal
 *    stop                                         -- silence
 */

#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"

#include "joy/joy.h"
#include "joy_max_bridge.h"
#include "dsp_graph.h"

#include <stdatomic.h>

#define JOY_TILDE_MAX_IO        8
#define JOY_TILDE_CODE_BUF      4096
#define JOY_TILDE_ERR_BUF       256
#define JOY_TILDE_MAX_BUF_LEN   (1 << 20) /* 1M samples max per channel */

/* ---------- object struct ---------- */

typedef struct _joy_tilde {
    t_pxobject  ob;
    JoyContext* ctx;
    JoyIO       io;

    long        num_inputs;
    long        num_outputs;
    double      sample_rate;
    long        vector_size;

    char        perform_code[JOY_TILDE_CODE_BUF];
    int         has_perform;

    /*
     * Double-buffered output: Joy writes to back buffer on message thread,
     * then atomically swaps. Audio thread reads from front buffer.
     */
    double*     out_buf[2][JOY_TILDE_MAX_IO]; /* [buf_index][channel] */
    long        out_len[2];                    /* samples per channel */
    atomic_int  active_buf;                    /* which buffer audio reads */
    long        phase;                         /* playback position */

    /* DSP graph mode: compiled expression graph for real-time processing */
    dsp_graph*  graph;
    dsp_func_table funcs;

    void*       info_outlet;
    long        verbose;

    char        last_error[JOY_TILDE_ERR_BUF];
} t_joy_tilde;

/* ---------- forward declarations ---------- */

static void* joy_tilde_new(t_symbol* s, long argc, t_atom* argv);
static void  joy_tilde_free(t_joy_tilde* x);
static void  joy_tilde_assist(t_joy_tilde* x, void* b, long m, long a, char* s);
static void  joy_tilde_dsp64(t_joy_tilde* x, t_object* dsp64, short* count,
                              double samplerate, long maxvectorsize, long flags);
static void  joy_tilde_perform64(t_joy_tilde* x, t_object* dsp64,
                                  double** ins, long numins,
                                  double** outs, long numouts,
                                  long sampleframes, long flags, void* userparam);
static void  joy_tilde_anything(t_joy_tilde* x, t_symbol* s, long argc, t_atom* argv);
static void  joy_tilde_dsp_msg(t_joy_tilde* x, t_symbol* s, long argc, t_atom* argv);
static void  joy_tilde_bang(t_joy_tilde* x);
static void  joy_tilde_stop(t_joy_tilde* x);
static void  joy_tilde_clear(t_joy_tilde* x);
static void  joy_tilde_status(t_joy_tilde* x);
static void  joy_tilde_def(t_joy_tilde* x, t_symbol* s, long argc, t_atom* argv);
static void  joy_tilde_undef(t_joy_tilde* x, t_symbol* s);
static void  joy_tilde_cleardef(t_joy_tilde* x);
static void  joy_tilde_load(t_joy_tilde* x, t_symbol* s);
static void  joy_tilde_load_file(t_joy_tilde* x, const char* filename);

/* I/O callbacks */
static void  tilde_io_write_char(void* user_data, int ch);
static void  tilde_io_write_string(void* user_data, const char* s);
static void  tilde_io_on_error(void* user_data, JoyResult result,
                                const char* message, const char* file,
                                int line, int column);

/* helpers */
static void  joy_tilde_init_context(t_joy_tilde* x);
static void  joy_tilde_eval_perform(t_joy_tilde* x, const char* code);
static long  joy_tilde_read_stack_to_buffers(t_joy_tilde* x, int buf_idx);
static long  joy_tilde_read_list_to_buffer(pEnv env, Index list,
                                            double* buf, long max_len);

/* ---------- class pointer ---------- */

static void* joy_tilde_class;

/* ---------- ext_main ---------- */

void ext_main(void* r)
{
    t_class* c = class_new("joy~",
                           (method)joy_tilde_new,
                           (method)joy_tilde_free,
                           (long)sizeof(t_joy_tilde),
                           0L, A_GIMME, 0);

    class_addmethod(c, (method)joy_tilde_dsp64,    "dsp64",    A_CANT, 0);
    class_addmethod(c, (method)joy_tilde_anything,  "anything", A_GIMME, 0);
    class_addmethod(c, (method)joy_tilde_anything,  "list",     A_GIMME, 0);
    class_addmethod(c, (method)joy_tilde_dsp_msg,  "dsp",      A_GIMME, 0);
    class_addmethod(c, (method)joy_tilde_bang,      "bang",     0);
    class_addmethod(c, (method)joy_tilde_stop,      "stop",     0);
    class_addmethod(c, (method)joy_tilde_clear,     "clear",    0);
    class_addmethod(c, (method)joy_tilde_status,    "status",   0);
    class_addmethod(c, (method)joy_tilde_def,       "def",      A_GIMME, 0);
    class_addmethod(c, (method)joy_tilde_undef,     "undef",    A_SYM, 0);
    class_addmethod(c, (method)joy_tilde_cleardef,  "cleardef", 0);
    class_addmethod(c, (method)joy_tilde_load,      "load",     A_SYM, 0);
    class_addmethod(c, (method)joy_tilde_assist,    "assist",   A_CANT, 0);

    CLASS_ATTR_LONG(c, "verbose", 0, t_joy_tilde, verbose);
    CLASS_ATTR_STYLE_LABEL(c, "verbose", 0, "onoff", "Verbose Mode");

    class_dspinit(c);
    class_register(CLASS_BOX, c);
    joy_tilde_class = c;
}

/* ---------- constructor / destructor ---------- */

static void* joy_tilde_new(t_symbol* s, long argc, t_atom* argv)
{
    t_joy_tilde* x = (t_joy_tilde*)object_alloc(joy_tilde_class);
    if (!x)
        return NULL;

    x->num_inputs  = 1;
    x->num_outputs = 1;
    x->verbose     = 0;

    if (argc >= 1 && atom_gettype(argv) == A_LONG)
        x->num_inputs = CLAMP(atom_getlong(argv), 1, JOY_TILDE_MAX_IO);
    if (argc >= 2 && atom_gettype(argv + 1) == A_LONG)
        x->num_outputs = CLAMP(atom_getlong(argv + 1), 1, JOY_TILDE_MAX_IO);

    /* create DSP inlets (first inlet is automatic) */
    dsp_setup((t_pxobject*)x, (short)x->num_inputs);

    /* info outlet (non-signal, rightmost) */
    x->info_outlet = outlet_new((t_object*)x, NULL);

    /* signal outlets (right to left) */
    for (long i = 0; i < x->num_outputs; i++)
        outlet_new((t_object*)x, "signal");

    x->perform_code[0] = '\0';
    x->has_perform     = 0;
    x->sample_rate     = 44100.0;
    x->vector_size     = 64;
    x->last_error[0]   = '\0';
    x->phase           = 0;
    atomic_store(&x->active_buf, 0);

    /* zero buffer pointers */
    for (int b = 0; b < 2; b++) {
        x->out_len[b] = 0;
        for (int ch = 0; ch < JOY_TILDE_MAX_IO; ch++)
            x->out_buf[b][ch] = NULL;
    }

    x->graph = NULL;
    dsp_func_table_init(&x->funcs);

    joy_tilde_init_context(x);

    /* optional 3rd arg: load a .dsp definitions file */
    if (argc >= 3 && atom_gettype(argv + 2) == A_SYM)
        joy_tilde_load_file(x, atom_getsym(argv + 2)->s_name);

    return x;
}

static void joy_tilde_free(t_joy_tilde* x)
{
    dsp_free((t_pxobject*)x);
    if (x->graph)
        dsp_graph_free(x->graph);
    if (x->ctx)
        joy_destroy(x->ctx);
    for (int b = 0; b < 2; b++)
        for (int ch = 0; ch < JOY_TILDE_MAX_IO; ch++)
            if (x->out_buf[b][ch])
                sysmem_freeptr(x->out_buf[b][ch]);
}

/* ---------- assist ---------- */

static void joy_tilde_assist(t_joy_tilde* x, void* b, long m, long a, char* s)
{
    if (m == ASSIST_INLET) {
        if (a == 0)
            sprintf(s, "(signal) Input %ld / Joy messages", a + 1);
        else
            sprintf(s, "(signal) Input %ld", a + 1);
    } else {
        if (a < x->num_outputs)
            sprintf(s, "(signal) Output %ld", a + 1);
        else
            sprintf(s, "Info/status");
    }
}

/* ---------- DSP ---------- */

static void joy_tilde_dsp64(t_joy_tilde* x, t_object* dsp64, short* count,
                             double samplerate, long maxvectorsize, long flags)
{
    x->sample_rate = samplerate;
    x->vector_size = maxvectorsize;
    x->phase       = 0;

    /* recompile DSP graph if active (sample rate or vector size may have changed) */
    if (x->graph && x->perform_code[0]) {
        char dsp_err[DSP_ERR_BUF];
        dsp_graph* new_graph = dsp_compile(x->perform_code, samplerate,
                                           maxvectorsize, &x->funcs,
                                           dsp_err, DSP_ERR_BUF);
        if (new_graph) {
            dsp_graph_free(x->graph);
            x->graph = new_graph;
        } else {
            object_error((t_object*)x, "joy~: DSP recompile failed: %s", dsp_err);
        }
    }

    if (x->verbose)
        object_post((t_object*)x, "joy~: dsp64 sr=%.0f vs=%ld in=%ld out=%ld",
                    samplerate, maxvectorsize, x->num_inputs, x->num_outputs);

    object_method(dsp64, gensym("dsp_add64"), x, joy_tilde_perform64, 0, NULL);
}

/*
 * perform64 -- audio thread callback.
 *
 * Zero Joy calls. Just reads from the pre-computed output buffer,
 * cycling through it. The buffer was computed on the message thread.
 */
static void joy_tilde_perform64(t_joy_tilde* x, t_object* dsp64,
                                 double** ins, long numins,
                                 double** outs, long numouts,
                                 long sampleframes, long flags, void* userparam)
{
    (void)dsp64;
    (void)flags;
    (void)userparam;

    /* DSP graph mode: pull through compiled graph */
    if (x->graph) {
        dsp_graph_process(x->graph, ins, numins, outs, numouts, sampleframes);
        return;
    }

    (void)ins;
    (void)numins;

    int buf_idx = atomic_load(&x->active_buf);
    long buf_len = x->out_len[buf_idx];

    if (!x->has_perform || buf_len <= 0) {
        for (long ch = 0; ch < numouts; ch++)
            memset(outs[ch], 0, sampleframes * sizeof(double));
        return;
    }

    long phase = x->phase;

    for (long ch = 0; ch < numouts; ch++) {
        double* src = x->out_buf[buf_idx][ch];
        if (!src) {
            memset(outs[ch], 0, sampleframes * sizeof(double));
            continue;
        }

        long p = phase;
        for (long i = 0; i < sampleframes; i++) {
            outs[ch][i] = src[p];
            if (++p >= buf_len)
                p = 0;
        }
    }

    /* advance phase (wrap around buffer length) */
    phase = (phase + sampleframes) % buf_len;
    x->phase = phase;
}

/* ---------- message handlers ---------- */

static void joy_tilde_anything(t_joy_tilde* x, t_symbol* s, long argc, t_atom* argv)
{
    if (s == gensym(""))
        return;

    if (!x->ctx) {
        object_error((t_object*)x, "joy~: no interpreter context");
        return;
    }

    /* check for "perform" message */
    if (s == gensym("perform")) {
        if (argc < 1) {
            object_error((t_object*)x, "joy~: perform requires Joy code");
            return;
        }

        /* convert atoms to text */
        long textsize = 0;
        char* text = NULL;
        t_max_err err = atom_gettext(argc, argv, &textsize, &text,
                                      OBEX_UTIL_ATOM_GETTEXT_SYM_NO_QUOTE);
        if (err != MAX_ERR_NONE || !textsize || !text) {
            object_error((t_object*)x, "joy~: failed to convert atoms to text");
            if (text)
                sysmem_freeptr(text);
            return;
        }

        /* save for bang re-evaluation */
        strncpy(x->perform_code, text, JOY_TILDE_CODE_BUF - 1);
        x->perform_code[JOY_TILDE_CODE_BUF - 1] = '\0';

        /* try DSP compile first */
        char dsp_err[DSP_ERR_BUF];
        dsp_graph* new_graph = dsp_compile(text, x->sample_rate,
                                           x->vector_size, &x->funcs,
                                           dsp_err, DSP_ERR_BUF);
        if (new_graph) {
            /* DSP compile succeeded -- use graph mode */
            dsp_graph* old = x->graph;
            x->graph = new_graph;
            x->has_perform = 1;
            if (old)
                dsp_graph_free(old);
            if (x->verbose)
                object_post((t_object*)x, "joy~: perform -> DSP graph (%d nodes)",
                            new_graph->node_count);
            sysmem_freeptr(text);
            return;
        }

        /* DSP compile failed -- fall back to Joy eval (buffer mode) */
        if (x->graph) {
            dsp_graph_free(x->graph);
            x->graph = NULL;
        }

        if (x->verbose)
            object_post((t_object*)x,
                "joy~: perform -> Joy fallback (DSP: %s)", dsp_err);

        joy_tilde_eval_perform(x, text);
        sysmem_freeptr(text);
        return;
    }

    /* otherwise, evaluate as arbitrary Joy code (stays on stack) */
    t_atom atoms[JOY_TILDE_CODE_BUF / 4];
    int offset = 0;

    if (s != gensym("list")) {
        atom_setsym(atoms, s);
        offset = 1;
    }

    long max_atoms = (long)(JOY_TILDE_CODE_BUF / 4) - offset;
    long copy = argc < max_atoms ? argc : max_atoms;
    for (long i = 0; i < copy; i++)
        atoms[i + offset] = argv[i];

    long total = copy + offset;
    long textsize = 0;
    char* text = NULL;
    t_max_err err = atom_gettext(total, atoms, &textsize, &text,
                                  OBEX_UTIL_ATOM_GETTEXT_SYM_NO_QUOTE);
    if (err != MAX_ERR_NONE || !textsize || !text) {
        object_error((t_object*)x, "joy~: failed to convert atoms to text");
        if (text)
            sysmem_freeptr(text);
        return;
    }

    size_t len = strlen(text);
    if (len + 3 > JOY_TILDE_CODE_BUF) {
        object_error((t_object*)x, "joy~: code too long");
        sysmem_freeptr(text);
        return;
    }

    char code[JOY_TILDE_CODE_BUF];
    memcpy(code, text, len);
    code[len]     = ' ';
    code[len + 1] = '.';
    code[len + 2] = '\0';
    sysmem_freeptr(text);

    if (x->verbose)
        object_post((t_object*)x, "joy~ eval: %s", code);

    joy_eval_string_keep(x->ctx, code);
}

static void joy_tilde_bang(t_joy_tilde* x)
{
    if (x->perform_code[0] == '\0') {
        object_error((t_object*)x, "joy~: no perform code set");
        return;
    }

    /* in DSP graph mode, bang is a no-op (graph is stateful/continuous) */
    if (x->graph)
        return;

    joy_tilde_eval_perform(x, x->perform_code);
}

/*
 * dsp message -- compile an RPN expression to a native DSP graph.
 *
 * This is NOT Joy. It's a minimal postfix DSP language:
 *   numbers, in1..in8, arithmetic (+,-,*,/,pow,min,max,mod,clip,mix),
 *   comparisons (gt,lt,gte,lte,eq,neq),
 *   unary (neg,abs,sin,cos,tanh,exp,log,sqrt,wrap,fold,floor,ceil,round,sign,db2a,a2db,mtof,ftom),
 *   generators (sinosc,phasor,noise,pink,dust,tri,saw,pulse),
 *   envelopes (decay,line,ar), filters (onepole,hp1,lag,slew,svflp,svfhp,svfbp,svfnotch,biquad),
 *   delay (delay,delayf), utilities (sah,latch), stack (dup,swap,pop,over,rot),
 *   feedback (fbread1-4,fbwrite1-4), bindings (let), functions (def,undef,cleardef).
 *
 * The compiled graph runs in the audio thread with zero allocation.
 */
static void joy_tilde_dsp_msg(t_joy_tilde* x, t_symbol* s, long argc, t_atom* argv)
{
    (void)s;

    if (argc < 1) {
        object_error((t_object*)x, "joy~: dsp requires an expression");
        return;
    }

    long textsize = 0;
    char* text = NULL;
    t_max_err err = atom_gettext(argc, argv, &textsize, &text,
                                  OBEX_UTIL_ATOM_GETTEXT_SYM_NO_QUOTE);
    if (err != MAX_ERR_NONE || !textsize || !text) {
        object_error((t_object*)x, "joy~: failed to convert atoms to text");
        if (text)
            sysmem_freeptr(text);
        return;
    }

    /* save expression for recompilation on sr/vs change */
    strncpy(x->perform_code, text, JOY_TILDE_CODE_BUF - 1);
    x->perform_code[JOY_TILDE_CODE_BUF - 1] = '\0';

    char dsp_err[DSP_ERR_BUF];
    dsp_graph* new_graph = dsp_compile(text, x->sample_rate,
                                       x->vector_size, &x->funcs,
                                       dsp_err, DSP_ERR_BUF);
    sysmem_freeptr(text);

    if (!new_graph) {
        object_error((t_object*)x, "joy~: dsp compile error: %s", dsp_err);
        return;
    }

    /* swap in the new graph */
    dsp_graph* old = x->graph;
    x->graph = new_graph;
    x->has_perform = 1;
    if (old)
        dsp_graph_free(old);

    if (x->verbose)
        object_post((t_object*)x, "joy~: DSP graph compiled (%d nodes)",
                    new_graph->node_count);
}

static void joy_tilde_stop(t_joy_tilde* x)
{
    if (x->graph) {
        dsp_graph_free(x->graph);
        x->graph = NULL;
    }
    x->has_perform = 0;
    x->perform_code[0] = '\0';
    x->phase = 0;
    if (x->verbose)
        object_post((t_object*)x, "joy~: stopped (output silence)");
}

static void joy_tilde_clear(t_joy_tilde* x)
{
    if (x->ctx)
        joy_stack_clear(x->ctx);
    if (x->verbose)
        object_post((t_object*)x, "joy~: stack cleared");
}

static void joy_tilde_status(t_joy_tilde* x)
{
    int buf_idx = atomic_load(&x->active_buf);
    object_post((t_object*)x,
        "joy~ status: sr=%.0f vs=%ld in=%ld out=%ld buf=%ld mode=%s perform=%s",
        x->sample_rate, x->vector_size, x->num_inputs, x->num_outputs,
        x->out_len[buf_idx],
        x->graph ? "dsp-graph" : "buffer",
        x->has_perform ? "active" : "(none)");

    if (x->ctx) {
        object_post((t_object*)x,
            "joy~ memory: used=%zu max=%zu gc=%zu",
            joy_memory_used(x->ctx), joy_memory_max(x->ctx), joy_gc_count(x->ctx));
    }

    t_atom atoms[4];
    atom_setfloat(atoms + 0, (float)x->sample_rate);
    atom_setlong(atoms + 1, x->vector_size);
    atom_setlong(atoms + 2, x->num_inputs);
    atom_setlong(atoms + 3, x->num_outputs);
    outlet_anything(x->info_outlet, gensym("status"), 4, atoms);
}

/* ---------- function definition messages ---------- */

static void joy_tilde_def(t_joy_tilde* x, t_symbol* s, long argc, t_atom* argv)
{
    (void)s;

    if (argc < 2) {
        object_error((t_object*)x, "joy~: def requires a name and body");
        return;
    }

    if (atom_gettype(argv) != A_SYM) {
        object_error((t_object*)x, "joy~: def: first argument must be a name");
        return;
    }
    const char* name = atom_getsym(argv)->s_name;

    long textsize = 0;
    char* text = NULL;
    t_max_err err = atom_gettext(argc - 1, argv + 1, &textsize, &text,
                                  OBEX_UTIL_ATOM_GETTEXT_SYM_NO_QUOTE);
    if (err != MAX_ERR_NONE || !textsize || !text) {
        object_error((t_object*)x, "joy~: def: failed to convert body to text");
        if (text) sysmem_freeptr(text);
        return;
    }

    char def_err[JOY_TILDE_ERR_BUF];
    if (!dsp_func_define(&x->funcs, name, text, def_err, JOY_TILDE_ERR_BUF))
        object_error((t_object*)x, "joy~: def: %s", def_err);
    else if (x->verbose)
        object_post((t_object*)x, "joy~: defined '%s' = %s", name, text);

    sysmem_freeptr(text);
}

static void joy_tilde_undef(t_joy_tilde* x, t_symbol* s)
{
    if (!dsp_func_undef(&x->funcs, s->s_name))
        object_error((t_object*)x, "joy~: undef: '%s' not found", s->s_name);
    else if (x->verbose)
        object_post((t_object*)x, "joy~: undefined '%s'", s->s_name);
}

static void joy_tilde_cleardef(t_joy_tilde* x)
{
    dsp_func_clear(&x->funcs);
    if (x->verbose)
        object_post((t_object*)x, "joy~: all functions cleared");
}

/* ---------- I/O callbacks ---------- */

static void tilde_io_write_char(void* user_data, int ch)
{
    (void)user_data;
    (void)ch;
}

static void tilde_io_write_string(void* user_data, const char* s)
{
    (void)user_data;
    (void)s;
}

static void tilde_io_on_error(void* user_data, JoyResult result,
                               const char* message, const char* file,
                               int line, int column)
{
    t_joy_tilde* x = (t_joy_tilde*)user_data;
    (void)file;
    (void)result;

    object_error((t_object*)x, "joy~ error [%d:%d]: %s",
                 line, column, message ? message : "(null)");

    if (message) {
        strncpy(x->last_error, message, JOY_TILDE_ERR_BUF - 1);
        x->last_error[JOY_TILDE_ERR_BUF - 1] = '\0';
    }
}

/* ---------- context initialization ---------- */

static void joy_tilde_init_context(t_joy_tilde* x)
{
    x->io.user_data    = x;
    x->io.read_char    = NULL;
    x->io.write_char   = tilde_io_write_char;
    x->io.write_string = tilde_io_write_string;
    x->io.on_error     = tilde_io_on_error;

    JoyConfig config;
    memset(&config, 0, sizeof(config));
    config.enable_autoput = 0;
    config.enable_echo    = 0;
    config.io             = &x->io;

    x->ctx = joy_create(&config);
    if (!x->ctx) {
        object_error((t_object*)x, "joy~: failed to create interpreter context");
        return;
    }

    pEnv env = joy_get_env(x->ctx);
    env->config.undeferror = 1;
}

/* ---------- core: evaluate perform code ---------- */

/*
 * Evaluate Joy code on the message thread, read result into back buffer,
 * then atomically swap so the audio thread picks it up.
 *
 * The code runs with the current stack state. Input signals are NOT
 * available here (this runs on the message thread, not per-block).
 * Use Joy to generate waveforms, wavetables, or computed buffers.
 */
static void joy_tilde_eval_perform(t_joy_tilde* x, const char* code)
{
    if (!x->ctx)
        return;

    /* clear stack before evaluation so result is clean */
    pEnv env = joy_get_env(x->ctx);
    env->stck = 0;

    /* build terminated code string */
    char buf[JOY_TILDE_CODE_BUF];
    size_t len = strlen(code);
    if (len + 3 > sizeof(buf)) {
        object_error((t_object*)x, "joy~: code too long");
        return;
    }
    memcpy(buf, code, len);
    buf[len]     = ' ';
    buf[len + 1] = '.';
    buf[len + 2] = '\0';

    JoyResult result = joy_eval_string_keep(x->ctx, buf);
    if (result != JOY_OK) {
        object_error((t_object*)x, "joy~: evaluation failed: %s",
                     joy_result_string(result));
        return;
    }

    /* determine which buffer to write (the inactive one) */
    int active = atomic_load(&x->active_buf);
    int back = 1 - active;

    /* read results from stack into back buffer */
    long n = joy_tilde_read_stack_to_buffers(x, back);
    if (n <= 0) {
        object_error((t_object*)x,
            "joy~: no vector/list output on stack (got 0 samples)");
        return;
    }

    /* swap: audio thread will pick up the new buffer next block */
    atomic_store(&x->active_buf, back);
    x->phase = 0;
    x->has_perform = 1;

    if (x->verbose)
        object_post((t_object*)x, "joy~: buffer ready, %ld samples x %ld ch",
                    n, x->num_outputs);
}

/*
 * Read output channels from Joy stack into the specified buffer set.
 * Top of stack = last output channel (or the only channel).
 * Returns the number of samples per channel, or 0 on failure.
 */
static long joy_tilde_read_stack_to_buffers(t_joy_tilde* x, int buf_idx)
{
    pEnv env = joy_get_env(x->ctx);
    long result_len = 0;

    for (long ch = x->num_outputs - 1; ch >= 0; ch--) {
        if (!env->stck) {
            object_error((t_object*)x,
                "joy~: not enough values on stack for %ld outputs", x->num_outputs);
            /* fill remaining channels with silence */
            for (long c = ch; c >= 0; c--) {
                if (!x->out_buf[buf_idx][c]) {
                    x->out_buf[buf_idx][c] = (double*)sysmem_newptr(
                        x->vector_size * sizeof(double));
                }
                if (x->out_buf[buf_idx][c])
                    memset(x->out_buf[buf_idx][c], 0,
                           x->vector_size * sizeof(double));
            }
            if (result_len == 0)
                result_len = x->vector_size;
            break;
        }

        Index top = env->stck;
        int typ = nodetype(top);
        long n = 0;

        if (typ == LIST_) {
            /* count list length first */
            Index node = nodevalue(top).lis;
            long count = 0;
            Index tmp = node;
            while (tmp && count < JOY_TILDE_MAX_BUF_LEN) {
                count++;
                POP(tmp);
            }

            /* allocate or resize buffer */
            if (x->out_buf[buf_idx][ch])
                sysmem_freeptr(x->out_buf[buf_idx][ch]);
            x->out_buf[buf_idx][ch] = (double*)sysmem_newptr(count * sizeof(double));
            if (!x->out_buf[buf_idx][ch]) {
                object_error((t_object*)x, "joy~: allocation failed");
                return 0;
            }

            n = joy_tilde_read_list_to_buffer(env, node,
                                               x->out_buf[buf_idx][ch], count);

        } else if (typ == VECTOR_) {
            VectorData* v = nodevalue(top).vec;
            if (!v || v->len <= 0) {
                object_error((t_object*)x, "joy~: empty vector on stack");
                env->stck = nextnode1(env->stck);
                continue;
            }
            n = v->len;
            if (x->out_buf[buf_idx][ch])
                sysmem_freeptr(x->out_buf[buf_idx][ch]);
            x->out_buf[buf_idx][ch] = (double*)sysmem_newptr(n * sizeof(double));
            if (!x->out_buf[buf_idx][ch]) {
                object_error((t_object*)x, "joy~: allocation failed");
                return 0;
            }
            memcpy(x->out_buf[buf_idx][ch], v->data, n * sizeof(double));

        } else if (typ == FLOAT_ || typ == INTEGER_) {
            /* scalar: fill one vector_size block */
            double val = (typ == FLOAT_) ? nodevalue(top).dbl
                                         : (double)nodevalue(top).num;
            n = x->vector_size;
            if (x->out_buf[buf_idx][ch])
                sysmem_freeptr(x->out_buf[buf_idx][ch]);
            x->out_buf[buf_idx][ch] = (double*)sysmem_newptr(n * sizeof(double));
            if (!x->out_buf[buf_idx][ch]) {
                object_error((t_object*)x, "joy~: allocation failed");
                return 0;
            }
            for (long i = 0; i < n; i++)
                x->out_buf[buf_idx][ch][i] = val;

        } else {
            object_error((t_object*)x,
                "joy~: unexpected type %d on stack (expected list/vector/number)", typ);
            env->stck = nextnode1(env->stck);
            continue;
        }

        if (result_len == 0)
            result_len = n;
        else if (n != result_len)
            object_post((t_object*)x,
                "joy~: channel %ld has %ld samples (expected %ld)", ch, n, result_len);

        env->stck = nextnode1(env->stck);
    }

    x->out_len[buf_idx] = result_len;
    return result_len;
}

/* ---------- helpers ---------- */

/*
 * Read a linked list of numeric nodes into a double buffer.
 * Returns the number of samples read.
 */
static long joy_tilde_read_list_to_buffer(pEnv env, Index list,
                                           double* buf, long max_len)
{
    long i = 0;
    Index node = list;
    while (node && i < max_len) {
        int typ = nodetype(node);
        if (typ == FLOAT_)
            buf[i] = nodevalue(node).dbl;
        else if (typ == INTEGER_)
            buf[i] = (double)nodevalue(node).num;
        else
            buf[i] = 0.0;
        i++;
        POP(node);
    }
    return i;
}

/* ---------- file loading ---------- */

static void joy_tilde_load_file(t_joy_tilde* x, const char* filename)
{
    short path;
    char fullname[MAX_PATH_CHARS];
    t_fourcc type;

    strncpy(fullname, filename, MAX_PATH_CHARS - 1);
    fullname[MAX_PATH_CHARS - 1] = '\0';
    if (locatefile_extended(fullname, &path, &type, NULL, 0)) {
        object_error((t_object*)x, "joy~: file not found: %s", filename);
        return;
    }

    t_filehandle fh;
    if (path_opensysfile(fullname, path, &fh, READ_PERM)) {
        object_error((t_object*)x, "joy~: cannot open: %s", filename);
        return;
    }

    t_ptr_size size;
    sysfile_geteof(fh, &size);
    char* buf = sysmem_newptr(size + 1);
    if (!buf) {
        sysfile_close(fh);
        object_error((t_object*)x, "joy~: allocation failed for: %s", filename);
        return;
    }
    sysfile_read(fh, &size, buf);
    buf[size] = '\0';
    sysfile_close(fh);

    char err[JOY_TILDE_ERR_BUF];
    int n = dsp_func_load_text(&x->funcs, buf, err, JOY_TILDE_ERR_BUF);
    sysmem_freeptr(buf);

    if (n < 0)
        object_error((t_object*)x, "joy~: load error: %s", err);
    else
        object_post((t_object*)x, "joy~: loaded %d definitions from %s", n, filename);
}

static void joy_tilde_load(t_joy_tilde* x, t_symbol* s)
{
    joy_tilde_load_file(x, s->s_name);
}
