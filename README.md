# thesis

## Software versions used
 - `ghc`: `8.8.4`
 - `gcc`: `11.2.0`

___
## Setup

Compile tool binaries
```
make energy-read
make metrics
```

Get `nofib` benchmarks from the repository (commit: `bca0196`)
```
git clone https://gitlab.haskell.org/ghc/nofib.git nofib
cd nofib
git checkout bca0196
cd ..
```

Switch `nofib`'s original `target.mk` for modified version
```
cp src/nofib/target.mk nofib/mk/target.mk
```

Install packages required by `nofib` benchmarks
```
cabal v1-install --allow-newer -w ghc 
cabal v1-install old-locale-1.0.0.7 old-time-1.1.0.3 parallel-3.2.2.0 primitive-0.7.4.0 random-1.2.1.1 regex-base-0.94.0.2 regex-compat-0.95.2.1 regex-posix-0.96.0.1 splitmix-0.1.0.4 unboxed-ref-0.4.0.0
```

Install `lm-sensors` required to measure the CPUs temperature
```
sudo apt update
sudo apt install lm-sensors
```

One more thing, compile `runstdtest`
```
cd nofib/runstdtest
make
```

___
## Usage

Before running the tool we need to generate the input files for the benchmarks, `nofib`'s Makefile system provides a way to that
```
cd nofib
make boot
```

If everything is setup correctly and the input files have been generated, you just need to run the `metrics` binary with root permissions, and the tool will start executing. <br>
**IMPORTANT:** `sudo` is required in order for `RAPL` to work
```
sudo ./bin/metrics
```

___
## Configuration

The macros file (`src/utils/macros.h`) can be modified to configure the behaviour of the framework. *Remember to recompile to apply changes to this file.*
| MACRO       | Description |
|:-----------:|:------------|
| `PROG_FILE` | relative path to the file that contains the paths to the benchmarks (ex: `resources/benchmarks.txt`) |
| `FLAG_FILE` | relative path to file that contains the optimization flags (ex: `resources/flags.txt`) |
| `TEMP_FILE` | absolute path to a `.txt` file that will hold the baseline temperature |
| `RES_FILE`  | absolute path to a `.csv` file to which the results will be written | 
| `LOG_FILE`  | absolute path to a `.txt` file to which the framework logs the execution |
| `NUM_CPUS`  | number of CPUs in your system |
| `NUM_RUNS`  | number of times each benchmark is executed with each flag |
| `COOLDOWN`  | time to let system cool (seconds) |
| `NUM_COOLDOWN`  | number of consecutive times the system is within the temperature range for it to be considered cooled |
| `TEMP_RANGE`  | difference from the baseline for the system to be considered cooled |
| `NUM_BASELINE`  | number of samples to be colected to establish the average temperature of the system |
| `INT_BASELINE`  | interval between baseline sample collection (seconds) |

<br>

If you don't want the benchmarks to print anything to the terminal you can go into `src/metrics_fw/ghc.c` and change line 24:
```c
// from this
sprintf(make_cmd, "sudo make NoFibRuns=%d EXTRA_HC_OPTS=\"-O0 %s\"", runs, flag);

// to this
sprintf(make_cmd, "sudo make NoFibRuns=%d EXTRA_HC_OPTS=\"-O0 %s\" >null 2>null", runs, flag);

```

___
## Troubleshooting

### Disabling benchmark outputs
Another issue I ran into was the package database not being found. The file can be found at `~/.ghc/<ghc-version>/package.conf.d`. To fix this issue go into `src/metrics_fw/ghc.c` and change line 24:
```c
// from this
sprintf(make_cmd, "sudo make NoFibRuns=%d EXTRA_HC_OPTS=\"-O0 %s\"", runs, flag);

// to something like this
sprintf(make_cmd, "sudo make NoFibRuns=%d EXTRA_HC_OPTS=\"-package-db /home/rapi/.ghc/x86_64-linux-8.6.5/package.conf.d -O0 %s\"", runs, flag);
```

### Handling `ghc` versions
Install [`ghcup`](https://www.haskell.org/ghcup/), the desired `ghc` version and set it
```
curl --proto '=https' --tlsv1.2 -sSf https://get-ghcup.haskell.org | sh
ghcup install 8.8.4
ghcup set 8.8.4
```
___
## Available Data

```csv

```