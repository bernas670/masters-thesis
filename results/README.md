### res_06
##### Programs
ok ==> gc/cacheprof
ok ==> gc/constraints

ERROR ==> gc/gc_bench

ok ==> gc/hash
ok ==> imaginary/bernouilli
ok ==> imaginary/exp3_8
ok ==> imaginary/gen_regexps
ok ==> imaginary/integrate

error ==> parallel/blackscholes
error ==> parallel/coins
error ==> parallel/matmult
error ==> parallel/nbody

ok ==> real/anna

error ==> real/ben-raytrace

ok ==> real/cacheprof
ok ==> real/compress2
ok ==> shootout/binary-trees

error ==> shootout/fannkuch-redux     ==> very cpu intensive, shutsdown server

ok ==> shootout/fasta
ok ==> shootout/k-nucleotide

error ==> shootout/n-body             ==> takes very long, measurements might not be accurate

ok ==> shootout/pidigits           
ok ==> smp/callback001
ok ==> smp/callback002
ok ==> smp/chan

error ==> smp/sieve

ok ==> spectral/ansi
ok ==> spectral/atom
ok ==> spectral/awards
ok ==> spectral/banner
##### Flags
-O0
-fcase-merge
-fcase-folding
-fcall-arity
-fexitification
-fcmm-elim-common-blocks
-fcmm-sink
-fcpr-anal