# ckc
Deterministic heuristic for the uniform capacitated vertex k-center problem

# Compile

```
$ gcc -o 'output' 'main.c' -lm

```

# Run

```
$ './output' [file] [n] [k] [L] [out_perc] [multicenter] [ind_rep] [rep] [print] [instance_format]
```

## Where,

|  Parameter |                                          Description                                          |
|----------|---------------------------------------------------------------------------------------------|
| `[file]` | (string) Instance file path with a valid format, defined here                                    |
| `[n]`    | (integer) Number of vertices  |
| `[k]`    | (integer) Number of centers   |
| `[L]`    | (integer) Uniform capacity    |
| `[out_perc]` | (float) Percentage of outliers between 0 and 1  |
| `[multicenter]` | (string) multicenters allowed = true; otherwise false  |
| `[ind_rep]`    | (integer) Independent repetitions of the algorithm, for sensitivity analysis |
| `[rep]`    | (integer) Number of repetions of the algorithm. The best of rep is returned by the algorithm. |
| `[print]`    | (string) If true, prints the solutions (centers and assigned centers). Otherwise, only prints the solution size |
| `[instance_format]`    | (string) Type of instance to use ('tsplib' or 'orlib' are supported). |

# Example
```
$ 'output' 'pr124.tsp' 124 10 13 0.03 false 1 1 false tsplib
```

### Output information
The execution report a output with the following relevant information:

```
$ Solution size (0 perc. outliers) = 4162.655273
  Solution size (3.00 perc. outliers) = 2045.880005
  
  Exec. time per repetition:
  0.014728,
  Total time: 0.014728 
```
# Another example

```
$ 'output' 'pr124.tsp' 124 10 13 0.03 false 1 1 true tsplib
```

### Output information
The execution report a output with the following relevant information:

```
$ Solution size (0 perc. outliers) = 4162.655273 
  Solution size (3.00 perc. outliers) = 2045.880005 

  { "instance": "/home/ckc/Escritorio/pr124.tsp", 
    "outliers": [83,40,115,114], 
    "centers": [ { "center": 59, "nodes": [28,32,33,34,35,54,57,58,59,60,61,64,65]},
                 { "center": 102, "nodes": [101,102,103,104,105,106,107,108,109,110,111,112,113]},
                 { "center": 8, "nodes": [8,9,10,11,12,13,14,15,16,46,47,48,49]},
                 { "center": 79, "nodes": [77,78,79,91,92,93,94,95,96,97,98,99,123]},
                 { "center": 6, "nodes": [0,1,2,3,4,5,6,7,26,27,29,30,31]},
                 { "center": 36, "nodes": [19,20,21,22,23,24,25,36,37,38,39,55,56]},
                 { "center": 16, "nodes": [17,18,40,41,42,43,44,45,50,51,52,53]},
                 { "center": 96, "nodes": [72,73,74,75,76,80,116,117,118,119,120,121,122]},
                 { "center": 89, "nodes": [84,85,86,87,88,89,90,100]},
                 { "center": 64, "nodes": [62,63,66,67,68,69,70,71,81,82,83,114,115]}
  ]}
  
  Exec. time per repetition:
  0.014728,
  Total time: 0.014728
 ```
  

