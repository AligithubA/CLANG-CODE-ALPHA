#include <stdio.h>
#include <stdlib.h>

#define MAX 10 /* maximum matrix dimension */

/* ── type alias for convenience ── */
typedef int Matrix[MAX][MAX];

/* ─────────────────────────────────────────────
   Input / Output helpers
───────────────────────────────────────────── */

void inputMatrix(Matrix m, int rows, int cols, const char *name)
{
    printf("\n  Enter elements of Matrix %s (%d x %d):\n", name, rows, cols);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            printf("    %s[%d][%d] = ", name, i + 1, j + 1);
            scanf("%d", &m[i][j]);
        }
}

void printMatrix(Matrix m, int rows, int cols, const char *title)
{
    printf("\n  %s\n", title);
    printf("  ");
    for (int j = 0; j < cols; j++)
        printf("------");
    printf("\n");
    for (int i = 0; i < rows; i++)
    {
        printf("  |");
        for (int j = 0; j < cols; j++)
            printf(" %4d ", m[i][j]);
        printf("|\n");
    }
    printf("  ");
    for (int j = 0; j < cols; j++)
        printf("------");
    printf("\n");
}

/* ─────────────────────────────────────────────
   Core matrix operations
───────────────────────────────────────────── */

/* Addition: result = A + B  (same dimensions required) */
void addMatrices(Matrix A, Matrix B, Matrix result, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result[i][j] = A[i][j] + B[i][j];
}

/* Multiplication: C[r1 x c2] = A[r1 x c1] * B[c1 x c2] */
void multiplyMatrices(Matrix A, Matrix B, Matrix result,
                      int r1, int c1, int c2)
{
    /* zero-initialise result */
    for (int i = 0; i < r1; i++)
        for (int j = 0; j < c2; j++)
            result[i][j] = 0;

    for (int i = 0; i < r1; i++)
        for (int j = 0; j < c2; j++)
            for (int k = 0; k < c1; k++)
                result[i][j] += A[i][k] * B[k][j];
}

/* Transpose: T[cols x rows] of A[rows x cols] */
void transposeMatrix(Matrix A, Matrix T, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            T[j][i] = A[i][j];
}

/* ─────────────────────────────────────────────
   Menu
───────────────────────────────────────────── */

void showMenu(void)
{
    printf("\n╔══════════════════════════════╗\n");
    printf("║   CODEALPHA MATRIX TOOLKIT   ║\n");
    printf("╠══════════════════════════════╣\n");
    printf("║  1. Matrix Addition          ║\n");
    printf("║  2. Matrix Multiplication    ║\n");
    printf("║  3. Matrix Transpose         ║\n");
    printf("║  4. Exit                     ║\n");
    printf("╚══════════════════════════════╝\n");
    printf("  Enter your choice: ");
}

int main(void)
{
    Matrix A, B, result;
    int rows1, cols1, rows2, cols2;
    int choice;

    printf("\n  Welcome to CodeAlpha Matrix Operations Tool!\n");

    while (1)
    {
        showMenu();
        scanf("%d", &choice);

        switch (choice)
        {

        /* ── Addition ── */
        case 1:
            printf("\n  [Addition] Enter dimensions of both matrices (must be equal).\n");
            printf("  Rows: ");
            scanf("%d", &rows1);
            printf("  Cols: ");
            scanf("%d", &cols1);

            if (rows1 <= 0 || cols1 <= 0 || rows1 > MAX || cols1 > MAX)
            {
                printf("  [!] Invalid size. Max is %d x %d.\n", MAX, MAX);
                break;
            }

            inputMatrix(A, rows1, cols1, "A");
            inputMatrix(B, rows1, cols1, "B");

            addMatrices(A, B, result, rows1, cols1);

            printMatrix(A, rows1, cols1, "Matrix A:");
            printMatrix(B, rows1, cols1, "Matrix B:");
            printMatrix(result, rows1, cols1, "A + B =");
            break;

        /* ── Multiplication ── */
        case 2:
            printf("\n  [Multiplication] Enter dimensions of Matrix A:\n");
            printf("  Rows: ");
            scanf("%d", &rows1);
            printf("  Cols: ");
            scanf("%d", &cols1);
            printf("  Enter dimensions of Matrix B:\n");
            printf("  Rows must equal %d (cols of A).\n", cols1);
            rows2 = cols1; /* required for multiplication */
            printf("  Cols of B: ");
            scanf("%d", &cols2);

            if (rows1 <= 0 || cols1 <= 0 || cols2 <= 0 ||
                rows1 > MAX || cols2 > MAX)
            {
                printf("  [!] Invalid dimensions.\n");
                break;
            }

            inputMatrix(A, rows1, cols1, "A");
            inputMatrix(B, rows2, cols2, "B");

            multiplyMatrices(A, B, result, rows1, cols1, cols2);

            printMatrix(A, rows1, cols1, "Matrix A:");
            printMatrix(B, rows2, cols2, "Matrix B:");
            printMatrix(result, rows1, cols2, "A x B =");
            break;

        /* ── Transpose ── */
        case 3:
            printf("\n  [Transpose] Enter dimensions of Matrix A:\n");
            printf("  Rows: ");
            scanf("%d", &rows1);
            printf("  Cols: ");
            scanf("%d", &cols1);

            if (rows1 <= 0 || cols1 <= 0 || rows1 > MAX || cols1 > MAX)
            {
                printf("  [!] Invalid size.\n");
                break;
            }

            inputMatrix(A, rows1, cols1, "A");
            transposeMatrix(A, result, rows1, cols1);

            printMatrix(A, rows1, cols1, "Original Matrix A:");
            printMatrix(result, cols1, rows1, "Transpose of A:");
            break;

        /* ── Exit ── */
        case 4:
            printf("\n  Thank you for using CodeAlpha Matrix Toolkit. Goodbye!\n\n");
            return 0;

        default:
            printf("  [!] Invalid choice. Please enter 1-4.\n");
        }
    }

    return 0;
}
