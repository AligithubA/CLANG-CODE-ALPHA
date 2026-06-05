#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ── Constants ── */
#define DATA_FILE "bank.dat"
#define MAX_NAME 50
#define MAX_TRANS 5 /* keep last N transactions */
#define TRANS_LEN 60

/* ── Transaction record (stored inside account) ── */
typedef struct
{
    char description[TRANS_LEN];
} Transaction;

/* ── Bank Account structure ── */
typedef struct
{
    long accountNo;
    char holderName[MAX_NAME];
    char accountType[20]; /* "Savings" or "Current" */
    double balance;
    int transCount;                 /* number of stored transactions */
    Transaction history[MAX_TRANS]; /* circular last-N log */
} Account;

/* ─────────────────────────────────────────────
   File helpers
───────────────────────────────────────────── */

Account *loadAll(int *count)
{
    FILE *fp = fopen(DATA_FILE, "rb");
    Account *arr = NULL;
    *count = 0;

    if (!fp)
        return NULL;

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);
    *count = (int)(size / sizeof(Account));

    if (*count == 0)
    {
        fclose(fp);
        return NULL;
    }

    arr = (Account *)malloc((*count) * sizeof(Account));
    if (!arr)
    {
        fclose(fp);
        return NULL;
    }

    fread(arr, sizeof(Account), *count, fp);
    fclose(fp);
    return arr;
}

void saveAll(Account *arr, int count)
{
    FILE *fp = fopen(DATA_FILE, "wb");
    if (!fp)
    {
        printf("  [!] Cannot write to data file.\n");
        return;
    }
    fwrite(arr, sizeof(Account), count, fp);
    fclose(fp);
}

/* find account index by account number; -1 if not found */
int findAccount(Account *arr, int count, long accNo)
{
    for (int i = 0; i < count; i++)
        if (arr[i].accountNo == accNo)
            return i;
    return -1;
}

/* ─────────────────────────────────────────────
   Transaction logger (circular buffer)
───────────────────────────────────────────── */
void logTransaction(Account *acc, const char *desc)
{
    /* shift history up, discard oldest if full */
    if (acc->transCount >= MAX_TRANS)
    {
        for (int i = 0; i < MAX_TRANS - 1; i++)
            acc->history[i] = acc->history[i + 1];
        acc->transCount = MAX_TRANS - 1;
    }
    strncpy(acc->history[acc->transCount].description, desc, TRANS_LEN - 1);
    acc->transCount++;
}

/* ─────────────────────────────────────────────
   Generate a simple timestamp string
───────────────────────────────────────────── */
void timestamp(char *buf, size_t len)
{
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buf, len, "%Y-%m-%d %H:%M", tm_info);
}

/* ─────────────────────────────────────────────
   Operations
───────────────────────────────────────────── */

/* CREATE ACCOUNT */
void createAccount(void)
{
    Account a;
    memset(&a, 0, sizeof(Account));

    printf("\n  ── Create New Account ──\n");

    /* generate account number from timestamp for uniqueness */
    a.accountNo = (long)time(NULL) % 9000000 + 1000000;

    printf("  Account Holder Name : ");
    scanf(" %[^\n]", a.holderName);

    printf("  Account Type (1=Savings / 2=Current): ");
    int t;
    scanf("%d", &t);
    strncpy(a.accountType, (t == 2) ? "Current" : "Savings", 19);

    printf("  Initial Deposit (min 500): ");
    scanf("%lf", &a.balance);
    if (a.balance < 500)
    {
        printf("  [!] Minimum deposit is 500. Setting to 500.\n");
        a.balance = 500;
    }

    char ts[20], desc[TRANS_LEN];
    timestamp(ts, sizeof(ts));
    snprintf(desc, TRANS_LEN, "[%s] Account opened. Deposit: +%.2f", ts, a.balance);
    logTransaction(&a, desc);

    /* append to file */
    FILE *fp = fopen(DATA_FILE, "ab");
    if (!fp)
    {
        printf("  [!] Could not open data file.\n");
        return;
    }
    fwrite(&a, sizeof(Account), 1, fp);
    fclose(fp);

    printf("\n  ╔════════════════════════════════╗\n");
    printf("  ║  Account Created Successfully! ║\n");
    printf("  ╠════════════════════════════════╣\n");
    printf("  ║  Account No  : %-15ld ║\n", a.accountNo);
    printf("  ║  Name        : %-15s ║\n", a.holderName);
    printf("  ║  Type        : %-15s ║\n", a.accountType);
    printf("  ║  Balance     : %-15.2f ║\n", a.balance);
    printf("  ╚════════════════════════════════╝\n");
}

/* DEPOSIT */
void deposit(void)
{
    long accNo;
    double amount;
    printf("\n  ── Deposit Money ──\n");
    printf("  Account Number: ");
    scanf("%ld", &accNo);

    int count;
    Account *arr = loadAll(&count);
    if (!arr)
    {
        printf("  No accounts found.\n");
        return;
    }

    int idx = findAccount(arr, count, accNo);
    if (idx == -1)
    {
        printf("  [!] Account not found.\n");
        free(arr);
        return;
    }

    printf("  Amount to Deposit: ");
    scanf("%lf", &amount);
    if (amount <= 0)
    {
        printf("  [!] Invalid amount.\n");
        free(arr);
        return;
    }

    arr[idx].balance += amount;

    char ts[20], desc[TRANS_LEN];
    timestamp(ts, sizeof(ts));
    snprintf(desc, TRANS_LEN, "[%s] Deposit: +%.2f | Bal: %.2f", ts, amount, arr[idx].balance);
    logTransaction(&arr[idx], desc);

    saveAll(arr, count);
    printf("  [✓] Deposited %.2f successfully.\n", amount);
    printf("  Updated Balance: %.2f\n", arr[idx].balance);
    free(arr);
}

/* WITHDRAW */
void withdraw(void)
{
    long accNo;
    double amount;
    printf("\n  ── Withdraw Money ──\n");
    printf("  Account Number: ");
    scanf("%ld", &accNo);

    int count;
    Account *arr = loadAll(&count);
    if (!arr)
    {
        printf("  No accounts found.\n");
        return;
    }

    int idx = findAccount(arr, count, accNo);
    if (idx == -1)
    {
        printf("  [!] Account not found.\n");
        free(arr);
        return;
    }

    printf("  Amount to Withdraw: ");
    scanf("%lf", &amount);
    if (amount <= 0)
    {
        printf("  [!] Invalid amount.\n");
        free(arr);
        return;
    }

    if (amount > arr[idx].balance)
    {
        printf("  [!] Insufficient funds. Available balance: %.2f\n", arr[idx].balance);
        free(arr);
        return;
    }

    /* Savings: maintain minimum balance of 500 */
    if (strcmp(arr[idx].accountType, "Savings") == 0 &&
        (arr[idx].balance - amount) < 500)
    {
        printf("  [!] Savings accounts must maintain a minimum balance of 500.\n");
        printf("  Maximum withdrawable: %.2f\n", arr[idx].balance - 500);
        free(arr);
        return;
    }

    arr[idx].balance -= amount;

    char ts[20], desc[TRANS_LEN];
    timestamp(ts, sizeof(ts));
    snprintf(desc, TRANS_LEN, "[%s] Withdrawal: -%.2f | Bal: %.2f", ts, amount, arr[idx].balance);
    logTransaction(&arr[idx], desc);

    saveAll(arr, count);
    printf("  [✓] Withdrawn %.2f successfully.\n", amount);
    printf("  Remaining Balance: %.2f\n", arr[idx].balance);
    free(arr);
}

/* BALANCE ENQUIRY */
void balanceEnquiry(void)
{
    long accNo;
    printf("\n  ── Balance Enquiry ──\n");
    printf("  Account Number: ");
    scanf("%ld", &accNo);

    int count;
    Account *arr = loadAll(&count);
    if (!arr)
    {
        printf("  No accounts found.\n");
        return;
    }

    int idx = findAccount(arr, count, accNo);
    if (idx == -1)
    {
        printf("  [!] Account not found.\n");
        free(arr);
        return;
    }

    printf("\n  ╔══════════════════════════════════╗\n");
    printf("  ║         ACCOUNT DETAILS          ║\n");
    printf("  ╠══════════════════════════════════╣\n");
    printf("  ║  Account No  : %-17ld ║\n", arr[idx].accountNo);
    printf("  ║  Name        : %-17s ║\n", arr[idx].holderName);
    printf("  ║  Type        : %-17s ║\n", arr[idx].accountType);
    printf("  ║  Balance     : %-17.2f ║\n", arr[idx].balance);
    printf("  ╚══════════════════════════════════╝\n");
    free(arr);
}

/* MINI STATEMENT */
void miniStatement(void)
{
    long accNo;
    printf("\n  ── Mini Statement (Last %d Transactions) ──\n", MAX_TRANS);
    printf("  Account Number: ");
    scanf("%ld", &accNo);

    int count;
    Account *arr = loadAll(&count);
    if (!arr)
    {
        printf("  No accounts found.\n");
        return;
    }

    int idx = findAccount(arr, count, accNo);
    if (idx == -1)
    {
        printf("  [!] Account not found.\n");
        free(arr);
        return;
    }

    printf("\n  Account: %ld  |  %s\n", arr[idx].accountNo, arr[idx].holderName);
    printf("  ─────────────────────────────────────────────────────\n");

    if (arr[idx].transCount == 0)
    {
        printf("  No transactions recorded.\n");
    }
    else
    {
        for (int i = 0; i < arr[idx].transCount; i++)
            printf("  %d. %s\n", i + 1, arr[idx].history[i].description);
    }

    printf("  ─────────────────────────────────────────────────────\n");
    printf("  Current Balance: %.2f\n", arr[idx].balance);
    free(arr);
}

/* DELETE ACCOUNT */
void deleteAccount(void)
{
    long accNo;
    printf("\n  ── Delete Account ──\n");
    printf("  Account Number: ");
    scanf("%ld", &accNo);

    int count;
    Account *arr = loadAll(&count);
    if (!arr)
    {
        printf("  No accounts found.\n");
        return;
    }

    int idx = findAccount(arr, count, accNo);
    if (idx == -1)
    {
        printf("  [!] Account not found.\n");
        free(arr);
        return;
    }

    printf("  Account: %ld | %s | Balance: %.2f\n",
           arr[idx].accountNo, arr[idx].holderName, arr[idx].balance);
    printf("  Confirm deletion? (y/n): ");
    char c;
    scanf(" %c", &c);
    if (c != 'y' && c != 'Y')
    {
        printf("  Deletion cancelled.\n");
        free(arr);
        return;
    }

    for (int i = idx; i < count - 1; i++)
        arr[i] = arr[i + 1];
    saveAll(arr, count - 1);
    printf("  [✓] Account deleted.\n");
    free(arr);
}

/* ─────────────────────────────────────────────
   Menu
───────────────────────────────────────────── */

void showMenu(void)
{
    printf("\n╔══════════════════════════════════════╗\n");
    printf("║    CODEALPHA BANK MANAGEMENT SYSTEM  ║\n");
    printf("╠══════════════════════════════════════╣\n");
    printf("║  1. Create New Account               ║\n");
    printf("║  2. Deposit                          ║\n");
    printf("║  3. Withdraw                         ║\n");
    printf("║  4. Balance Enquiry                  ║\n");
    printf("║  5. Mini Statement                   ║\n");
    printf("║  6. Delete Account                   ║\n");
    printf("║  7. Exit                             ║\n");
    printf("╚══════════════════════════════════════╝\n");
    printf("  Enter your choice: ");
}

int main(void)
{
    int choice;
    printf("\n  Welcome to CodeAlpha Bank Management System!\n");

    while (1)
    {
        showMenu();
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            createAccount();
            break;
        case 2:
            deposit();
            break;
        case 3:
            withdraw();
            break;
        case 4:
            balanceEnquiry();
            break;
        case 5:
            miniStatement();
            break;
        case 6:
            deleteAccount();
            break;
        case 7:
            printf("\n  Thank you for banking with CodeAlpha. Goodbye!\n\n");
            return 0;
        default:
            printf("  [!] Invalid choice. Please enter 1-7.\n");
        }
    }

    return 0;
}
