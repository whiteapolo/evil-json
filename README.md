# evil-json

`evil-json` is a small C JSON parser and a benchmark program. The parser reads
JSON text from a memory buffer and builds values in a `Flat_Pool` allocator.
The benchmark in `src/main.c` loads `data/20MB.json`, parses it, and prints the
elapsed time and approximate input throughput.

## Build and run the example

Build with the compiler configured as `cc` on your system (the Makefile uses
`-O3`):

```sh
make
```

This creates the `exe` program in the repository root. Run it from the root so
its relative path to the sample data file resolves correctly:

```sh
./exe
```

The program parses `data/20MB.json` and prints the elapsed milliseconds and
throughput in MB/s. Despite the filename, the checked-in file is currently
about 47 MB. The sample is intentionally large for benchmarking.

## Using the parser

The public entry point is declared in `src/json.h`:

```c
Json_Value json_parse(Flat_Pool *pool, const char *document, size_t length);
```

Pass the parser a pointer and byte length for the JSON text, along with a
`Flat_Pool` to hold the resulting values. `Json_Value` is tagged with one of
`JSON_STRING`, `JSON_NUMBER`, `JSON_BOOLEAN`, `JSON_NULL`, `JSON_OBJECT`, or
`JSON_ARRAY`; its `type` field tells you which union member to read. Strings are
represented by `Json_String` (`ptr` plus `length`), numbers by `double`, and
booleans by `bool`. Objects contain `Json_Object_Entry` values (a key and a
`Json_Value`); arrays contain `Json_Value` elements.

For a small in-memory example, the parse call looks like this:

```c
const char input[] = "{\"answer\":42}";
Flat_Pool pool = flat_pool_new();
Json_Value root = json_parse(&pool, input, sizeof(input) - 1);

/* Inspect root.type and its corresponding value here. */
flat_pool_free(&pool);
```

Include `src/json.h` and `src/flat-pool.h` and link the implementation files
under `src/` when writing a separate program. The repository's Makefile builds
the benchmark executable; it does not currently provide a separate library
target or accept an input filename on the command line.

## Repository layout

- `src/json.c`, `src/json.h`: JSON parsing and value types.
- `src/json-array.*`, `src/json-object.*`: array and object storage.
- `src/flat-pool.*`: arena-style allocation used for parsed data.
- `src/main.c`: benchmark executable entry point.
- `data/`: JSON inputs used by the benchmark and for experimentation.
