# json2yaml
A simple json2yaml converter with the elegance of recursion

```
Depends: libjansson

Compile:
    gcc -o json2yaml json2yaml.c -I<path to jansson.h> -L<path to libjansson.so> -ljansson

Usage:
    ./json2yaml <input json file> > <output json file>
```
