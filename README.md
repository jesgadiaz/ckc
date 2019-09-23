# ckc
Deterministic heuristic for the uniform capacitated vertex k-center problem

# Compile

```
$ gcc -o 'output' 'main.c' -lm

```

# Run

```
$ 'output' {file} {n} {k} {L} {out_perc} {multicenter} {ind_rep} {rep} {print}
```

Where,

|  Parameter |                                          Description                                          |
|----------|---------------------------------------------------------------------------------------------|
| `{file}` | (string) Instance file path with a valid format, defined here                                    |
| `{n}`    | (integer) Number of vertices  |
| `{k}`    | (integer) Number of centers   |
| `{L}`    | (integer) Uniform capacity    |
| `{out_perc}` | (float) Percentage of outliers between 0 and 1  |
| `{multicenter}` | (string) multicenters allowed = true; otherwise false  |
| `{ind_rep}`    | (integer) Independent repetitions of the algorithm, for sensitivity analysis |
| `{rep}`    | (integer) Number of repetions of the algorithm. The best of rep is returned by the algorithm. |
| `{print}`    | (string) If true, prints the solutions (centers and assigned centers). Otherwise, only prints the solution size |

# Example
```
$ 'output' 'pr124.tsp' 124 10 13 0.03 false 1 1 true
```

# Output information
The execution report a output with the following relevant information:

```
$ Solution size (0 perc. outliers) = 5668.443359
  Solution size (10.00 perc. outliers) = 1490.102051
  
  Exec. time per repetition:
  0.004904,
  Total time: 0.004904
```

