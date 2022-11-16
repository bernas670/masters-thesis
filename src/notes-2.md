# thesis

## `stack`

`stack exec -- corediff-exe diff ... ...`
`stack exec -- ghc -fplugin GhcDump.Plugin ...`

## GHC 8.8.4 Optimization Flags
[Extracted list of flags](flags.txt) from [here](https://downloads.haskell.org/~ghc/8.8.4/docs/html/users_guide/using-optimisation.html) using the following XPath expression and removing the `-O*` flags.
```xpath
//*[@class="ghc-flag"]//code[@class="sig-name descname"]
```

## `nofib` Makefile

`cabal v1-install --allow-newer -w ghc random parallel old-time`

`cabal v1-install regex-compat`

`-package-db /home/rapi/.ghc/x86_64-linux-8.6.5/package.conf.d`

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

```
find nofib -type f -name "Makefile" | sed -r 's|/[^/]+$||' | sort >> resources/bench_test.txt
```

`find nofib -type f -name "Makefile" | sed -r 's|/[^/]+$||' | sort | uniq`
`find nofib -type f -name "*.lhs" | sed -r 's|/[^/]+$||' | sort | uniq`
```bash
# 1st find gets all directories with a Makefile
# 2nd find gets all directories with .lhs files (these don't work with ghc-dump)
comm -23 <(find nofib -type f -name "Makefile" | sed -r 's|/[^/]+$||' | sort | uniq) <(find nofib -type f -name "*.lhs" | sed -r 's|/[^/]+$||' | sort | uniq) >> resources/bench_test.txt
```

```
comm -12 <(find nofib -type f -name "Makefile" | sed -r 's|/[^/]+$||') <(find nofib -type d '!' -exec test -e "{}/*.lhs" ';' -print) >> resources/bench_test.txt
```

find nofib -type f -name "Makefile" | sed -r 's|/[^/]+$||'
find nofib -type d '!' -exec test -e "{}/*.lhs" ';' -print'

 - removed `.git` directories
 - removed other non-benchmark directories

## RAPL

 - Package: Package (PKG) domain measures the energy consumption of the entire socket. It
includes the consumption of all the cores, integrated graphics and also the uncore components
(last level caches, memory controller).
 - Power Plane 0: Power Plane 0 (PP0) domain measures the energy consumption of all
processor cores on the socket.
 - Power Plane 1: Power Plane 1 (PP1) domain measures the energy consumption of processor
graphics (GPU) on the socket (desktop models only).
 - DRAM: DRAM domain measures the energy consumption of random access memory (RAM)
attached to the integrated memory controller.
 - PSys: Intel Skylake has introduced a new RAPL Domain named PSys. It monitors and controls
the thermal and power specifications of the entire SoC and it is useful especially when the
source of the power consumption is neither the CPU nor the GPU. As Figure 1 suggests, PSys
includes the power consumption of the package domain, System Agent, PCH , eDRAM and a
few more domains on a single socket SoC.

**Note:** For multi-socket server systems, each socket reports its own RAPL values (for example a 2-socket
computing system has two separate PKG readings for both the packages, two separate PP0 readings,
etc).

**Source:** [RAPL in Action: Experiences in Using RAPL for Power Measurements](https://helda.helsinki.fi/bitstream/handle/10138/321707/RAPL_in_Action_Experiences_in_Using_RAPL_for_Power_Measurements.pdf?sequence=1)

- power plane 0 - core measurements
- power plane 1 - uncore measurements, see [here](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)




#### Notes
[`-Wtabs`](https://downloads.haskell.org/~ghc/8.8.4/docs/html/users_guide/using-warnings.html?highlight=wno%20tabs#ghc-flag--Wtabs) warns about tabs in the source file.


https://gitlab.haskell.org/ghc/ghc/-/wikis/building/running-nofib
https://mpickering.github.io/users_guide/flags.html#options-f-compact
https://github.com/greensoftwarelab/Energy-Languages

