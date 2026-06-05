#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ── Constants ── */
#define MAX_NAME 50
#define MAX_COURSE 40
#define DATA_FILE "students.dat"

/* ── Student structure ── */
typedef struct
{
    int rollNo;
    char name[MAX_NAME];
    char course[MAX_COURSE];
    int age;
    float gpa;
} Student;

/* ─────────────────────────────────────────────
   Helper: load all records into a dynamic array.
   Caller must free() the returned pointer.
   *count is set to the number of records found.
───────────────────────────────────────────── */
Student *loadAll(int *count)
{
    FILE *fp = fopen(DATA_FILE, "rb");
    Student *arr = NULL;
    *count = 0;

    if (!fp)
        return NULL; /* file may not exist yet */

    /* count records */
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);
    *count = (int)(size / sizeof(Student));

    if (*count == 0)
    {
        fclose(fp);
        return NULL;
    }

    arr = (Student *)malloc((*count) * sizeof(Student));
    if (!arr)
    {
        fclose(fp);
        return NULL;
    }

    fread(arr, sizeof(Student), *count, fp);
    fclose(fp);
    return arr;
}

/* save entire array back to file */
void saveAll(Student *arr, int count)
{
    FILE *fp = fopen(DATA_FILE, "wb");
    if (!fp)
    {
        printf("  [!] Cannot open data file for writing.\n");
        return;
    }
    fwrite(arr, sizeof(Student), count, fp);
    fclose(fp);
}

/* check if roll number already exists */
int rollExists(int roll)
{
    int count;
    Student *arr = loadAll(&count);
    if (!arr)
        return 0;
    for (int i = 0; i < count; i++)
        if (arr[i].rollNo == roll)
        {
            free(arr);
            return 1;
        }
    free(arr);
    return 0;
}

/* ─────────────────────────────────────────────
   Print a single student row
───────────────────────────────────────────── */
void printHeader(void)
{
    printf("\n  %-6s  %-25s  %-20s  %-4s  %-5s\n",
           "Roll", "Name", "Course", "Age", "GPA");
    printf("  %-6s  %-25s  %-20s  %-4s  %-5s\n",
           "------", "-------------------------",
           "--------------------", "----", "-----");
}

void printStudent(const Student *s)
{
    printf("  %-6d  %-25s  %-20s  %-4d  %.2f\n",
           s->rollNo, s->name, s->course, s->age, s->gpa);
}

/* ─────────────────────────────────────────────
   CRUD Operations
───────────────────────────────────────────── */

/* ADD */
void addStudent(void)
{
    Student s;

    printf("\n  ── Add New Student ──\n");
    printf("  Roll Number : ");
    scanf("%d", &s.rollNo);

    if (rollExists(s.rollNo))
    {
        printf("  [!] Roll number %d already exists.\n", s.rollNo);
        return;
    }

    printf("  Name        : ");
    scanf(" %[^\n]", s.name);
    printf("  Course      : ");
    scanf(" %[^\n]", s.course);
    printf("  Age         : ");
    scanf("%d", &s.age);
    printf("  GPA (0-4)   : ");
    scanf("%f", &s.gpa);

    /* append to file */
    FILE *fp = fopen(DATA_FILE, "ab");
    if (!fp)
    {
        printf("  [!] Could not open data file.\n");
        return;
    }
    fwrite(&s, sizeof(Student), 1, fp);
    fclose(fp);

    printf("  [✓] Student added successfully.\n");
}

/* DISPLAY ALL */
void displayAll(void)
{
    int count;
    Student *arr = loadAll(&count);

    printf("\n  ── All Student Records ──");

    if (!arr || count == 0)
    {
        printf("\n  No records found.\n");
        free(arr);
        return;
    }

    printHeader();
    for (int i = 0; i < count; i++)
        printStudent(&arr[i]);
    printf("\n  Total records: %d\n", count);

    free(arr);
}

/* SEARCH */
void searchStudent(void)
{
    int choice;
    printf("\n  Search by:\n  1. Roll Number\n  2. Name\n  Choice: ");
    scanf("%d", &choice);

    int count;
    Student *arr = loadAll(&count);

    if (!arr || count == 0)
    {
        printf("  No records available.\n");
        free(arr);
        return;
    }

    int found = 0;

    if (choice == 1)
    {
        int roll;
        printf("  Enter Roll Number: ");
        scanf("%d", &roll);
        printHeader();
        for (int i = 0; i < count; i++)
            if (arr[i].rollNo == roll)
            {
                printStudent(&arr[i]);
                found = 1;
            }
    }
    else if (choice == 2)
    {
        char name[MAX_NAME];
        printf("  Enter Name (partial match OK): ");
        scanf(" %[^\n]", name);

        /* convert search term to lower for case-insensitive match */
        char lname[MAX_NAME];
        strncpy(lname, name, MAX_NAME);
        for (int k = 0; lname[k]; k++)
            lname[k] = tolower(lname[k]);

        printHeader();
        for (int i = 0; i < count; i++)
        {
            char tmp[MAX_NAME];
            strncpy(tmp, arr[i].name, MAX_NAME);
            for (int k = 0; tmp[k]; k++)
                tmp[k] = tolower(tmp[k]);
            if (strstr(tmp, lname))
            {
                printStudent(&arr[i]);
                found = 1;
            }
        }
    }
    else
    {
        printf("  [!] Invalid choice.\n");
    }

    if (!found)
        printf("  No matching record found.\n");
    free(arr);
}

/* UPDATE */
void updateStudent(void)
{
    int roll;
    printf("\n  ── Update Student Record ──\n");
    printf("  Enter Roll Number to update: ");
    scanf("%d", &roll);

    int count;
    Student *arr = loadAll(&count);

    if (!arr || count == 0)
    {
        printf("  No records available.\n");
        free(arr);
        return;
    }

    int idx = -1;
    for (int i = 0; i < count; i++)
        if (arr[i].rollNo == roll)
        {
            idx = i;
            break;
        }

    if (idx == -1)
    {
        printf("  [!] Student with roll %d not found.\n", roll);
        free(arr);
        return;
    }

    printf("  Current record:\n");
    printHeader();
    printStudent(&arr[idx]);

    printf("\n  Enter new details (press Enter to keep current):\n");

    char buf[MAX_NAME];

    printf("  New Name [%s]: ", arr[idx].name);
    scanf(" %[^\n]", buf);
    if (strlen(buf) > 0)
        strncpy(arr[idx].name, buf, MAX_NAME);

    printf("  New Course [%s]: ", arr[idx].course);
    scanf(" %[^\n]", buf);
    if (strlen(buf) > 0)
        strncpy(arr[idx].course, buf, MAX_COURSE);

    printf("  New Age [%d]: ", arr[idx].age);
    int age;
    scanf("%d", &age);
    if (age > 0)
        arr[idx].age = age;

    printf("  New GPA [%.2f]: ", arr[idx].gpa);
    float gpa;
    scanf("%f", &gpa);
    if (gpa >= 0)
        arr[idx].gpa = gpa;

    saveAll(arr, count);
    printf("  [✓] Record updated successfully.\n");
    free(arr);
}

/* DELETE */
void deleteStudent(void)
{
    int roll;
    printf("\n  ── Delete Student Record ──\n");
    printf("  Enter Roll Number to delete: ");
    scanf("%d", &roll);

    int count;
    Student *arr = loadAll(&count);

    if (!arr || count == 0)
    {
        printf("  No records available.\n");
        free(arr);
        return;
    }

    int idx = -1;
    for (int i = 0; i < count; i++)
        if (arr[i].rollNo == roll)
        {
            idx = i;
            break;
        }

    if (idx == -1)
    {
        printf("  [!] Student with roll %d not found.\n", roll);
        free(arr);
        return;
    }

    printf("  Record to delete:\n");
    printHeader();
    printStudent(&arr[idx]);

    printf("  Confirm delete? (y/n): ");
    char c;
    scanf(" %c", &c);
    if (c != 'y' && c != 'Y')
    {
        printf("  Deletion cancelled.\n");
        free(arr);
        return;
    }

    /* shift array left to remove the record */
    for (int i = idx; i < count - 1; i++)
        arr[i] = arr[i + 1];

    saveAll(arr, count - 1);
    printf("  [✓] Student deleted successfully.\n");
    free(arr);
}

/* ─────────────────────────────────────────────
   Menu
───────────────────────────────────────────── */

void showMenu(void)
{
    printf("\n╔═════════════════════════════════════╗\n");
    printf("║   CODEALPHA STUDENT MGMT SYSTEM     ║\n");
    printf("╠═════════════════════════════════════╣\n");
    printf("║  1. Add    Student                  ║\n");
    printf("║  2. Display All Students            ║\n");
    printf("║  3. Search Student                  ║\n");
    printf("║  4. Update Student                  ║\n");
    printf("║  5. Delete Student                  ║\n");
    printf("║  6. Exit                            ║\n");
    printf("╚═════════════════════════════════════╝\n");
    printf("  Enter your choice: ");
}

int main(void)
{
    int choice;
    printf("\n  Welcome to CodeAlpha Student Management System!\n");

    while (1)
    {
        showMenu();
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            addStudent();
            break;
        case 2:
            displayAll();
            break;
        case 3:
            searchStudent();
            break;
        case 4:
            updateStudent();
            break;
        case 5:
            deleteStudent();
            break;
        case 6:
            printf("\n  Exiting... Thank you for using the system!\n\n");
            return 0;
        default:
            printf("  [!] Invalid choice. Please enter 1-6.\n");
        }
    }

    return 0;
}
