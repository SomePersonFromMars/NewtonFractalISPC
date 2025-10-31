## Newton Fractal visualisation

### Example execution
The following Newton fractal visualises the complex roots of

$$
f(z) = z^5 - 1
$$

![newton-fractal-ispc](https://github.com/user-attachments/assets/ed308835-81a2-41d4-8d81-444e33c58b5d)

### Benchmarks
Executed on a mid-range `Intel i5-6300U (4) @ 3.000GHz` mobile CPU.
```txt
f(z) = z^5 - 1
WIDTH = 2048
WIDTH = 2048
MAX_ITERATIONS = 40
Started the parallel generation via ISPC...
Generation finished in 175 ms. Writing to file newton-fractal-ispc.ppm
```

```txt
f(z) = z^15 - 1
WIDTH = 4096
WIDTH = 4096
MAX_ITERATIONS = 40
Started the parallel generation via ISPC...
Generation finished in 2451 ms. Writing to file newton-fractal-ispc.ppm
```

```txt
f(z) = z^5 - 1
WIDTH = 8192
WIDTH = 8192
MAX_ITERATIONS = 40
Started the parallel generation via ISPC...
Generation finished in 2766 ms. Writing to file newton-fractal-ispc.ppm
```

### Usage
```txt
Usage: ./fractal.e N
    N - positive integer defining the function f(z) = z^N - 1
```

### Build instructions
The solution has been tested on Linux with `ispc` compiler installed. Simply execute `make` in the source directory.

