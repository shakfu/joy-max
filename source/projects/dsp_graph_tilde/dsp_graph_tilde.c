/*
 *  dsp_graph_tilde.c -- Pure compiled DSP graph external for Max/MSP
 *
 *  Compiles stack-based RPN expressions into a native C DSP node graph
 *  at message time. The audio thread pulls through the graph with zero
 *  allocation, zero interpretation, zero GC.
 *
 *  No Joy dependency. No interpreter. Just DSP.
 *
 *  Usage:
 *    [dsp_graph~ 1 1]               -- 1 in, 1 out
 *    440 sinosc 0.3 *               -- compiled sine oscillator
 *    in1 2000 0.707 svflp           -- SVF lowpass on input
 *    stop                           -- silence
 *
 *  Messages:
 *    <expr>    -- compile and run RPN expression (anything/list handler)
 *    stop      -- silence, free graph
 *    status    -- post info to Max console + info outlet
 */

#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"

#include "dsp_graph.h"

#define DG_MAX_IO       8
#define DG_CODE_BUF     4096
#define DG_ERR_BUF      256

/* ---------- object struct ---------- */

typedef struct _dsp_graph_tilde {
    t_pxobject  ob;

    long        num_inputs;
    long        num_outputs;
    double      sample_rate;
    long        vector_size;

    char        code[DG_CODE_BUF];
    int         active;

    dsp_graph*  graph;
    dsp_func_table funcs;

    void*       info_outlet;
    long        verbose;
} t_dsp_graph_tilde;

/* ---------- forward declarations ---------- */

static void* dg_new(t_symbol* s, long argc, t_atom* argv);
static void  dg_free(t_dsp_graph_tilde* x);
static void  dg_assist(t_dsp_graph_tilde* x, void* b, long m, long a, char* s);
static void  dg_dsp64(t_dsp_graph_tilde* x, t_object* dsp64, short* count,
                       double samplerate, long maxvectorsize, long flags);
static void  dg_perform64(t_dsp_graph_tilde* x, t_object* dsp64,
                           double** ins, long numins,
                           double** outs, long numouts,
                           long sampleframes, long flags, void* userparam);
static void  dg_anything(t_dsp_graph_tilde* x, t_symbol* s, long argc, t_atom* argv);
static void  dg_stop(t_dsp_graph_tilde* x);
static void  dg_status(t_dsp_graph_tilde* x);
static void  dg_def(t_dsp_graph_tilde* x, t_symbol* s, long argc, t_atom* argv);
static void  dg_undef(t_dsp_graph_tilde* x, t_symbol* s);
static void  dg_cleardef(t_dsp_graph_tilde* x);

/* ---------- class pointer ---------- */

static void* dg_class;

/* ---------- ext_main ---------- */

void ext_main(void* r)
{
    t_class* c = class_new("dsp_graph~",
                            (method)dg_new,
                            (method)dg_free,
                            (long)sizeof(t_dsp_graph_tilde),
                            0L, A_GIMME, 0);

    class_addmethod(c, (method)dg_dsp64,    "dsp64",    A_CANT, 0);
    class_addmethod(c, (method)dg_anything,  "anything", A_GIMME, 0);
    class_addmethod(c, (method)dg_anything,  "list",     A_GIMME, 0);
    class_addmethod(c, (method)dg_stop,      "stop",     0);
    class_addmethod(c, (method)dg_status,    "status",   0);
    class_addmethod(c, (method)dg_def,       "def",      A_GIMME, 0);
    class_addmethod(c, (method)dg_undef,     "undef",    A_SYM, 0);
    class_addmethod(c, (method)dg_cleardef,  "cleardef", 0);
    class_addmethod(c, (method)dg_assist,    "assist",   A_CANT, 0);

    CLASS_ATTR_LONG(c, "verbose", 0, t_dsp_graph_tilde, verbose);
    CLASS_ATTR_STYLE_LABEL(c, "verbose", 0, "onoff", "Verbose Mode");

    class_dspinit(c);
    class_register(CLASS_BOX, c);
    dg_class = c;
}

/* ---------- constructor / destructor ---------- */

static void* dg_new(t_symbol* s, long argc, t_atom* argv)
{
    (void)s;

    t_dsp_graph_tilde* x = (t_dsp_graph_tilde*)object_alloc(dg_class);
    if (!x)
        return NULL;

    x->num_inputs  = 1;
    x->num_outputs = 1;
    x->verbose     = 0;

    if (argc >= 1 && atom_gettype(argv) == A_LONG)
        x->num_inputs = CLAMP(atom_getlong(argv), 1, DG_MAX_IO);
    if (argc >= 2 && atom_gettype(argv + 1) == A_LONG)
        x->num_outputs = CLAMP(atom_getlong(argv + 1), 1, DG_MAX_IO);

    /* create DSP inlets (first inlet is automatic) */
    dsp_setup((t_pxobject*)x, (short)x->num_inputs);

    /* info outlet (non-signal, rightmost) */
    x->info_outlet = outlet_new((t_object*)x, NULL);

    /* signal outlets (right to left) */
    for (long i = 0; i < x->num_outputs; i++)
        outlet_new((t_object*)x, "signal");

    x->code[0]      = '\0';
    x->active        = 0;
    x->sample_rate   = 44100.0;
    x->vector_size   = 64;
    x->graph         = NULL;
    dsp_func_table_init(&x->funcs);

    return x;
}

static void dg_free(t_dsp_graph_tilde* x)
{
    dsp_free((t_pxobject*)x);
    if (x->graph)
        dsp_graph_free(x->graph);
}

/* ---------- assist ---------- */

static void dg_assist(t_dsp_graph_tilde* x, void* b, long m, long a, char* s)
{
    (void)b;

    if (m == ASSIST_INLET) {
        if (a == 0)
            sprintf(s, "(signal) Input %ld / DSP messages", a + 1);
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

static void dg_dsp64(t_dsp_graph_tilde* x, t_object* dsp64, short* count,
                      double samplerate, long maxvectorsize, long flags)
{
    (void)count;
    (void)flags;

    x->sample_rate = samplerate;
    x->vector_size = maxvectorsize;

    /* recompile graph if active (sample rate or vector size may have changed) */
    if (x->graph && x->code[0]) {
        char dsp_err[DSP_ERR_BUF];
        dsp_graph* new_graph = dsp_compile(x->code, samplerate,
                                           maxvectorsize, &x->funcs,
                                           dsp_err, DSP_ERR_BUF);
        if (new_graph) {
            dsp_graph_free(x->graph);
            x->graph = new_graph;
        } else {
            object_error((t_object*)x, "recompile failed: %s", dsp_err);
        }
    }

    if (x->verbose)
        object_post((t_object*)x, "dsp64 sr=%.0f vs=%ld in=%ld out=%ld",
                    samplerate, maxvectorsize, x->num_inputs, x->num_outputs);

    object_method(dsp64, gensym("dsp_add64"), x, dg_perform64, 0, NULL);
}

static void dg_perform64(t_dsp_graph_tilde* x, t_object* dsp64,
                          double** ins, long numins,
                          double** outs, long numouts,
                          long sampleframes, long flags, void* userparam)
{
    (void)dsp64;
    (void)flags;
    (void)userparam;

    if (x->graph) {
        dsp_graph_process(x->graph, ins, numins, outs, numouts, sampleframes);
        return;
    }

    /* silence when no graph */
    for (long ch = 0; ch < numouts; ch++)
        memset(outs[ch], 0, sampleframes * sizeof(double));
}

/* ---------- message handlers ---------- */

/*
 * Any message (other than stop/status) is treated as a DSP expression.
 * Compile it and swap in the new graph.
 */
static void dg_anything(t_dsp_graph_tilde* x, t_symbol* s, long argc, t_atom* argv)
{
    if (s == gensym(""))
        return;

    /* build atom array with selector prepended (unless it's "list") */
    t_atom atoms[DG_CODE_BUF / 4];
    int offset = 0;

    if (s != gensym("list")) {
        atom_setsym(atoms, s);
        offset = 1;
    }

    long max_atoms = (long)(DG_CODE_BUF / 4) - offset;
    long copy = argc < max_atoms ? argc : max_atoms;
    for (long i = 0; i < copy; i++)
        atoms[i + offset] = argv[i];

    long total = copy + offset;
    long textsize = 0;
    char* text = NULL;
    t_max_err err = atom_gettext(total, atoms, &textsize, &text,
                                  OBEX_UTIL_ATOM_GETTEXT_SYM_NO_QUOTE);
    if (err != MAX_ERR_NONE || !textsize || !text) {
        object_error((t_object*)x, "failed to convert message to text");
        if (text)
            sysmem_freeptr(text);
        return;
    }

    /* save expression for recompilation on sr/vs change */
    strncpy(x->code, text, DG_CODE_BUF - 1);
    x->code[DG_CODE_BUF - 1] = '\0';

    char dsp_err[DG_ERR_BUF];
    dsp_graph* new_graph = dsp_compile(text, x->sample_rate,
                                       x->vector_size, &x->funcs,
                                       dsp_err, DG_ERR_BUF);
    sysmem_freeptr(text);

    if (!new_graph) {
        object_error((t_object*)x, "compile error: %s", dsp_err);
        return;
    }

    /* swap in new graph */
    dsp_graph* old = x->graph;
    x->graph = new_graph;
    x->active = 1;
    if (old)
        dsp_graph_free(old);

    if (x->verbose)
        object_post((t_object*)x, "compiled (%d nodes)",
                    new_graph->node_count);
}

static void dg_stop(t_dsp_graph_tilde* x)
{
    if (x->graph) {
        dsp_graph_free(x->graph);
        x->graph = NULL;
    }
    x->active = 0;
    x->code[0] = '\0';
    if (x->verbose)
        object_post((t_object*)x, "stopped");
}

static void dg_status(t_dsp_graph_tilde* x)
{
    object_post((t_object*)x,
        "status: sr=%.0f vs=%ld in=%ld out=%ld nodes=%d active=%s funcs=%d",
        x->sample_rate, x->vector_size, x->num_inputs, x->num_outputs,
        x->graph ? x->graph->node_count : 0,
        x->active ? "yes" : "no",
        x->funcs.count);

    t_atom atoms[4];
    atom_setfloat(atoms + 0, (float)x->sample_rate);
    atom_setlong(atoms + 1, x->vector_size);
    atom_setlong(atoms + 2, x->num_inputs);
    atom_setlong(atoms + 3, x->num_outputs);
    outlet_anything(x->info_outlet, gensym("status"), 4, atoms);
}

/* ---------- function definition messages ---------- */

/*
 * def <name> <body...> -- define a compile-time function.
 * The first atom is the function name, the rest form the body.
 */
static void dg_def(t_dsp_graph_tilde* x, t_symbol* s, long argc, t_atom* argv)
{
    (void)s;

    if (argc < 2) {
        object_error((t_object*)x, "def requires a name and body");
        return;
    }

    /* first atom must be a symbol (function name) */
    if (atom_gettype(argv) != A_SYM) {
        object_error((t_object*)x, "def: first argument must be a name");
        return;
    }
    const char* name = atom_getsym(argv)->s_name;

    /* remaining atoms -> body text */
    long textsize = 0;
    char* text = NULL;
    t_max_err err = atom_gettext(argc - 1, argv + 1, &textsize, &text,
                                  OBEX_UTIL_ATOM_GETTEXT_SYM_NO_QUOTE);
    if (err != MAX_ERR_NONE || !textsize || !text) {
        object_error((t_object*)x, "def: failed to convert body to text");
        if (text) sysmem_freeptr(text);
        return;
    }

    char def_err[DG_ERR_BUF];
    if (!dsp_func_define(&x->funcs, name, text, def_err, DG_ERR_BUF))
        object_error((t_object*)x, "def: %s", def_err);
    else if (x->verbose)
        object_post((t_object*)x, "defined '%s' = %s", name, text);

    sysmem_freeptr(text);
}

static void dg_undef(t_dsp_graph_tilde* x, t_symbol* s)
{
    if (!dsp_func_undef(&x->funcs, s->s_name))
        object_error((t_object*)x, "undef: '%s' not found", s->s_name);
    else if (x->verbose)
        object_post((t_object*)x, "undefined '%s'", s->s_name);
}

static void dg_cleardef(t_dsp_graph_tilde* x)
{
    dsp_func_clear(&x->funcs);
    if (x->verbose)
        object_post((t_object*)x, "all functions cleared");
}
