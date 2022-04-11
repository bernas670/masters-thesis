# thesis

## GHC 8.8.4 Optimization Flags
[Extracted list of flags](flags.txt) from [here](https://downloads.haskell.org/~ghc/8.8.4/docs/html/users_guide/using-optimisation.html) using the following XPath expression and removing the `-O*` flags.
```xpath
//*[@class="ghc-flag"]//code[@class="sig-name descname"]
```

## `nofib` Makefile

`cabal v1-install --allow-newer -w ghc random parallel old-time`
`cabal v1-install regex-compat`

In [`boilerplate.mk`](nofib/mk/boilerplate.mk)

| File | Before | After |
|:-----|:-------|:------|
| [`boilerplate.mk, line 45`](nofib/mk/boilerplate.mk#L45) | `# Haskell compiler options for nofib`<br>`NoFibHcOpts = -O2 -Wno-tabs` | `# Haskell compiler options for nofib`<br>`NoFibHcOpts = -O0 -Wno-tabs` |
|[`boilerplate.mk, line 48`](nofib/mk/boilerplate.mk#L48) | `# Number of times to run each program`<br>`NoFibRuns = 5` | `# Number of times to run each program`<br>`NoFibRuns = 0` |


| Command | Description |
|:--------|:------------|
| `make clean` | **will this remove the generated input files??** |
| `make boot`  | generate input files for the benchmarks and build compilation dependencies for `make` |
| `make`       | builds benchmark and runs it `$NoFibRuns` times |
| `make EXTRA_HC_OPTS="-fllvm"` | add extra compilation flags |
| `make NoFibRuns=30` | builds benchmark and runs it `$NoFibRuns` times |


## GHC-Dump
```
cabal install ghc-dump-core --lib
```

## Moss
```
moss -d
```

## nofib benchmarks
```
find nofib -type d >> resources/benchmarks.txt
```
 - removed `.git` directories
 - removed other non-benchmark directories


#### Notes
[`-Wtabs`](https://downloads.haskell.org/~ghc/8.8.4/docs/html/users_guide/using-warnings.html?highlight=wno%20tabs#ghc-flag--Wtabs) warns about tabs in the source file.


https://gitlab.haskell.org/ghc/ghc/-/wikis/building/running-nofib
https://mpickering.github.io/users_guide/flags.html#options-f-compact
https://github.com/greensoftwarelab/Energy-Languages

