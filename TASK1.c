#include <stdio.h>

/* ── display the menu ── */
void showMenu(void)
{
    printf("\n╔══════════════════════════╗\n");
    printf("║    CODEALPHA CALCULATOR  ║\n");
    printf("╠══════════════════════════╣\n");
    printf("║  1. Addition    (+)      ║\n");
    printf("║  2. Subtraction (-)      ║\n");
    printf("║  3. Multiplication (*)   ║\n");
    printf("║  4. Division    (/)      ║\n");
    printf("║  5. Exit                 ║\n");
    printf("╚══════════════════════════╝\n");
    printf("  Enter your choice: ");
}

int main(void)
{
    int choice;
    double num1, num2, result;

    printf("\n  Welcome to the CodeAlpha Calculator!\n");

    while (1)
    {
        showMenu();
        scanf("%d", &choice);

        if (choice == 5)
        {
            printf("\n  Thank you for using CodeAlpha Calculator. Goodbye!\n\n");
            break;
        }

        if (choice < 1 || choice > 4)
        {
            printf("  [!] Invalid choice. Please enter 1-5.\n");
            continue;
        }

        printf("  Enter first  number: ");
        scanf("%lf", &num1);
        printf("  Enter second number: ");
        scanf("%lf", &num2);

        switch (choice)
        {
        /* ── Addition ── */
        case 1:
            result = num1 + num2;
            printf("\n  Result: %.2lf + %.2lf = %.2lf\n", num1, num2, result);
            break;

        /* ── Subtraction ── */
        case 2:
            result = num1 - num2;
            printf("\n  Result: %.2lf - %.2lf = %.2lf\n", num1, num2, result);
            break;

        /* ── Multiplication ── */
        case 3:
            result = num1 * num2;
            printf("\n  Result: %.2lf * %.2lf = %.2lf\n", num1, num2, result);
            break;

        /* ── Division ── */
        case 4:
            if (num2 == 0)
            {
                printf("\n  [!] Error: Division by zero is not allowed.\n");
            }
            else
            {
                result = num1 / num2;
                printf("\n  Result: %.2lf / %.2lf = %.2lf\n", num1, num2, result);
            }
            break;
        }
    }

    return 0;
}
