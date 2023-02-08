#pragma once

#include "eval.hh"

#include <tuple>
#include <vector>

namespace nix {

struct RegisterPrimOp
{
    struct Info
    {
        std::string_view name;
        std::vector<std::string_view> args;
        size_t arity = 0;
        std::string_view doc;
        PrimOpFun fun;
        std::optional<ExperimentalFeature> experimentalFeature;
    };

    typedef std::vector<Info> PrimOps;
    static PrimOps * primOps;

    /* You can register a constant by passing an arity of 0. fun
       will get called during EvalState initialization, so there
       may be primops not yet added and builtins is not yet sorted. */
    RegisterPrimOp(
        std::string_view name,
        size_t arity,
        PrimOpFun fun);

    RegisterPrimOp(
        std::string_view name,
        std::vector<std::string_view> args,
        PrimOpFun fun);

    RegisterPrimOp(Info && info);
};

/* These primops are disabled without enableNativeCode, but plugins
   may wish to use them in limited contexts without globally enabling
   them. */

/* Load a ValueInitializer from a DSO and return whatever it initializes */
void prim_importNative(EvalState & state, const PosIdx pos, Value * * args, Value & v);

/* Execute a program and parse its output */
void prim_exec(EvalState & state, const PosIdx pos, Value * * args, Value & v);

}
