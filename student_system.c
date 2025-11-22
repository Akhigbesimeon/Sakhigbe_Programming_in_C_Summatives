#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define FILENAME "students.txt"
#define INITIAL_CAPACITY 5
#define MAX_GRADES 3


typedef struct {
    int id;
    char name[50];
    int age;
    char course[50];
    float grades[MAX_GRADES];
    float gpa;
} Student;


Student *student_list = NULL;
int student_count = 0;
int student_capacity = 0;

// Function Prototypes
void display_menu();
void initialize_list();
void check_and_resize();
void add_student();
void display_students();
void update_student();
void delete_student();
void search_records();
void sort_records();
void generate_reports();
float calculate_gpa(float grades[]);
int get_unique_id();
void save_records();
void load_records();
void cleanup_memory();

// Core Functions
void initialize_list() {
    student_list = (Student *)malloc(INITIAL_CAPACITY * sizeof(Student));
    if (student_list == NULL) {
        perror("Error allocating initial memory");
        exit(EXIT_FAILURE);
    }
    student_capacity = INITIAL_CAPACITY;
    printf("System initialized with capacity for %d students.\n", student_capacity);
}

// Resize the array using realloc
void check_and_resize() {
    if (student_count >= student_capacity) {
        student_capacity *= 2; 
        Student *temp = (Student *)realloc(student_list, student_capacity * sizeof(Student));
        
        if (temp == NULL) {
            perror("Error reallocating memory");
            student_capacity /= 2;
            printf("Memory resize failed. Current student count: %d\n", student_count);
        } else {
            student_list = temp;
            printf("Memory successfully resized to capacity: %d\n", student_capacity);
        }
    }
}

float calculate_gpa(float grades[]) {
    float sum = 0;
    for (int i = 0; i < MAX_GRADES; i++) {
        sum += grades[i];
    }
    return sum / MAX_GRADES;
}

int get_unique_id() {
    int id;
    int is_unique;
    do {
        is_unique = 1;
        printf("Enter Student ID: ");
        if (scanf("%d", &id) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }
        for (int i = 0; i < student_count; i++) {
            if (student_list[i].id == id) {
                printf("Error: ID %d already exists. Please enter a unique ID.\n", id);
                is_unique = 0;
                break;
            }
        }
    } while (!is_unique);
    return id;
}

// CRUD Operations 

void add_student() {
    check_and_resize();
    
    Student *new_student = &student_list[student_count];
    
    // Get ID 
    new_student->id = get_unique_id();

    // Get Name
    printf("Enter Name: ");
    while (getchar() != '\n'); 
    fgets(new_student->name, 50, stdin);
    new_student->name[strcspn(new_student->name, "\n")] = 0; 

    // Get Age
    do {
        printf("Enter Age (18-99): ");
        if (scanf("%d", &new_student->age) != 1 || new_student->age < 18 || new_student->age > 99) {
            printf("Invalid age entered.\n");
            while (getchar() != '\n');
        } else {
            break;
        }
    } while (1);

    // Get Course
    printf("Enter Course: ");
    while (getchar() != '\n');
    fgets(new_student->course, 50, stdin);
    new_student->course[strcspn(new_student->course, "\n")] = 0;

    // Get Grades
    printf("Enter %d Grades (0.0 - 4.0):\n", MAX_GRADES);
    for (int i = 0; i < MAX_GRADES; i++) {
        do {
            printf("  Grade %d: ", i + 1);
            if (scanf("%f", &new_student->grades[i]) != 1 || new_student->grades[i] < 0.0 || new_student->grades[i] > 4.0) {
                printf("Invalid grade. Must be between 0.0 and 4.0.\n");
                while (getchar() != '\n');
            } else {
                break;
            }
        } while (1);
    }

    // Calculate GPA
    new_student->gpa = calculate_gpa(new_student->grades);
    
    student_count++;
    printf("\nStudent %s added successfully (GPA: %.2f).\n", new_student->name, new_student->gpa);
}

void display_students() {
    if (student_count == 0) {
        printf("\n-- No records to display. --\n");
        return;
    }
    
    printf("\n--- Student Records (%d/%d) ---\n", student_count, student_capacity);
    printf("ID | Name            | Age | Course         | Grades (%.1f avg) | GPA\n", (float)MAX_GRADES);
    printf("------------------------------------------------------------------------\n");
    for (int i = 0; i < student_count; i++) {
        printf("%-3d| %-15s | %-3d | %-14s | ", 
            student_list[i].id, 
            student_list[i].name, 
            student_list[i].age, 
            student_list[i].course);
        
        for (int j = 0; j < MAX_GRADES; j++) {
            printf("%.1f ", student_list[i].grades[j]);
        }
        printf("| %.2f\n", student_list[i].gpa);
    }
    printf("------------------------------------------------------------------------\n");
}

void delete_student() {
    if (student_count == 0) {
        printf("No students to delete.\n");
        return;
    }

    int id_to_delete;
    printf("Enter ID of student to delete: ");
    if (scanf("%d", &id_to_delete) != 1) {
        printf("Invalid input.\n");
        while (getchar() != '\n');
        return;
    }

    for (int i = 0; i < student_count; i++) {
        if (student_list[i].id == id_to_delete) {
            if (i < student_count - 1) {
                memmove(&student_list[i], &student_list[i + 1], 
                        (student_count - 1 - i) * sizeof(Student));
            }
            student_count--;
            printf("Student with ID %d deleted.\n", id_to_delete);
            return;
        }
    }
    printf("Error: Student with ID %d not found.\n", id_to_delete);
}


void update_student() {
    if (student_count == 0) {
        printf("No student records available to update.\n");
        return;
    }

    int id_to_update;
    printf("\n--- Update Student Record ---\n");
    printf("Enter ID of student to update: ");
    if (scanf("%d", &id_to_update) != 1) {
        printf("Invalid input.\n");
        while (getchar() != '\n'); // Clear buffer
        return;
    }

    int index = -1;
    // Search for the student
    for (int i = 0; i < student_count; i++) {
        if (student_list[i].id == id_to_update) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("Error: Student with ID %d not found.\n", id_to_update);
        return;
    }

    // Pointer to the specific student for easier access
    Student *s = &student_list[index];
    
    printf("Student Found: %s (Current GPA: %.2f)\n", s->name, s->gpa);
    printf("What would you like to update?\n");
    printf("1. Name\n");
    printf("2. Age\n");
    printf("3. Course\n");
    printf("4. Grades (Recalculates GPA)\n");
    printf("0. Cancel\n");
    printf("Enter choice: ");

    int choice;
    if (scanf("%d", &choice) != 1) {
        printf("Invalid choice.\n");
        while (getchar() != '\n');
        return;
    }

    while (getchar() != '\n'); 

    switch (choice) {
        case 1: // Update Name
            printf("Enter New Name: ");
            fgets(s->name, 50, stdin);
            s->name[strcspn(s->name, "\n")] = 0; 
            printf("Name updated successfully.\n");
            break;

        case 2: // Update Age
            printf("Enter New Age: ");
            int new_age;
            if (scanf("%d", &new_age) == 1 && new_age >= 18 && new_age <= 99) {
                s->age = new_age;
                printf("Age updated successfully.\n");
            } else {
                printf("Invalid age input. Update failed.\n");
            }
            break;

        case 3: // Update Course
            printf("Enter New Course: ");
            fgets(s->course, 50, stdin);
            s->course[strcspn(s->course, "\n")] = 0;
            printf("Course updated successfully.\n");
            break;

        case 4: // Update Grades
            printf("Enter %d New Grades (0.0 - 4.0):\n", MAX_GRADES);
            for (int i = 0; i < MAX_GRADES; i++) {
                do {
                    printf("  Grade %d: ", i + 1);
                    if (scanf("%f", &s->grades[i]) != 1 || s->grades[i] < 0.0 || s->grades[i] > 4.0) {
                        printf("Invalid grade. Try again.\n");
                        while (getchar() != '\n');
                    } else {
                        break;
                    }
                } while (1);
            }
            
	    // Recalculate GPA after changing grades
            s->gpa = calculate_gpa(s->grades);
            printf("Grades updated. New GPA is %.2f.\n", s->gpa);
            break;

        case 0:
            printf("Update cancelled.\n");
            break;

        default:
            printf("Invalid selection.\n");
    }
}

// Search and Sorting Algorithms
void search_records() {
    int choice;
    printf("Search by: 1. ID | 2. Name: ");
    if (scanf("%d", &choice) != 1) {
        printf("Invalid choice.\n");
        return;
    }
    
    if (choice == 1) {
        int id_search;
        printf("Enter ID to search: ");
        if (scanf("%d", &id_search) != 1) {
            printf("Invalid input.\n");
            return;
        }
        
        // Linear Search
        for (int i = 0; i < student_count; i++) {
            if (student_list[i].id == id_search) {
                printf("\n--- Found Student ---\n");
                printf("ID: %d, Name: %s, GPA: %.2f\n", 
                       student_list[i].id, student_list[i].name, student_list[i].gpa);
                return;
            }
        }
        printf("Student with ID %d not found.\n", id_search);

    } else if (choice == 2) {
        char name_search[50];
        printf("Enter Name to search: ");
        while (getchar() != '\n');
        fgets(name_search, 50, stdin);
        name_search[strcspn(name_search, "\n")] = 0;

        for (int i = 0; i < student_count; i++) {
            if (strcmp(student_list[i].name, name_search) == 0) {
                printf("\n--- Found Student ---\n");
                printf("ID: %d, Name: %s, GPA: %.2f\n", 
                       student_list[i].id, student_list[i].name, student_list[i].gpa);
                return;
            }
        }
        printf("Student with name '%s' not found.\n", name_search);
    }
}

// Bubble Sort Implementation
void sort_records() {
    if (student_count < 2) {
        printf("Need at least 2 students to sort.\n");
        return;
    }
    
    int choice;
    printf("Sort by: 1. GPA | 2. ID | 3. Name: ");
    if (scanf("%d", &choice) != 1) {
        printf("Invalid input.\n");
        while(getchar() != '\n');
        return;
    }
    
    // Bubble Sort Algorithm
    for (int i = 0; i < student_count - 1; i++) {
        for (int j = 0; j < student_count - 1 - i; j++) {
            int swap = 0;
            
            if (choice == 1 && student_list[j].gpa < student_list[j+1].gpa) { // Sort by GPA (Descending)
                swap = 1;
            } else if (choice == 2 && student_list[j].id > student_list[j+1].id) { // Sort by ID (Ascending)
                swap = 1;
            } else if (choice == 3 && strcmp(student_list[j].name, student_list[j+1].name) > 0) { // Sort by Name (Ascending)
                swap = 1;
            }

            if (swap) {
                Student temp = student_list[j];
                student_list[j] = student_list[j+1];
                student_list[j+1] = temp;
            }
        }
    }

    printf("Records sorted by ");
    if (choice == 1) printf("GPA (highest first).\n");
    else if (choice == 2) printf("ID (ascending).\n");
    else if (choice == 3) printf("Name (A-Z).\n");
}

// Statistical and Analytical Features
void generate_reports() {
    if (student_count == 0) {
        printf("No student data available for reports.\n");
        return;
    }

    // --- Part 1: Class-wide Statistics ---
    float total_gpa = 0;
    float max_gpa = -1.0;
    float min_gpa = 5.0;
    int top_student_index = -1;

    for (int i = 0; i < student_count; i++) {
        total_gpa += student_list[i].gpa;
        
        // Check for Global Max
        if (student_list[i].gpa > max_gpa) {
            max_gpa = student_list[i].gpa;
            top_student_index = i;
        }
        // Check for Global Min
        if (student_list[i].gpa < min_gpa) {
            min_gpa = student_list[i].gpa;
        }
    }

    printf("-- Performance Report --\n");
    printf("Total Students:      %d\n", student_count);
    printf("Overall Average GPA: %.2f\n", total_gpa / student_count);
    if (top_student_index != -1) {
        printf("Best Student Overall: %s (GPA: %.2f)\n", 
               student_list[top_student_index].name, max_gpa);
    }
    printf("Lowest Class GPA:    %.2f\n", min_gpa);


    // Course Analysis 
    printf("\n-- Analysis by Course --\n");

    for (int i = 0; i < student_count; i++) {
        // 1. Duplicate Check:
        // Check if we have already processed this course name in a previous iteration
        int already_processed = 0;
        for (int prev = 0; prev < i; prev++) {
            if (strcmp(student_list[i].course, student_list[prev].course) == 0) {
                already_processed = 1;
                break;
            }
        }

        // If we have seen this course before, skip it to avoid duplicate reports
        if (already_processed) continue;

        char *current_course = student_list[i].course;
        
        float course_total_gpa = 0;
        int course_count = 0;
        float course_max = -1.0;
        char top_student_in_course[50];
        strcpy(top_student_in_course, "N/A");

        // Loop through the ENTIRE list again to find matches for this course
        for (int j = 0; j < student_count; j++) {
            if (strcmp(student_list[j].course, current_course) == 0) {
                course_total_gpa += student_list[j].gpa;
                course_count++;
                
                if (student_list[j].gpa > course_max) {
                    course_max = student_list[j].gpa;
                    strcpy(top_student_in_course, student_list[j].name);
                }
            }
        }

        printf("Course: %-15s\n", current_course);
        printf("   - Students Enrolled: %d\n", course_count);
        printf("   - Average GPA:       %.2f\n", course_total_gpa / course_count);
        printf("   - Top Performer:     %s (%.2f)\n", top_student_in_course, course_max);
    }
}

// File Handling

void save_records() {
    FILE *fp = fopen(FILENAME, "w"); 
    if (fp == NULL) {
        perror("Error opening file for saving");
        return;
    }

    // Write the total number of records first
    fwrite(&student_count, sizeof(int), 1, fp);

    // Write the entire array block
    if (student_count > 0) {
        fwrite(student_list, sizeof(Student), student_count, fp);
    }
    
    fclose(fp);
    printf("\nSuccessfully saved %d records to %s.\n", student_count, FILENAME);
}

void load_records() {
    FILE *fp = fopen(FILENAME, "r");
    if (fp == NULL) {
        perror("Error opening file for loading. Starting with empty list.");
        return;
    }

    // Read the total number of records
    int saved_count = 0;
    if (fread(&saved_count, sizeof(int), 1, fp) != 1) {
        printf("File is empty or corrupted.\n");
        fclose(fp);
        return;
    }

    if (saved_count > student_capacity) {
        student_capacity = saved_count;
        Student *temp = (Student *)realloc(student_list, student_capacity * sizeof(Student));
        if (temp == NULL) {
            perror("Error reallocating memory for loaded data");
            fclose(fp);
            return;
        }
        student_list = temp;
    }

    // Read the entire array block
    if (fread(student_list, sizeof(Student), saved_count, fp) != saved_count) {
        printf("Error reading all records from file.\n");
        fclose(fp);
        return;
    }
    
    student_count = saved_count;
    fclose(fp);
    printf("\nSuccessfully loaded %d records from %s.\n", student_count, FILENAME);
}

void cleanup_memory() {
    if (student_list != NULL) {
        free(student_list);
        student_list = NULL;
    }
}

// Main Function & Menu
void display_menu() {
    printf("\n\n--- Student Management System ---\n");
    printf("1. Add New Student Record\n");
    printf("2. Display All Records\n");
    printf("3. Search Record\n");
    printf("4. Update Record\n");
    printf("5. Delete Record\n");
    printf("6. Sort Records (by ID, Name, GPA)\n");
    printf("7. Generate Reports & Statistics\n");
    printf("8. Save Records to File\n");
    printf("9. Load Records from File\n");
    printf("0. Exit and Cleanup\n");
    printf("Enter choice: ");
}

int main() {
    initialize_list();
    load_records(); 
    
    int choice;
    do {
        display_menu();
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); 
            continue;
        }

        switch (choice) {
            case 1: add_student(); break;
            case 2: display_students(); break;
            case 3: search_records(); break;
            case 4: update_student(); break;
            case 5: delete_student(); break;
            case 6: sort_records(); display_students(); break;
            case 7: generate_reports(); break;
            case 8: save_records(); break;
            case 9: load_records(); break;
            case 0: break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 0);

    save_records(); 
    cleanup_memory(); 
    printf("\nSystem exited. Memory freed.\n");
    
    return 0;
}
