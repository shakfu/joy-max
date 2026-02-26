/*
 *  joy_max_bridge.h -- Bridge header for using Joy internals in Max externals
 *
 *  Handles the naming conflict between Joy's error() and Max SDK's error(),
 *  and provides access to JoyContext internals for stack inspection.
 *
 *  Usage: Include Max headers FIRST, then include this header.
 *  Do NOT include globals.h directly.
 */
#ifndef JOY_MAX_BRIDGE_H
#define JOY_MAX_BRIDGE_H

#include <string.h>
#include <stddef.h>

/* Max SDK defines error() as a logging function.
 * Joy's globals.h declares error(pEnv, char*) for parse errors.
 * Rename Joy's error to avoid the conflict. */
#define error joy_internal_error
#include "globals.h"
#undef error

/* JoyContext definition (mirrors src/joy.c) so we can access env */
struct JoyContext {
    Env env;
    JoyIO* io;
    JoyResult last_result;
    size_t gc_count;
};

/* Helper: get the internal Env pointer from a JoyContext */
static inline pEnv joy_get_env(JoyContext* ctx) {
    return &((struct JoyContext*)ctx)->env;
}

/*
 * joy_eval_string_keep -- like joy_eval_string but keeps results on stack.
 *
 * joy_eval_string discards the computed result after exec_term (it's
 * designed for the REPL where print() consumes results). This version
 * pops the program list BEFORE executing, so results remain on the stack.
 */
static inline JoyResult joy_eval_string_keep(JoyContext* ctx, const char* source)
{
    if (!ctx || !source)
        return JOY_ERROR_RUNTIME;

    pEnv env = &ctx->env;
    int err;

    err = setjmp(env->error_jmp);
    if (err == ABORT_QUIT) {
        ctx->last_result = JOY_ERROR_QUIT;
        return JOY_ERROR_QUIT;
    }
    if (err == ABORT_RETRY) {
        ctx->last_result = JOY_ERROR_ABORT;
        return JOY_ERROR_ABORT;
    }

    vec_push(env->pushback, ' ');
    vec_push(env->pushback, ' ');
    size_t len = strlen(source);
    for (size_t i = len; i > 0; i--) {
        vec_push(env->pushback, source[i - 1]);
    }

    if (!env->scanner.srcfile) {
        inilinebuffer(env);
    }

    int ch = getch(env);
    ch = getsym(env, ch);

    while (env->scanner.sym != '.') {
        if (env->scanner.sym == LIBRA || env->scanner.sym == HIDE ||
            env->scanner.sym == MODULE_ || env->scanner.sym == PRIVATE ||
            env->scanner.sym == PUBLIC || env->scanner.sym == CONST_) {
            ch = compound_def(env, ch);
            if (env->scanner.sym == '.')
                break;
            ch = getsym(env, ch);
        } else {
            ch = readterm(env, ch);
            if (env->stck) {
                /* Extract program, pop the list node, then execute.
                 * This leaves results on the stack instead of discarding them. */
                Index prog = nodevalue(env->stck).lis;
                env->stck = nextnode1(env->stck);
                exec_term(env, prog);
            }
            if (env->scanner.sym == '.')
                break;
            ch = getsym(env, ch);
        }
    }

    ctx->last_result = JOY_OK;
    return JOY_OK;
}

#endif /* JOY_MAX_BRIDGE_H */
