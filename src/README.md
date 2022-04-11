To ensure that the computer supports RAPL:
```
gcc -O2 -Wall -o rapl-read rapl-read.c -lm
sudo ./rapl-read -s
```


### CBOR to TXT
CBOR files are binary but Moss can only deal with text files, this program converts CBOR to TXT

To compile cbor-txt.hs:
```
ghc -o cbor-txt cbor-txt.hs -package ghc-dump-util
```

To use cbor-txt:
```
./cbor-txt <input_file> <output_file>
```

### Dependencies
 - colorized-logs
 <!-- - html2text -->

### 
https://stackoverflow.com/questions/646241/c-run-a-system-command-and-get-output
https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-regexec-execute-compiled-regular-expression
https://stackoverflow.com/questions/11765301/how-do-i-get-the-unix-timestamp-in-c-as-an-int
https://stackoverflow.com/questions/19843557/suppress-system-output