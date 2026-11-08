
#include <stdio.h>
#include <math.h>
#include "matrix.h"

/* =========================================================
   SIMPLE TEST FRAMEWORK
   ========================================================= */

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define EPSILON 0.0001f

#define CHECK(condition, description) do {                    \
    tests_run++;                                               \
    if (condition) {                                           \
        tests_passed++;                                        \
        printf("[PASS] %s\n", description);                    \
    } else {                                                   \
        tests_failed++;                                        \
        printf("[FAIL] %s (line %d)\n", description, __LINE__);\
    }                                                          \
} while (0)

#define CHECK_STATUS(actual, expected, description) \
    CHECK((actual) == (expected), description)

#define CHECK_FLOAT(actual, expected, description) \
    CHECK(fabsf((actual) - (expected)) < EPSILON, description)

/* Check every element of a matrix against an expected array. */
static void check_matrix(
    const Matrix *M,
    const float *expected,
    const char *description)
{
    int correct = 1;

    if (M == NULL || M->data == NULL) {
        correct = 0;
    } else {
        for (uint8_t i = 0; i < M->rows; i++) {
            for (uint8_t j = 0; j < M->cols; j++) {
                float actual = MAT((*M), i, j);
                float target = expected[i * M->cols + j];

                if (fabsf(actual - target) >= EPSILON) {
                    correct = 0;
                }
            }
        }
    }

    CHECK(correct, description);
}

/* =========================================================
   1. INITIALIZATION
   ========================================================= */

static void test_initialization(void)
{
    Matrix A;
    float data[16] = {0};

    CHECK_STATUS(
        matrix_init(&A, 2, 3, data),
        MATRIX_OK,
        "Initialize valid 2x3 matrix"
    );

    CHECK(A.rows == 2 && A.cols == 3,
          "Initialization stores correct dimensions");

    CHECK(A.data == data,
          "Initialization stores correct data pointer");

    CHECK_STATUS(
        matrix_init(NULL, 2, 2, data),
        MATRIX_INVALID_POINTER,
        "Reject NULL matrix pointer"
    );

    CHECK_STATUS(
        matrix_init(&A, 2, 2, NULL),
        MATRIX_INVALID_POINTER,
        "Reject NULL data pointer"
    );

    CHECK_STATUS(
        matrix_init(&A, 0, 2, data),
        MATRIX_INVALID_DIMENSIONS,
        "Reject zero rows"
    );

    CHECK_STATUS(
        matrix_init(&A, 2, 0, data),
        MATRIX_INVALID_DIMENSIONS,
        "Reject zero columns"
    );

    CHECK_STATUS(
        matrix_init(&A, 5, 2, data),
        MATRIX_INVALID_DIMENSIONS,
        "Reject more than MATRIX_MAX_DIM rows"
    );

    CHECK_STATUS(
        matrix_init(&A, 2, 5, data),
        MATRIX_INVALID_DIMENSIONS,
        "Reject more than MATRIX_MAX_DIM columns"
    );
}

/* =========================================================
   2. ZERO
   ========================================================= */

static void test_zero(void)
{
    float data[6] = {1, 2, 3, 4, 5, 6};
    float expected[6] = {0, 0, 0, 0, 0, 0};

    Matrix A;
    matrix_init(&A, 2, 3, data);

    CHECK_STATUS(
        matrix_zero(&A),
        MATRIX_OK,
        "Zero a matrix"
    );

    check_matrix(&A, expected,
                 "All matrix elements become zero");

    CHECK_STATUS(
        matrix_zero(NULL),
        MATRIX_INVALID_POINTER,
        "Reject NULL matrix in zero"
    );
}

/* =========================================================
   3. IDENTITY
   ========================================================= */

static void test_identity(void)
{
    float data[9] = {0};
    float expected[9] = {
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    };

    Matrix A;
    matrix_init(&A, 3, 3, data);

    CHECK_STATUS(
        matrix_identity(&A),
        MATRIX_OK,
        "Create 3x3 identity matrix"
    );

    check_matrix(&A, expected,
                 "Identity matrix has correct elements");

    float rectangular_data[6] = {0};
    Matrix B;
    matrix_init(&B, 2, 3, rectangular_data);

    CHECK_STATUS(
        matrix_identity(&B),
        MATRIX_INVALID_DIMENSIONS,
        "Reject identity matrix with unequal dimensions"
    );

    CHECK_STATUS(
        matrix_identity(NULL),
        MATRIX_INVALID_POINTER,
        "Reject NULL matrix in identity"
    );
}

/* =========================================================
   4. COPY
   ========================================================= */

static void test_copy(void)
{
    float source_data[4] = {1, 2, 3, 4};
    float destination_data[4] = {0};

    Matrix A, B;
    matrix_init(&A, 2, 2, source_data);
    matrix_init(&B, 2, 2, destination_data);

    CHECK_STATUS(
        matrix_copy(&B, &A),
        MATRIX_OK,
        "Copy matrix"
    );

    check_matrix(&B, source_data,
                 "Copied matrix contains correct values");

    CHECK_STATUS(
        matrix_copy(&A, &A),
        MATRIX_OK,
        "Copy matrix onto itself"
    );

    float wrong_size_data[6] = {0};
    Matrix C;
    matrix_init(&C, 2, 3, wrong_size_data);

    CHECK_STATUS(
        matrix_copy(&C, &A),
        MATRIX_INVALID_DIMENSIONS,
        "Reject copy between different dimensions"
    );

    CHECK_STATUS(
        matrix_copy(NULL, &A),
        MATRIX_INVALID_POINTER,
        "Reject NULL destination in copy"
    );

    CHECK_STATUS(
        matrix_copy(&B, NULL),
        MATRIX_INVALID_POINTER,
        "Reject NULL source in copy"
    );
}

/* =========================================================
   5. ADDITION
   ========================================================= */

static void test_add(void)
{
    float a_data[4] = {1, 2, 3, 4};
    float b_data[4] = {5, 6, 7, 8};
    float c_data[4] = {0};

    float expected[4] = {6, 8, 10, 12};

    Matrix A, B, C;
    matrix_init(&A, 2, 2, a_data);
    matrix_init(&B, 2, 2, b_data);
    matrix_init(&C, 2, 2, c_data);

    CHECK_STATUS(
        matrix_add(&A, &B, &C),
        MATRIX_OK,
        "Add two matrices"
    );

    check_matrix(&C, expected,
                 "Addition produces correct values");

    /* Test output aliasing the first input. */
    CHECK_STATUS(
        matrix_add(&A, &B, &A),
        MATRIX_OK,
        "Addition with output aliasing first input"
    );

    check_matrix(&A, expected,
                 "Aliased addition produces correct values");

    float wrong_data[6] = {0};
    Matrix D;
    matrix_init(&D, 2, 3, wrong_data);

    CHECK_STATUS(
        matrix_add(&A, &D, &C),
        MATRIX_INVALID_DIMENSIONS,
        "Reject addition with incompatible dimensions"
    );

    CHECK_STATUS(
        matrix_add(NULL, &B, &C),
        MATRIX_INVALID_POINTER,
        "Reject NULL first operand in addition"
    );
}

/* =========================================================
   6. SUBTRACTION
   ========================================================= */

static void test_subtract(void)
{
    float a_data[4] = {5, 7, 9, 11};
    float b_data[4] = {1, 2, 3, 4};
    float c_data[4] = {0};

    float expected[4] = {4, 5, 6, 7};

    Matrix A, B, C;
    matrix_init(&A, 2, 2, a_data);
    matrix_init(&B, 2, 2, b_data);
    matrix_init(&C, 2, 2, c_data);

    CHECK_STATUS(
        matrix_subtract(&A, &B, &C),
        MATRIX_OK,
        "Subtract two matrices"
    );

    check_matrix(&C, expected,
                 "Subtraction produces correct values");

    CHECK_STATUS(
        matrix_subtract(&A, &B, &A),
        MATRIX_OK,
        "Subtraction with output aliasing first input"
    );

    check_matrix(&A, expected,
                 "Aliased subtraction produces correct values");

    CHECK_STATUS(
        matrix_subtract(NULL, &B, &C),
        MATRIX_INVALID_POINTER,
        "Reject NULL operand in subtraction"
    );
}

/* =========================================================
   7. MULTIPLICATION
   ========================================================= */

static void test_multiply(void)
{
    float a_data[6] = {
        1, 2, 3,
        4, 5, 6
    };

    float b_data[6] = {
        7, 8,
        9, 10,
        11, 12
    };

    float c_data[4] = {0};

    float expected[4] = {
        58, 64,
        139, 154
    };

    Matrix A, B, C;

    matrix_init(&A, 2, 3, a_data);
    matrix_init(&B, 3, 2, b_data);
    matrix_init(&C, 2, 2, c_data);

    CHECK_STATUS(
        matrix_multiply(&A, &B, &C),
        MATRIX_OK,
        "Multiply 2x3 by 3x2 matrix"
    );

    check_matrix(&C, expected,
                 "Rectangular multiplication is correct");

    /* Test multiplication with output aliasing an input. */
    float alias_a[4] = {1, 2, 3, 4};
    float alias_b[4] = {5, 6, 7, 8};

    Matrix D, E;
    matrix_init(&D, 2, 2, alias_a);
    matrix_init(&E, 2, 2, alias_b);

    float alias_expected[4] = {19, 22, 43, 50};

    CHECK_STATUS(
        matrix_multiply(&D, &E, &D),
        MATRIX_OK,
        "Multiplication with output aliasing first input"
    );

    check_matrix(&D, alias_expected,
                 "Aliased multiplication is correct");

    float incompatible_data[4] = {0};
    Matrix F;
    matrix_init(&F, 2, 2, incompatible_data);

    CHECK_STATUS(
        matrix_multiply(&A, &F, &C),
        MATRIX_INVALID_DIMENSIONS,
        "Reject multiplication with incompatible inner dimensions"
    );

    CHECK_STATUS(
        matrix_multiply(NULL, &B, &C),
        MATRIX_INVALID_POINTER,
        "Reject NULL operand in multiplication"
    );
}

/* =========================================================
   8. SCALAR MULTIPLICATION
   ========================================================= */

static void test_scale(void)
{
    float a_data[4] = {1, 2, 3, 4};
    float b_data[4] = {0};

    float expected[4] = {2, 4, 6, 8};

    Matrix A, B;
    matrix_init(&A, 2, 2, a_data);
    matrix_init(&B, 2, 2, b_data);

    CHECK_STATUS(
        matrix_scale(&A, 2.0f, &B),
        MATRIX_OK,
        "Scale matrix by 2"
    );

    check_matrix(&B, expected,
                 "Scalar multiplication produces correct values");

    float zero_expected[4] = {0};

    CHECK_STATUS(
        matrix_scale(&A, 0.0f, &B),
        MATRIX_OK,
        "Scale matrix by zero"
    );

    check_matrix(&B, zero_expected,
                 "Scaling by zero produces zero matrix");

    CHECK_STATUS(
        matrix_scale(&A, -1.0f, &A),
        MATRIX_OK,
        "Scale matrix in place"
    );

    float negative_expected[4] = {-1, -2, -3, -4};

    check_matrix(&A, negative_expected,
                 "In-place negative scaling is correct");

    CHECK_STATUS(
        matrix_scale(NULL, 2.0f, &B),
        MATRIX_INVALID_POINTER,
        "Reject NULL input in scale"
    );
}

/* =========================================================
   9. TRANSPOSE
   ========================================================= */

static void test_transpose(void)
{
    float a_data[6] = {
        1, 2, 3,
        4, 5, 6
    };

    float b_data[6] = {0};

    float expected[6] = {
        1, 4,
        2, 5,
        3, 6
    };

    Matrix A, B;
    matrix_init(&A, 2, 3, a_data);
    matrix_init(&B, 3, 2, b_data);

    CHECK_STATUS(
        matrix_transpose(&A, &B),
        MATRIX_OK,
        "Transpose 2x3 matrix"
    );

    check_matrix(&B, expected,
                 "Transpose produces correct values");

    /* Same underlying storage, different matrix dimensions. */
    float alias_data[6] = {
        1, 2, 3,
        4, 5, 6
    };

    Matrix C, D;
    matrix_init(&C, 2, 3, alias_data);
    matrix_init(&D, 3, 2, alias_data);

    CHECK_STATUS(
        matrix_transpose(&C, &D),
        MATRIX_OK,
        "Transpose using overlapping input/output storage"
    );

    check_matrix(&D, expected,
                 "Overlapping transpose produces correct values");

    CHECK_STATUS(
        matrix_transpose(NULL, &B),
        MATRIX_INVALID_POINTER,
        "Reject NULL input in transpose"
    );
}

/* =========================================================
   10. INVERSE
   ========================================================= */

static void test_inverse(void)
{
    float a_data[4] = {
        4, 7,
        2, 6
    };

    float inverse_data[4] = {0};

    float expected[4] = {
        0.6f, -0.7f,
        -0.2f, 0.4f
    };

    Matrix A, B;
    matrix_init(&A, 2, 2, a_data);
    matrix_init(&B, 2, 2, inverse_data);

    CHECK_STATUS(
        matrix_inverse(&A, &B),
        MATRIX_OK,
        "Invert nonsingular 2x2 matrix"
    );

    check_matrix(&B, expected,
                 "Inverse contains correct values");

    /* Verify A * inverse(A) = identity. */
    float product_data[4] = {0};
    float identity_expected[4] = {
        1, 0,
        0, 1
    };

    Matrix C;
    matrix_init(&C, 2, 2, product_data);

    matrix_multiply(&A, &B, &C);

    check_matrix(&C, identity_expected,
                 "A multiplied by inverse(A) equals identity");

    /* Test a matrix requiring row pivoting. */
    float pivot_data[4] = {
        0, 1,
        1, 0
    };

    float pivot_inverse_data[4] = {0};

    float pivot_expected[4] = {
        0, 1,
        1, 0
    };

    Matrix D, E;
    matrix_init(&D, 2, 2, pivot_data);
    matrix_init(&E, 2, 2, pivot_inverse_data);

    CHECK_STATUS(
        matrix_inverse(&D, &E),
        MATRIX_OK,
        "Invert matrix requiring row pivoting"
    );

    check_matrix(&E, pivot_expected,
                 "Pivoted inverse is correct");

    /* Singular matrix. */
    float singular_data[4] = {
        1, 2,
        2, 4
    };

    float singular_inverse_data[4] = {0};

    Matrix F, G;
    matrix_init(&F, 2, 2, singular_data);
    matrix_init(&G, 2, 2, singular_inverse_data);

    CHECK_STATUS(
        matrix_inverse(&F, &G),
        MATRIX_SINGULAR,
        "Reject singular matrix"
    );

    /* Nonsquare matrix. */
    float rectangular_data[6] = {1, 2, 3, 4, 5, 6};
    float rectangular_inverse_data[6] = {0};

    Matrix H, I;
    matrix_init(&H, 2, 3, rectangular_data);
    matrix_init(&I, 2, 3, rectangular_inverse_data);

    CHECK_STATUS(
        matrix_inverse(&H, &I),
        MATRIX_INVALID_DIMENSIONS,
        "Reject nonsquare matrix inversion"
    );

    CHECK_STATUS(
        matrix_inverse(NULL, &B),
        MATRIX_INVALID_POINTER,
        "Reject NULL input in inverse"
    );

    /* Test inversion using overlapping storage. */
    float inplace_data[4] = {4, 7, 2, 6};
    Matrix J, K;
    matrix_init(&J, 2, 2, inplace_data);
    matrix_init(&K, 2, 2, inplace_data);

    CHECK_STATUS(
        matrix_inverse(&J, &K),
        MATRIX_OK,
        "Invert matrix using overlapping input/output storage"
    );

    check_matrix(&K, expected,
                 "Overlapping inverse produces correct values");
}

/* =========================================================
   11. DOT PRODUCT
   ========================================================= */

static void test_dot(void)
{
    float a_data[3] = {1, 2, 3};
    float b_data[3] = {4, 5, 6};

    Matrix A, B;
    matrix_init(&A, 1, 3, a_data);
    matrix_init(&B, 1, 3, b_data);

    float result = 0;

    CHECK_STATUS(
        matrix_dot(&A, &B, &result),
        MATRIX_OK,
        "Calculate vector dot product"
    );

    CHECK_FLOAT(result, 32.0f,
                "Dot product equals 32");

    /* Row vector multiplied by column vector. */
    float column_data[3] = {4, 5, 6};
    Matrix C;
    matrix_init(&C, 3, 1, column_data);

    CHECK_STATUS(
        matrix_dot(&A, &C, &result),
        MATRIX_OK,
        "Dot product between row and column vectors"
    );

    CHECK_FLOAT(result, 32.0f,
                "Row/column dot product is correct");

    /* Non-vector matrix. */
    float matrix_data[4] = {1, 2, 3, 4};
    Matrix D;
    matrix_init(&D, 2, 2, matrix_data);

    CHECK_STATUS(
        matrix_dot(&D, &B, &result),
        MATRIX_NOT_VECTOR,
        "Reject non-vector in dot product"
    );

    /* Different vector lengths. */
    float short_data[2] = {1, 2};
    Matrix E;
    matrix_init(&E, 1, 2, short_data);

    CHECK_STATUS(
        matrix_dot(&A, &E, &result),
        MATRIX_INVALID_DIMENSIONS,
        "Reject vectors with different lengths"
    );

    CHECK_STATUS(
        matrix_dot(&A, &B, NULL),
        MATRIX_INVALID_POINTER,
        "Reject NULL dot product result pointer"
    );
}

/* =========================================================
   12. NORM
   ========================================================= */

static void test_norm(void)
{
    float data[3] = {3, 4, 0};
    Matrix A;
    matrix_init(&A, 1, 3, data);

    float result = 0;

    CHECK_STATUS(
        matrix_norm(&A, &result),
        MATRIX_OK,
        "Calculate vector norm"
    );

    CHECK_FLOAT(result, 5.0f,
                "Vector norm equals 5");

    /* Column vector. */
    float column_data[3] = {3, 4, 0};
    Matrix B;
    matrix_init(&B, 3, 1, column_data);

    CHECK_STATUS(
        matrix_norm(&B, &result),
        MATRIX_OK,
        "Calculate column vector norm"
    );

    CHECK_FLOAT(result, 5.0f,
                "Column vector norm equals 5");

    float matrix_data[4] = {1, 2, 3, 4};
    Matrix C;
    matrix_init(&C, 2, 2, matrix_data);

    CHECK_STATUS(
        matrix_norm(&C, &result),
        MATRIX_NOT_VECTOR,
        "Reject non-vector in norm"
    );

    CHECK_STATUS(
        matrix_norm(&A, NULL),
        MATRIX_INVALID_POINTER,
        "Reject NULL norm result pointer"
    );
}

/* =========================================================
   13. NORMALIZE
   ========================================================= */

static void test_normalize(void)
{
    float data[3] = {3, 4, 0};
    float expected[3] = {0.6f, 0.8f, 0};

    Matrix A;
    matrix_init(&A, 1, 3, data);

    CHECK_STATUS(
        matrix_normalize(&A),
        MATRIX_OK,
        "Normalize vector"
    );

    check_matrix(&A, expected,
                 "Normalized vector has correct values");

    float result = 0;

    matrix_norm(&A, &result);

    CHECK_FLOAT(result, 1.0f,
                "Normalized vector has unit magnitude");

    /* Test column vector. */
    float column_data[3] = {0, 0, 5};
    float column_expected[3] = {0, 0, 1};

    Matrix B;
    matrix_init(&B, 3, 1, column_data);

    CHECK_STATUS(
        matrix_normalize(&B),
        MATRIX_OK,
        "Normalize column vector"
    );

    check_matrix(&B, column_expected,
                 "Normalized column vector is correct");

    /* Zero vector. */
    float zero_data[3] = {0, 0, 0};
    Matrix C;
    matrix_init(&C, 1, 3, zero_data);

    CHECK_STATUS(
        matrix_normalize(&C),
        MATRIX_ZERO_VECTOR,
        "Reject normalization of zero vector"
    );

    /* Non-vector. */
    float matrix_data[4] = {1, 2, 3, 4};
    Matrix D;
    matrix_init(&D, 2, 2, matrix_data);

    CHECK_STATUS(
        matrix_normalize(&D),
        MATRIX_NOT_VECTOR,
        "Reject normalization of non-vector"
    );

    CHECK_STATUS(
        matrix_normalize(NULL),
        MATRIX_INVALID_POINTER,
        "Reject NULL matrix in normalize"
    );
}

/* =========================================================
   14. PRINT
   ========================================================= */

static void test_print(void)
{
    float data[4] = {
        1, 2,
        3, 4
    };

    Matrix A;
    matrix_init(&A, 2, 2, data);

    printf("\nExpected matrix print output:\n");

    matrix_print(&A);

    printf("Expected invalid matrix print output:\n");

    matrix_print(NULL);

    /* matrix_print returns void, so this is a smoke test. */
    CHECK(1, "matrix_print executes without crashing");
}

/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    printf("\n====================================\n");
    printf("       MATRIX LIBRARY TESTS\n");
    printf("====================================\n\n");

    test_initialization();
    test_zero();
    test_identity();
    test_copy();
    test_add();
    test_subtract();
    test_multiply();
    test_scale();
    test_transpose();
    test_inverse();
    test_dot();
    test_norm();
    test_normalize();
    test_print();

    printf("\n====================================\n");
    printf("             TEST RESULTS\n");
    printf("====================================\n");

    printf("Tests run:    %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);

    if (tests_failed == 0) {
        printf("\nALL TESTS PASSED!\n");
    } else {
        printf("\nSOME TESTS FAILED.\n");
    }

    printf("====================================\n");

    return tests_failed == 0 ? 0 : 1;
}