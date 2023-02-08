#pragma once

#include <list>
#include <map>
#include <unordered_map>

#include "types.hh"
#include "chunked-vector.hh"

namespace nix {

/* Symbol table used by the parser and evaluator to represent and look
   up identifiers and attributes efficiently.  SymbolTable::create()
   converts a string into a symbol.  Symbols have the property that
   they can be compared efficiently (using an equality test),
   because the symbol table stores only one copy of each string. */

/* This class mainly exists to give us an operator<< for ostreams. We could also
   return plain strings from SymbolTable, but then we'd have to wrap every
   instance of a symbol that is fmt()ed, which is inconvenient and error-prone. */
class SymbolStr
{
    friend class MinimalSymbolTable;

private:
    const std::string * s;

    explicit SymbolStr(const std::string & symbol): s(&symbol) {}

public:
    bool operator == (std::string_view s2) const
    {
        return *s == s2;
    }

    operator const std::string & () const
    {
        return *s;
    }

    operator const std::string_view () const
    {
        return *s;
    }

    friend std::ostream & operator <<(std::ostream & os, const SymbolStr & symbol);
};

class Symbol
{
    friend class MinimalSymbolTable;

private:
    uint32_t id;

    explicit Symbol(uint32_t id): id(id) {}

public:
    Symbol() : id(0) {}

    explicit operator bool() const { return id > 0; }

    bool operator<(const Symbol other) const { return id < other.id; }
    bool operator==(const Symbol other) const { return id == other.id; }
    bool operator!=(const Symbol other) const { return id != other.id; }
};

/* Minimal SymbolTable for testing-purposes. */
class MinimalSymbolTable
{
    friend struct SymbolTable;

private:
    std::unordered_map<std::string_view, std::pair<const std::string *, uint32_t>> symbols;
    ChunkedVector<std::string, 8192> store{16};

    // Caution: Be sure, the symbol to add is not already inserted.
    // In doubt, use create().
    Symbol insert(std::string_view s)
    {
        const auto & [rawSym, idx] = store.add(std::string(s));
        symbols.emplace(rawSym, std::make_pair(&rawSym, idx));
        return Symbol(idx + 1);
    }

public:
    Symbol create(std::string_view s)
    {
        // Most symbols are looked up more than once, so we trade off insertion performance
        // for lookup performance.
        // TODO: could probably be done more efficiently with transparent Hash and Equals
        // on the original implementation using unordered_set
        // FIXME: make this thread-safe.
        auto it = symbols.find(s);
        return (it != symbols.end()) ? Symbol(it->second.second + 1) : insert(s);
    }

    std::vector<SymbolStr> resolve(const std::vector<Symbol> & symbols) const
    {
        std::vector<SymbolStr> result;
        result.reserve(symbols.size());
        for (auto sym : symbols)
            result.push_back((*this)[sym]);
        return result;
    }

    SymbolStr operator[](Symbol s) const
    {
        if (s.id == 0 || s.id > store.size())
            abort();
        return SymbolStr(store[s.id - 1]);
    }

    size_t size() const
    {
        return store.size();
    }

    size_t totalSize() const;

    template<typename T>
    void dump(T callback) const
    {
        store.forEach(callback);
    }
};

/* MinimalSymbolTable, but enhanced with common symbols. This was moved here
   from EvalState, because the number of these symbols increased too much and
   now it is more compact. */
struct SymbolTable : public MinimalSymbolTable
{
    const Symbol
        // sorted alphabetically
        __contentAddressed, __functor, __ignoreNulls, __impure, __operators,
        __overrides, __structuredAttrs, __toString,
        _combineChannels, _type,
        add, allOutputs, args, body, builder, column, concat,
        derivation, derivations, description, divide, drvPath,
        epsilon, equal, file, flake, follows, has, hydraJobs,
        inputs, imply, import, key, lessThan, line, logicAnd, logicNot, logicOr,
        mainProgram, meta, multiply, name, negate, nixConfig, nixPath,
        operator_, or_, outPath, outputHash, outputHashAlgo, outputHashMode,
        outputName, outputSpecified, outputs, outputsToInstall,
        path, pname, prefix, priority,
        readFileType, recurseForDerivations, resolvePath, right,
        self, startSet, subtract, system,
        type, update, url, urls, value, welcomeText, with, wrong;

    SymbolTable()
    : __contentAddressed(insert("__contentAddressed"))
    , __functor(insert("__functor"))
    , __ignoreNulls(insert("__ignoreNulls"))
    , __impure(insert("__impure"))
    , __operators(insert("__operators"))
    , __overrides(insert("__overrides"))
    , __structuredAttrs(insert("__structuredAttrs"))
    , __toString(insert("__toString"))
    , _combineChannels(insert("_combineChannels"))
    , _type(insert("_type"))
    , add(insert("add"))
    , allOutputs(insert("allOutputs"))
    , args(insert("args"))
    , body(insert("body"))
    , builder(insert("builder"))
    , column(insert("column"))
    , concat(insert("concat"))
    , derivation(insert("derivation"))
    , derivations(insert("derivations"))
    , description(insert("description"))
    , divide(insert("divide"))
    , drvPath(insert("drvPath"))
    , epsilon(insert(""))
    , equal(insert("equal"))
    , file(insert("file"))
    , flake(insert("flake"))
    , follows(insert("follows"))
    , has(insert("has"))
    , hydraJobs(insert("hydraJobs"))
    , inputs(insert("inputs"))
    , imply(insert("imply"))
    , import(insert("import"))
    , key(insert("key"))
    , lessThan(insert("lessThan"))
    , line(insert("line"))
    , logicAnd(insert("logicOr"))
    , logicNot(insert("logicNot"))
    , logicOr(insert("logicAnd"))
    , mainProgram(insert("mainProgram"))
    , meta(insert("meta"))
    , multiply(insert("multiply"))
    , name(insert("name"))
    , negate(insert("negate"))
    , nixConfig(insert("nixConfig"))
    , nixPath(insert("nixPath"))
    , operator_(insert("operator"))
    , or_(insert("or"))
    , outPath(insert("outPath"))
    , outputHash(insert("outputHash"))
    , outputHashAlgo(insert("outputHashAlgo"))
    , outputHashMode(insert("outputHashMode"))
    , outputName(insert("outputName"))
    , outputSpecified(insert("outputSpecified"))
    , outputs(insert("outputs"))
    , outputsToInstall(insert("outputsToInstall"))
    , path(insert("path"))
    , pname(insert("pname"))
    , prefix(insert("prefix"))
    , priority(insert("priority"))
    , readFileType(insert("readFileType"))
    , recurseForDerivations(insert("recurseForDerivations"))
    , resolvePath(insert("resolvePath"))
    , right(insert("right"))
    , self(insert("self"))
    , startSet(insert("startSet"))
    , subtract(insert("subtract"))
    , system(insert("system"))
    , type(insert("type"))
    , update(insert("update"))
    , url(insert("url"))
    , urls(insert("urls"))
    , value(insert("value"))
    , welcomeText(insert("welcomeText"))
    , with(insert("<with>"))
    , wrong(insert("wrong"))
    {}
};

}
