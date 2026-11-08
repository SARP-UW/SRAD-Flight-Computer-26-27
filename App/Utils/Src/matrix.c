#include "matrix.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

// Check whether a matrix is valid.
static Matrix_Status matrix_validate(const Matrix *m)
{
    if (m == NULL || m->data == NULL)
        return MATRIX_INVALID_POINTER;

    if (m->rows == 0 || m->cols == 0 ||
        m->rows > MATRIX_MAX_DIM ||
        m->cols > MATRIX_MAX_DIM)
        return MATRIX_INVALID_DIMENSIONS;

    return MATRIX_OK;
}


// Initialize a matrix.
Matrix_Status matrix_init(Matrix *m, uint8_t rows,
                          uint8_t cols, float *data)
{
    if (m == NULL || data == NULL)
        return MATRIX_INVALID_POINTER;

    if (rows == 0 || cols == 0 ||
        rows > MATRIX_MAX_DIM ||
        cols > MATRIX_MAX_DIM)
        return MATRIX_INVALID_DIMENSIONS;

    m->rows = rows;
    m->cols = cols;
    m->data = data;

    return MATRIX_OK;
}


// Set all elements to zero.
Matrix_Status matrix_zero(Matrix *m)
{
    if (matrix_validate(m) != MATRIX_OK)
        return matrix_validate(m);

    for (uint8_t i = 0; i < m->rows * m->cols; i++)
        m->data[i] = 0.0f;

    return MATRIX_OK;
}


// Create an identity matrix.
Matrix_Status matrix_identity(Matrix *m)
{
    if (matrix_validate(m) != MATRIX_OK)
        return matrix_validate(m);

    if (m->rows != m->cols)
        return MATRIX_INVALID_DIMENSIONS;

    matrix_zero(m);

    for (uint8_t i = 0; i < m->rows; i++)
        MAT((*m), i, i) = 1.0f;

    return MATRIX_OK;
}


// Copy one matrix into another.
Matrix_Status matrix_copy(Matrix *dest, const Matrix *src)
{
    if (matrix_validate(dest) != MATRIX_OK)
        return matrix_validate(dest);

    if (matrix_validate(src) != MATRIX_OK)
        return matrix_validate(src);

    if (dest->rows != src->rows ||
        dest->cols != src->cols)
        return MATRIX_INVALID_DIMENSIONS;

    for (uint8_t i = 0; i < src->rows * src->cols; i++)
        dest->data[i] = src->data[i];

    return MATRIX_OK;
}


// Add two matrices: C = A + B.
Matrix_Status matrix_add(const Matrix *A,
                         const Matrix *B, Matrix *C)
{
    if (matrix_validate(A) != MATRIX_OK)
        return matrix_validate(A);

    if (matrix_validate(B) != MATRIX_OK)
        return matrix_validate(B);

    if (matrix_validate(C) != MATRIX_OK)
        return matrix_validate(C);

    if (A->rows != B->rows || A->cols != B->cols ||
        C->rows != A->rows || C->cols != A->cols)
        return MATRIX_INVALID_DIMENSIONS;

    for (uint8_t i = 0; i < A->rows * A->cols; i++)
        C->data[i] = A->data[i] + B->data[i];

    return MATRIX_OK;
}


// Subtract two matrices: C = A - B.
Matrix_Status matrix_subtract(const Matrix *A,
                              const Matrix *B, Matrix *C)
{
    if (matrix_validate(A) != MATRIX_OK)
        return matrix_validate(A);

    if (matrix_validate(B) != MATRIX_OK)
        return matrix_validate(B);

    if (matrix_validate(C) != MATRIX_OK)
        return matrix_validate(C);

    if (A->rows != B->rows || A->cols != B->cols ||
        C->rows != A->rows || C->cols != A->cols)
        return MATRIX_INVALID_DIMENSIONS;

    for (uint8_t i = 0; i < A->rows * A->cols; i++)
        C->data[i] = A->data[i] - B->data[i];

    return MATRIX_OK;
}


// Multiply two matrices: C = A * B.
Matrix_Status matrix_multiply(const Matrix *A,
                              const Matrix *B, Matrix *C)
{
    if (matrix_validate(A) != MATRIX_OK)
        return matrix_validate(A);

    if (matrix_validate(B) != MATRIX_OK)
        return matrix_validate(B);

    if (matrix_validate(C) != MATRIX_OK)
        return matrix_validate(C);

    if (A->cols != B->rows ||
        C->rows != A->rows ||
        C->cols != B->cols)
        return MATRIX_INVALID_DIMENSIONS;

    // Temporary storage allows C to alias A or B.
    float temp[MATRIX_MAX_DIM * MATRIX_MAX_DIM] = {0};

    for (uint8_t i = 0; i < A->rows; i++) {
        for (uint8_t j = 0; j < B->cols; j++) {

            for (uint8_t k = 0; k < A->cols; k++) {
                temp[i * C->cols + j] +=
                    MAT((*A), i, k) * MAT((*B), k, j);
            }
        }
    }

    for (uint8_t i = 0; i < C->rows * C->cols; i++)
        C->data[i] = temp[i];

    return MATRIX_OK;
}


// Multiply a matrix by a scalar: B = scalar * A.
Matrix_Status matrix_scale(const Matrix *A, float scalar,
                           Matrix *B)
{
    if (matrix_validate(A) != MATRIX_OK)
        return matrix_validate(A);

    if (matrix_validate(B) != MATRIX_OK)
        return matrix_validate(B);

    if (A->rows != B->rows || A->cols != B->cols)
        return MATRIX_INVALID_DIMENSIONS;

    for (uint8_t i = 0; i < A->rows * A->cols; i++)
        B->data[i] = scalar * A->data[i];

    return MATRIX_OK;
}


// Transpose a matrix: B = A^T.
Matrix_Status matrix_transpose(const Matrix *A, Matrix *B)
{
    if (matrix_validate(A) != MATRIX_OK)
        return matrix_validate(A);

    if (matrix_validate(B) != MATRIX_OK)
        return matrix_validate(B);

    if (B->rows != A->cols || B->cols != A->rows)
        return MATRIX_INVALID_DIMENSIONS;

    float temp[MATRIX_MAX_DIM * MATRIX_MAX_DIM] = {0};

    for (uint8_t i = 0; i < A->rows; i++) {
        for (uint8_t j = 0; j < A->cols; j++) {
            temp[j * B->cols + i] = MAT((*A), i, j);
        }
    }

    for (uint8_t i = 0; i < B->rows * B->cols; i++)
        B->data[i] = temp[i];

    return MATRIX_OK;
}


// Invert a square matrix using Gauss-Jordan elimination
// with partial pivoting.
Matrix_Status matrix_inverse(const Matrix *A, Matrix *B)
{
    if (matrix_validate(A) != MATRIX_OK)
        return matrix_validate(A);

    if (matrix_validate(B) != MATRIX_OK)
        return matrix_validate(B);

    if (A->rows != A->cols ||
        B->rows != A->rows ||
        B->cols != A->cols)
        return MATRIX_INVALID_DIMENSIONS;

    const uint8_t n = A->rows;

    // Augmented matrix [A | I].
    float aug[MATRIX_MAX_DIM][2 * MATRIX_MAX_DIM] = {0};

    for (uint8_t i = 0; i < n; i++) {
        for (uint8_t j = 0; j < n; j++)
            aug[i][j] = MAT((*A), i, j);

        aug[i][i + n] = 1.0f;
    }

    for (uint8_t i = 0; i < n; i++) {

        // Find largest pivot in the current column.
        uint8_t pivot_row = i;
        float max_value = fabsf(aug[i][i]);

        for (uint8_t j = i + 1; j < n; j++) {
            float value = fabsf(aug[j][i]);

            if (value > max_value) {
                max_value = value;
                pivot_row = j;
            }
        }

        // Check for a singular or nearly singular matrix.
        if (max_value < 1e-6f)
            return MATRIX_SINGULAR;

        // Swap rows if necessary.
        if (pivot_row != i) {
            for (uint8_t j = 0; j < 2 * n; j++) {
                float temp = aug[i][j];
                aug[i][j] = aug[pivot_row][j];
                aug[pivot_row][j] = temp;
            }
        }

        // Normalize pivot row.
        float pivot = aug[i][i];

        for (uint8_t j = 0; j < 2 * n; j++)
            aug[i][j] /= pivot;

        // Eliminate other entries in this column.
        for (uint8_t k = 0; k < n; k++) {
            if (k == i)
                continue;

            float factor = aug[k][i];

            for (uint8_t j = 0; j < 2 * n; j++)
                aug[k][j] -= factor * aug[i][j];
        }
    }

    // Copy inverse into B.
    for (uint8_t i = 0; i < n; i++) {
        for (uint8_t j = 0; j < n; j++)
            MAT((*B), i, j) = aug[i][j + n];
    }

    return MATRIX_OK;
}


// Calculate dot product of two vectors.
Matrix_Status matrix_dot(const Matrix *A, const Matrix *B,
                         float *result)
{
    if (result == NULL)
        return MATRIX_INVALID_POINTER;

    if (matrix_validate(A) != MATRIX_OK)
        return matrix_validate(A);

    if (matrix_validate(B) != MATRIX_OK)
        return matrix_validate(B);

    uint8_t size_A = A->rows * A->cols;
    uint8_t size_B = B->rows * B->cols;

    bool A_vector = (A->rows == 1 || A->cols == 1);
    bool B_vector = (B->rows == 1 || B->cols == 1);

    if (!A_vector || !B_vector)
        return MATRIX_NOT_VECTOR;

    if (size_A != size_B)
        return MATRIX_INVALID_DIMENSIONS;

    *result = 0.0f;

    for (uint8_t i = 0; i < size_A; i++)
        *result += A->data[i] * B->data[i];

    return MATRIX_OK;
}


// Calculate vector magnitude.
Matrix_Status matrix_norm(const Matrix *A, float *result)
{
    if (result == NULL)
        return MATRIX_INVALID_POINTER;

    if (matrix_validate(A) != MATRIX_OK)
        return matrix_validate(A);

    if (A->rows != 1 && A->cols != 1)
        return MATRIX_NOT_VECTOR;

    float sum = 0.0f;

    for (uint8_t i = 0; i < A->rows * A->cols; i++)
        sum += A->data[i] * A->data[i];

    *result = sqrtf(sum);

    return MATRIX_OK;
}


// Normalize a vector to unit magnitude.
Matrix_Status matrix_normalize(Matrix *A)
{
    float magnitude;

    Matrix_Status status = matrix_norm(A, &magnitude);

    if (status != MATRIX_OK)
        return status;

    if (magnitude < 1e-6f)
        return MATRIX_ZERO_VECTOR;

    for (uint8_t i = 0; i < A->rows * A->cols; i++)
        A->data[i] /= magnitude;

    return MATRIX_OK;
}


// Print a matrix for debugging.
void matrix_print(const Matrix *A)
{
    if (matrix_validate(A) != MATRIX_OK) {
        printf("Invalid matrix\n");
        return;
    }

    for (uint8_t i = 0; i < A->rows; i++) {
        printf("[ ");

        for (uint8_t j = 0; j < A->cols; j++)
            printf("%8.4f ", MAT((*A), i, j));

        printf("]\n");
    }
}

// Print a matrix status for debugging.
void print_matrix_status(Matrix_Status status)
{
    switch (status)
    {
        case MATRIX_OK:                 printf("MATRIX_OK\n"); break;
        case MATRIX_INVALID_DIMENSIONS: printf("MATRIX_INVALID_DIMENSIONS\n"); break;
        case MATRIX_SINGULAR:           printf("MATRIX_SINGULAR\n"); break;
        case MATRIX_INVALID_POINTER:    printf("MATRIX_INVALID_POINTER\n"); break;
        case MATRIX_NOT_VECTOR:         printf("MATRIX_NOT_VECTOR\n"); break;
        case MATRIX_ZERO_VECTOR:        printf("MATRIX_ZERO_VECTOR\n"); break;
        default:                        printf("UNKNOWN_STATUS\n"); break;
    }
}