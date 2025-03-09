# Matrix Multiplication with Multi-threading

This project implements a matrix multiplication program in C using POSIX threads. It demonstrates three different strategies for parallelizing matrix multiplication:

- **Per Matrix:** A single thread multiplies the entire matrix.
- **Per Row:** Each row of the resulting matrix is computed by a separate thread.
- **Per Element:** Each element of the resulting matrix is computed by its own thread.

The program also measures the execution time for each approach using `gettimeofday`.

---

## Features

- **Dynamic Matrix Allocation:** Matrices are allocated and freed dynamically.
- **Flexible File I/O:** Reads matrices from input files and writes multiplication results to separate output files.
- **Error Handling:** Includes error checking for memory allocation and file operations.
- **Command-line Arguments:** Supports default file names or custom file paths provided as arguments.
- **Performance Measurement:** Execution times (in microseconds) are printed for each threading approach.
- **Multi-threaded Computation:** Utilizes pthreads to perform parallel matrix multiplication.

---

## File Structure

- **Source Code:** `matrix_multiplication.c` (contains all functions and the `main` routine)

---

## Requirements

- GCC (or any compatible C compiler)
- POSIX Threads Library (`pthread`)
- Linux/Unix environment (or any environment that supports POSIX threads)

---

## Compilation

Compile the program using GCC. Make sure to link the pthread library:

```bash
gcc matrix_multiplication.c -o matrix_multiplication -lpthread
```

---

## Usage

The program can be executed with or without command-line arguments.

### With Default File Paths

If no command-line arguments are provided, the program uses default file names:
- Input matrix A: `a.txt`
- Input matrix B: `b.txt`
- Output matrix files will have the base name `c` with different suffixes:
  - `c_per_matrix.txt`
  - `c_per_row.txt`
  - `c_per_element.txt`

Run with:

```bash
./matrix_multiplication
```

### With Custom File Paths

To specify custom file names, provide three arguments:
1. Base name for matrix A (without extension)
2. Base name for matrix B (without extension)
3. Base name for output matrix C

For example:

```bash
./matrix_multiplication matrixA matrixB result
```

This command will use:
- `matrixA.txt` for matrix A,
- `matrixB.txt` for matrix B, and
- Create output files: `result_per_matrix.txt`, `result_per_row.txt`, `result_per_element.txt`.

---

## Input File Format

Each input matrix file should have the following format:

1. **Header:** A line specifying the number of rows and columns, e.g.,  
   ```
   row=3 col=4
   ```
2. **Matrix Elements:** Subsequent lines contain the matrix elements (space-separated integers), matching the dimensions defined in the header.

Example of a 2x3 matrix:

```
row=2 col=3
1 2 3
4 5 6
```

---

## Program Structure

### Key Functions

- **`str_append`**: Concatenates two strings and returns a newly allocated string.
- **`open_file`**: Opens a file and handles errors if the file cannot be opened.
- **`allocate_matrix` / `free_matrix`**: Functions to allocate and free memory for matrices.
- **`read_matrix` / `write_matrix`**: Functions to read a matrix from a file and write a matrix to a file.
- **`clear_matrix`**: Resets all elements of a matrix to zero.
- **`threaded_matrx_mult`**: The function executed by threads for computing a segment of the matrix multiplication.
- **`create_paths` / `free_paths`**: Create and free file path structures based on command-line arguments.

### Multi-threading Approaches

- **Per Matrix:** Uses one thread for the entire matrix multiplication.
- **Per Row:** Spawns one thread per row, with each thread computing one row of the result.
- **Per Element:** Creates one thread per element in the resulting matrix.

---

## Performance Reporting

After executing the multiplication using each strategy, the program prints:
- The time taken (in microseconds)
- The number of threads created

---

## Example

Assume you have two input files, `a.txt` and `b.txt`, formatted as described above. To compile and run:

```bash
gcc matrix_multiplication.c -o matrix_multiplication -lpthread
./matrix_multiplication
```

The program will generate three output files:
- `c_per_matrix.txt`
- `c_per_row.txt`
- `c_per_element.txt`

And print the execution times and thread counts for each approach to the console.

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
