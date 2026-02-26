/*
 *  joy_object.c -- Joy language interpreter for Max
 *
 *  A non-DSP Max external that evaluates Joy code and outputs
 *  results as Max atoms. Embeds the Joy interpreter via the
 *  public C API (joy/joy.h).
 */

#include "ext.h"
#include "ext_obex.h"

#include "joy/joy.h"
#include "joy_max_bridge.h"

#define JOY_MAX_OUTPUT_BUF  4096
#define JOY_MAX_CODE_BUF    4096
#define JOY_MAX_ATOMS       1024

/* ---------- object struct ---------- */

typedef struct _joy_obj {
    t_object    ob;
    JoyContext* ctx;
    JoyIO       io;
    char*       output_buf;
    size_t      output_len;
    size_t      output_cap;
    char        last_code[JOY_MAX_CODE_BUF];
    void*       result_outlet;
    void*       error_outlet;
    long        verbose;
} t_joy_obj;

/* ---------- forward declarations ---------- */

static void* joy_obj_new(t_symbol* s, long argc, t_atom* argv);
static void  joy_obj_free(t_joy_obj* x);
static void  joy_obj_assist(t_joy_obj* x, void* b, long m, long a, char* s);
static void  joy_obj_anything(t_joy_obj* x, t_symbol* s, long argc, t_atom* argv);
static void  joy_obj_bang(t_joy_obj* x);
static void  joy_obj_clear(t_joy_obj* x);
static void  joy_obj_stack(t_joy_obj* x);
static void  joy_obj_reset(t_joy_obj* x);

/* I/O callbacks */
static void  io_write_char(void* user_data, int ch);
static void  io_write_string(void* user_data, const char* s);
static void  io_on_error(void* user_data, JoyResult result,
                          const char* message, const char* file,
                          int line, int column);

/* helpers */
static void  output_buf_reset(t_joy_obj* x);
static void  output_buf_append_char(t_joy_obj* x, int ch);
static void  output_buf_append_str(t_joy_obj* x, const char* s);
static void  joy_obj_init_context(t_joy_obj* x);
static void  joy_obj_eval(t_joy_obj* x, const char* code);
static void  joy_obj_eval_atoms(t_joy_obj* x, long argc, t_atom* argv);
static int   joy_node_to_atoms(t_joy_obj* x, pEnv env, Index n, t_atom* atoms, int max_atoms);
static void  joy_obj_output_stack_top(t_joy_obj* x);
static void  joy_obj_output_stack(t_joy_obj* x);

/* ---------- class pointer ---------- */

static void* joy_obj_class;

/* ---------- ext_main ---------- */

void ext_main(void* r)
{
    t_class* c = class_new("joy",
                           (method)joy_obj_new,
                           (method)joy_obj_free,
                           (long)sizeof(t_joy_obj),
                           0L, A_GIMME, 0);

    class_addmethod(c, (method)joy_obj_anything, "anything", A_GIMME, 0);
    class_addmethod(c, (method)joy_obj_anything, "list",    A_GIMME, 0);
    class_addmethod(c, (method)joy_obj_bang,     "bang",     0);
    class_addmethod(c, (method)joy_obj_clear,    "clear",    0);
    class_addmethod(c, (method)joy_obj_stack,    "stack",    0);
    class_addmethod(c, (method)joy_obj_reset,    "reset",    0);
    class_addmethod(c, (method)joy_obj_assist,   "assist",   A_CANT, 0);

    CLASS_ATTR_LONG(c, "verbose", 0, t_joy_obj, verbose);
    CLASS_ATTR_STYLE_LABEL(c, "verbose", 0, "onoff", "Verbose Mode");

    class_register(CLASS_BOX, c);
    joy_obj_class = c;
}

/* ---------- constructor / destructor ---------- */

static void* joy_obj_new(t_symbol* s, long argc, t_atom* argv)
{
    t_joy_obj* x = (t_joy_obj*)object_alloc(joy_obj_class);
    if (!x)
        return NULL;

    /* create outlets right-to-left */
    x->error_outlet  = outlet_new((t_object*)x, NULL);
    x->result_outlet = outlet_new((t_object*)x, NULL);

    x->output_cap = JOY_MAX_OUTPUT_BUF;
    x->output_buf = (char*)sysmem_newptr(x->output_cap);
    x->output_len = 0;
    x->last_code[0] = '\0';
    x->verbose = 0;

    /* process attributes */
    attr_args_process(x, argc, argv);

    joy_obj_init_context(x);

    if (x->verbose)
        object_post((t_object*)x, "joy: %s", joy_version());

    return x;
}

static void joy_obj_free(t_joy_obj* x)
{
    if (x->ctx)
        joy_destroy(x->ctx);
    if (x->output_buf)
        sysmem_freeptr(x->output_buf);
}

/* ---------- assist ---------- */

static void joy_obj_assist(t_joy_obj* x, void* b, long m, long a, char* s)
{
    (void)x;
    if (m == ASSIST_INLET) {
        sprintf(s, "Joy code (symbol/list)");
    } else {
        switch (a) {
        case 0: sprintf(s, "Result (atoms)"); break;
        case 1: sprintf(s, "Error/status (symbol)"); break;
        }
    }
}

/* ---------- message handlers ---------- */

static void joy_obj_anything(t_joy_obj* x, t_symbol* s, long argc, t_atom* argv)
{
    t_atom atoms[JOY_MAX_ATOMS];
    int offset = 0;

    if (s == gensym(""))
        return;

    if (s != gensym("list")) {
        /* prepend selector symbol as first atom */
        atom_setsym(atoms, s);
        offset = 1;
    }

    for (long i = 0; i < argc && (i + offset) < JOY_MAX_ATOMS; i++) {
        switch (atom_gettype(argv + i)) {
        case A_LONG:
            atom_setlong(atoms + i + offset, atom_getlong(argv + i));
            break;
        case A_FLOAT:
            atom_setfloat(atoms + i + offset, atom_getfloat(argv + i));
            break;
        case A_SYM:
            atom_setsym(atoms + i + offset, atom_getsym(argv + i));
            break;
        default:
            break;
        }
    }

    joy_obj_eval_atoms(x, argc + offset, atoms);
}

static void joy_obj_bang(t_joy_obj* x)
{
    if (x->last_code[0] == '\0') {
        object_error((t_object*)x, "joy: no code to re-evaluate");
        return;
    }
    joy_obj_eval(x, x->last_code);
}

static void joy_obj_clear(t_joy_obj* x)
{
    if (x->ctx)
        joy_stack_clear(x->ctx);
    if (x->verbose)
        object_post((t_object*)x, "joy: stack cleared");
}

static void joy_obj_stack(t_joy_obj* x)
{
    joy_obj_output_stack(x);
}

static void joy_obj_reset(t_joy_obj* x)
{
    if (x->ctx)
        joy_destroy(x->ctx);
    x->ctx = NULL;
    joy_obj_init_context(x);
    if (x->verbose)
        object_post((t_object*)x, "joy: context reset");
}

/* ---------- I/O callbacks ---------- */

static void io_write_char(void* user_data, int ch)
{
    t_joy_obj* x = (t_joy_obj*)user_data;
    output_buf_append_char(x, ch);
}

static void io_write_string(void* user_data, const char* s)
{
    t_joy_obj* x = (t_joy_obj*)user_data;
    output_buf_append_str(x, s);
}

static void io_on_error(void* user_data, JoyResult result,
                         const char* message, const char* file,
                         int line, int column)
{
    t_joy_obj* x = (t_joy_obj*)user_data;
    (void)file;

    object_error((t_object*)x, "joy error [%d:%d]: %s",
                 line, column, message ? message : "(null)");

    if (x->error_outlet) {
        t_atom a;
        atom_setsym(&a, gensym(message ? (char*)message : "error"));
        outlet_anything(x->error_outlet, gensym("error"), 1, &a);
    }
}

/* ---------- output buffer ---------- */

static void output_buf_reset(t_joy_obj* x)
{
    x->output_len = 0;
    if (x->output_buf)
        x->output_buf[0] = '\0';
}

static void output_buf_append_char(t_joy_obj* x, int ch)
{
    if (x->output_len + 2 > x->output_cap)
        return;
    x->output_buf[x->output_len++] = (char)ch;
    x->output_buf[x->output_len] = '\0';
}

static void output_buf_append_str(t_joy_obj* x, const char* s)
{
    if (!s) return;
    size_t len = strlen(s);
    if (x->output_len + len + 1 > x->output_cap)
        return;
    memcpy(x->output_buf + x->output_len, s, len);
    x->output_len += len;
    x->output_buf[x->output_len] = '\0';
}

/* ---------- context initialization ---------- */

static void joy_obj_init_context(t_joy_obj* x)
{
    x->io.user_data    = x;
    x->io.read_char    = NULL;
    x->io.write_char   = io_write_char;
    x->io.write_string = io_write_string;
    x->io.on_error     = io_on_error;

    JoyConfig config;
    memset(&config, 0, sizeof(config));
    config.enable_autoput = 0;
    config.enable_echo    = 0;
    config.io             = &x->io;

    x->ctx = joy_create(&config);
    if (!x->ctx) {
        object_error((t_object*)x, "joy: failed to create interpreter context");
        return;
    }

    /* enable error reporting for undefined symbols */
    pEnv env = joy_get_env(x->ctx);
    env->config.undeferror = 1;
}

/* ---------- evaluation ---------- */

/*
 * Convert an atom array to a Joy code string and evaluate it.
 * Uses Max's atom_gettext for robust atom-to-text serialization.
 */
static void joy_obj_eval_atoms(t_joy_obj* x, long argc, t_atom* argv)
{
    long textsize = 0;
    char* text = NULL;

    t_max_err err = atom_gettext(argc, argv, &textsize, &text,
                                 OBEX_UTIL_ATOM_GETTEXT_SYM_NO_QUOTE);
    if (err != MAX_ERR_NONE || !textsize || !text) {
        object_error((t_object*)x, "joy: failed to convert atoms to text");
        if (text)
            sysmem_freeptr(text);
        return;
    }

    /* build Joy code: text + " ." terminator */
    size_t len = strlen(text);
    if (len + 3 > JOY_MAX_CODE_BUF) {
        object_error((t_object*)x, "joy: code too long");
        sysmem_freeptr(text);
        return;
    }

    char code[JOY_MAX_CODE_BUF];
    memcpy(code, text, len);
    code[len]     = ' ';
    code[len + 1] = '.';
    code[len + 2] = '\0';
    sysmem_freeptr(text);

    /* save for bang replay */
    strncpy(x->last_code, code, JOY_MAX_CODE_BUF - 1);
    x->last_code[JOY_MAX_CODE_BUF - 1] = '\0';

    joy_obj_eval(x, code);
}

static void joy_obj_eval(t_joy_obj* x, const char* code)
{
    if (!x->ctx) {
        object_error((t_object*)x, "joy: no interpreter context");
        return;
    }

    output_buf_reset(x);

    if (x->verbose)
        object_post((t_object*)x, "joy eval: %s", code);

    JoyResult result = joy_eval_string_keep(x->ctx, code);

    if (x->verbose)
        object_post((t_object*)x, "joy result: %s, stack depth: %zu",
                    joy_result_string(result), joy_stack_depth(x->ctx));

    /* if Joy printed output via callbacks, send as symbol */
    if (x->output_len > 0) {
        /* trim trailing newline */
        while (x->output_len > 0 &&
               (x->output_buf[x->output_len - 1] == '\n' ||
                x->output_buf[x->output_len - 1] == '\r')) {
            x->output_buf[--x->output_len] = '\0';
        }
        if (x->output_len > 0) {
            outlet_anything(x->result_outlet, gensym(x->output_buf), 0, NULL);
        }
    }

    if (result != JOY_OK && result != JOY_ERROR_QUIT) {
        object_error((t_object*)x, "joy: eval returned %s", joy_result_string(result));
        return;
    }

    /* output stack top as atoms */
    if (!joy_stack_empty(x->ctx)) {
        joy_obj_output_stack_top(x);
    }
}

/* ---------- stack -> atom conversion ---------- */

/*
 * Convert a single Joy node to Max atoms.
 * Returns number of atoms written, or -1 on error.
 */
static int joy_node_to_atoms(t_joy_obj* x, pEnv env, Index n, t_atom* atoms, int max_atoms)
{
    if (!n || max_atoms <= 0)
        return 0;

    switch (nodetype(n)) {
    case INTEGER_:
        atom_setlong(atoms, (t_atom_long)nodevalue(n).num);
        return 1;

    case FLOAT_:
        atom_setfloat(atoms, (float)nodevalue(n).dbl);
        return 1;

    case BOOLEAN_:
        atom_setlong(atoms, nodevalue(n).num ? 1 : 0);
        return 1;

    case STRING_: {
        char* s = GETSTRING(n);
        atom_setsym(atoms, gensym(s));
        return 1;
    }

    case LIST_: {
        /* flatten list into atoms */
        int count = 0;
        Index elem = nodevalue(n).lis;
        while (elem && count < max_atoms) {
            int wrote = joy_node_to_atoms(x, env, elem, atoms + count, max_atoms - count);
            if (wrote < 0)
                return -1;
            count += wrote;
            POP(elem);
        }
        return count;
    }

#ifdef JOY_NATIVE_TYPES
    case VECTOR_: {
        VectorData* v = nodevalue(n).vec;
        if (!v) return 0;
        int count = v->len < max_atoms ? v->len : max_atoms;
        for (int i = 0; i < count; i++) {
            atom_setfloat(atoms + i, (float)v->data[i]);
        }
        return count;
    }
#endif

    case CHAR_:
        atom_setlong(atoms, (t_atom_long)nodevalue(n).num);
        return 1;

    case SET_:
        atom_setlong(atoms, (t_atom_long)nodevalue(n).set);
        return 1;

    case USR_:
        atom_setsym(atoms, gensym(vec_at(env->symtab, nodevalue(n).ent).name));
        return 1;

    case ANON_FUNCT_:
        atom_setsym(atoms, gensym(opername(operindex(env, nodevalue(n).proc))));
        return 1;

    default: {
        /* unsupported type - include type number for debugging */
        char tbuf[64];
        snprintf(tbuf, sizeof(tbuf), "<type-%d>", nodetype(n));
        atom_setsym(atoms, gensym(tbuf));
        return 1;
    }
    }
}

/*
 * Output the top of the Joy stack as Max atoms.
 */
static void joy_obj_output_stack_top(t_joy_obj* x)
{
    if (!x->ctx || joy_stack_empty(x->ctx))
        return;

    pEnv env = joy_get_env(x->ctx);
    Index top = env->stck;

    t_atom atoms[JOY_MAX_ATOMS];
    int count = joy_node_to_atoms(x, env, top, atoms, JOY_MAX_ATOMS);

    if (count > 0) {
        if (count == 1) {
            /* single value: output as appropriate type */
            switch (atom_gettype(atoms)) {
            case A_LONG:
                outlet_int(x->result_outlet, atom_getlong(atoms));
                break;
            case A_FLOAT:
                outlet_float(x->result_outlet, atom_getfloat(atoms));
                break;
            case A_SYM:
                outlet_anything(x->result_outlet, atom_getsym(atoms), 0, NULL);
                break;
            default:
                outlet_list(x->result_outlet, NULL, count, atoms);
                break;
            }
        } else {
            outlet_list(x->result_outlet, NULL, count, atoms);
        }
    }
}

/*
 * Output the full Joy stack as Max atoms (bottom to top).
 */
static void joy_obj_output_stack(t_joy_obj* x)
{
    if (!x->ctx || joy_stack_empty(x->ctx)) {
        if (x->verbose)
            object_post((t_object*)x, "joy: stack is empty");
        return;
    }

    pEnv env = joy_get_env(x->ctx);

    /* count stack depth */
    size_t depth = joy_stack_depth(x->ctx);

    /* collect stack nodes in reverse (bottom first) */
    Index nodes[JOY_MAX_ATOMS];
    size_t n = 0;
    Index curr = env->stck;
    while (curr && n < JOY_MAX_ATOMS) {
        nodes[n++] = curr;
        POP(curr);
    }

    /* output each element from bottom to top */
    t_atom atoms[JOY_MAX_ATOMS];
    for (size_t i = n; i > 0; i--) {
        int count = joy_node_to_atoms(x, env, nodes[i - 1], atoms, JOY_MAX_ATOMS);
        if (count > 0) {
            if (count == 1 && atom_gettype(atoms) == A_LONG) {
                outlet_int(x->result_outlet, atom_getlong(atoms));
            } else if (count == 1 && atom_gettype(atoms) == A_FLOAT) {
                outlet_float(x->result_outlet, atom_getfloat(atoms));
            } else if (count == 1 && atom_gettype(atoms) == A_SYM) {
                outlet_anything(x->result_outlet, atom_getsym(atoms), 0, NULL);
            } else {
                outlet_list(x->result_outlet, NULL, count, atoms);
            }
        }
    }

    /* send depth to error outlet as status */
    t_atom depth_atom;
    atom_setlong(&depth_atom, (t_atom_long)depth);
    outlet_anything(x->error_outlet, gensym("depth"), 1, &depth_atom);
}
