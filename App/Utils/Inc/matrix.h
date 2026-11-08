// Comment

#pragma once

#include <stdint.h>

// Maximum number of rows or columns supported by the matrix library.
#define MATRIX_MAX_DIM 4


// Status codes returned by matrix operations.
typedef enum {
    MATRIX_OK,                  // Operation completed successfully.
    MATRIX_INVALID_DIMENSIONS,  // Matrix dimensions are incompatible or invalid.
    MATRIX_SINGULAR,             // Matrix cannot be inverted.
    MATRIX_INVALID_POINTER,      // A required pointer is NULL.
    MATRIX_NOT_VECTOR,           // Operation requires a vector, not a matrix.
    MATRIX_ZERO_VECTOR           // Cannot normalize a zero-magnitude vector.
} Matrix_Status;


// Represents a matrix stored as a one-dimensional, row-major array.
typedef struct {
    uint8_t rows;   // Number of rows in the matrix.
    uint8_t cols;   // Number of columns in the matrix.
    float *data;    // Pointer to the matrix elements in row-major order.
} Matrix;


// Accesses the element at row r and column c.
// Uses zero-based indexing.
// Example: MAT(A, 0, 0) accesses the first element of matrix A.
#define MAT(m, r, c) ((m).data[(r) * (m).cols + (c)])


// ======================== INITIALIZATION ========================

// Initializes a matrix with specified dimensions and a data array.
// Does not allocate memory or initialize the array's values.
// Parameters:
//   m    - Pointer to the matrix structure.
//   rows - Number of rows.
//   cols - Number of columns.
//   data - Pointer to the statically allocated array.
// Returns:
//   MATRIX_OK if successful; otherwise an error status.
Matrix_Status matrix_init(Matrix *m, uint8_t rows,
                          uint8_t cols, float *data);


// ======================== BASIC OPERATIONS ========================

// Sets every element of a matrix to zero.
// Parameters:
//   m - Pointer to the matrix.
// Returns:
//   MATRIX_OK if successful; otherwise an error status.
Matrix_Status matrix_zero(Matrix *m);


// Sets a square matrix to the identity matrix.
// Diagonal elements are set to 1; all other elements are set to 0.
// Parameters:
//   m - Pointer to the matrix.
// Returns:
//   MATRIX_OK if successful; otherwise an error status.
Matrix_Status matrix_identity(Matrix *m);


// Copies all elements from the source matrix into the destination.
// Source and destination must have identical dimensions.
// Parameters:
//   dest - Pointer to the destination matrix.
//   src  - Pointer to the source matrix.
// Returns:
//   MATRIX_OK if successful; otherwise an error status.
Matrix_Status matrix_copy(Matrix *dest, const Matrix *src);


// ======================== MATRIX ARITHMETIC ========================

// Adds two matrices element by element: C = A + B.
// All three matrices must have identical dimensions.
// Parameters:
//   A - Pointer to the first input matrix.
//   B - Pointer to the second input matrix.
//   C - Pointer to the output matrix.
// Returns:
//   MATRIX_OK if successful; otherwise an error status.
Matrix_Status matrix_add(const Matrix *A, const Matrix *B, Matrix *C);


// Subtracts two matrices element by element: C = A - B.
// All three matrices must have identical dimensions.
// Parameters:
//   A - Pointer to the first input matrix.
//   B - Pointer to the second input matrix.
//   C - Pointer to the output matrix.
// Returns:
//   MATRIX_OK if successful; otherwise an error status.
Matrix_Status matrix_subtract(const Matrix *A, const Matrix *B, Matrix *C);


// Multiplies two matrices: C = A * B.
// Number of columns in A must equal number of rows in B.
// C must have the dimensions rows(A) x cols(B).
// Supports C sharing its data array with A or B.
// Parameters:
//   A - Pointer to the first input matrix.
//   B - Pointer to the second input matrix.
//   C - Pointer to the output matrix.
// Returns:
//   MATRIX_OK if successful; otherwise an error status.
Matrix_Status matrix_multiply(const Matrix *A, const Matrix *B, Matrix *C);


// Multiplies every element of a matrix by a scalar: B = scalar * A.
// Input and output matrices must have identical dimensions.
// Parameters:
//   A      - Pointer to the input matrix.
//   scalar - Scalar multiplication factor.
//   B      - Pointer to the output matrix.
// Returns:
//   MATRIX_OK if successful; otherwise an error status.
Matrix_Status matrix_scale(const Matrix *A, float scalar, Matrix *B);


// ======================== MATRIX TRANSFORMATIONS ========================

// Transposes a matrix: B = A^T.
// Rows become columns and columns become rows.
// B must have dimensions cols(A) x rows(A).
// Supports A and B sharing the same data array.
// Parameters:
//   A - Pointer to the input matrix.
//   B - Pointer to the output matrix.
// Returns:
//   MATRIX_OK if successful; otherwise an error status.
Matrix_Status matrix_transpose(const Matrix *A, Matrix *B);


// Calculates the inverse of a square matrix: B = A^-1.
// Uses Gauss-Jordan elimination with partial pivoting.
// Returns MATRIX_SINGULAR if the matrix cannot be inverted
// within the implementation's numerical tolerance.
// Parameters:
//   A - Pointer to the input matrix.
//   B - Pointer to the output matrix.
// Returns:
//   MATRIX_OK if successful; otherwise an error status.
Matrix_Status matrix_inverse(const Matrix *A, Matrix *B);


// ======================== VECTOR OPERATIONS ========================

// Calculates the dot product of two vectors:
// result = A^T * B (for column vectors).
// Both inputs must be vectors with the same number of elements.
// Supports row vectors and column vectors.
// Parameters:
//   A      - Pointer to the first input vector.
//   B      - Pointer to the second input vector.
//   result - Pointer to the scalar output.
// Returns:
//   MATRIX_OK if successful; otherwise an error status.
Matrix_Status matrix_dot(const Matrix *A, const Matrix *B, float *result);


// Calculates the Euclidean norm (magnitude) of a vector:
// result = sqrt(sum of squared elements).
// The input must be a row vector or column vector.
// Parameters:
//   A      - Pointer to the input vector.
//   result - Pointer to the scalar output.
// Returns:
//   MATRIX_OK if successful; otherwise an error status.
Matrix_Status matrix_norm(const Matrix *A, float *result);


// Normalizes a vector so that its magnitude equals 1.
// Each element is divided by the vector's Euclidean norm.
// The input vector is modified in place.
// Returns MATRIX_ZERO_VECTOR if its magnitude is too small.
// Parameters:
//   A - Pointer to the vector to normalize.
// Returns:
//   MATRIX_OK if successful; otherwise an error status.
Matrix_Status matrix_normalize(Matrix *A);


// ======================== DEBUGGING ========================

// Prints a matrix's elements in row and column format.
// Intended for debugging; requires a configured printf output.
// Parameters:
//   A - Pointer to the matrix to print.
void matrix_print(const Matrix *A);

void print_matrix_status(Matrix_Status status);
