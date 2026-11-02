# evil-json

`evil-json` is a JSON parser that is meant only for the purpose to see
the fastest JSON parser I can build.
And it turned out very fast.
About 4x times faster than `js JSON.parse` engine and 2x times faster than `rapidjson`.

### build
```sh
make
```

### run

```sh
./exe data/20MB.json
```
