#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global Data and Constants
#define DATA_FILENAME "dataset.txt"

// Dynamic array pointer and size
int *dataset = NULL;
int data_size = 0;

// Function Pointer Definition
typedef void (*OperationFunc)(int*, int);


// Dynamic Memory Management Functions 
void cleanup_memory() {
    if (dataset != NULL) {
        free(dataset);
        dataset = NULL;
        data_size = 0;
        printf("\nDynamic memory freed.\n");
    }
}

void add_element() {
    int value;
    printf("Enter integer value to add: ");
    if (scanf("%d", &value) != 1) {
        printf("Invalid input.\n");
        while (getchar() != '\n');
        return;
    }
    
    
    data_size++;
    int *temp = (int*)realloc(dataset, data_size * sizeof(int));
    
    if (temp == NULL) {
        printf("Error: Failed to reallocate memory for new element.\n");
        data_size--; 
        return;
    }
    
    dataset = temp;
    dataset[data_size - 1] = value;
    printf("Element %d added successfully. New size: %d\n", value, data_size);
}


void delete_element() {
    if (data_size == 0) {
        printf("Dataset is empty.\n");
        return;
    }
    int index;
    printf("Enter index (0 to %d) of element to delete: ", data_size - 1);
    if (scanf("%d", &index) != 1 || index < 0 || index >= data_size) {
        printf("Invalid index.\n");
        while (getchar() != '\n');
        return;
    }

    memmove(&dataset[index], &dataset[index + 1], (data_size - index - 1) * sizeof(int));

    // Decrement size and shrink memory
    data_size--;
    int *temp = (int*)realloc(dataset, data_size * sizeof(int));

    if (temp == NULL && data_size > 0) {
        printf("Warning: Failed to shrink memory, but deletion successful.\n");
    } else {
        dataset = temp;
    }
    printf("Element at index %d deleted. New size: %d\n", index, data_size);
}

// Function Pointers
void compute_sum(int *data, int size) {
    if (size == 0) { printf("Dataset is empty.\n"); return; }
    long long sum = 0; // Use long long to prevent overflow
    for (int i = 0; i < size; i++) {
        sum += data[i];
    }
    printf("--- Result ---\nTotal Sum: %lld\n", sum);
}

void compute_average(int *data, int size) {
    if (size == 0) { printf("Dataset is empty.\n"); return; }
    long long sum = 0;
    for (int i = 0; i < size; i++) {
        sum += data[i];
    }
    printf("--- Result ---\nAverage: %.2f\n", (float)sum / size);
}

void find_min_max(int *data, int size) {
    if (size == 0) { printf("Dataset is empty.\n"); return; }
    int min = data[0];
    int max = data[0];
    for (int i = 1; i < size; i++) {
        if (data[i] < min) min = data[i];
        if (data[i] > max) max = data[i];
    }
    printf("--- Result ---\nMinimum Value: %d\nMaximum Value: %d\n", min, max);
}

// Bubble Sort implementation
void sort_dataset(int *data, int size) {
    if (size < 2) { printf("Need at least 2 elements to sort.\n"); return; }
    int swapped;
    for (int i = 0; i < size - 1; i++) {
        swapped = 0;
        for (int j = 0; j < size - 1 - i; j++) {
            if (data[j] > data[j+1]) {
                int temp = data[j];
                data[j] = data[j+1];
                data[j+1] = temp;
                swapped = 1;
            }
        }
        if (swapped == 0) break; 
    }
    printf("Dataset sorted in ascending order.\n");
}

void search_value(int *data, int size) {
    if (size == 0) { printf("Dataset is empty.\n"); return; }
    int target;
    int found_count = 0;
    printf("Enter value to search for: ");
    if (scanf("%d", &target) != 1) {
        printf("Invalid input.\n");
        while (getchar() != '\n');
        return;
    }
    
    printf("--- Search Results ---\n");
    for (int i = 0; i < size; i++) {
        if (data[i] == target) {
            printf("Found %d at index %d\n", target, i);
            found_count++;
        }
    }
    if (found_count == 0) {
        printf("Value %d not found in the dataset.\n", target);
    } else {
        printf("Total occurrences: %d\n", found_count);
    }
}

// File I/O Functions

void save_results() {
    if (data_size == 0) {
        printf("Dataset is empty. Nothing to save.\n");
        return;
    }
    FILE *fp = fopen(DATA_FILENAME, "w");
    if (fp == NULL) {
        perror("Error opening file for saving");
        return;
    }
    
    fprintf(fp, "DATA_SIZE=%d\n", data_size);
    for (int i = 0; i < data_size; i++) {
        fprintf(fp, "%d\n", dataset[i]);
    }
    
    fclose(fp);
    printf("Dataset saved to %s.\n", DATA_FILENAME);
}

void load_data() {
    FILE *fp = fopen(DATA_FILENAME, "r");
    if (fp == NULL) {
        printf("File %s not found. Starting with empty dataset.\n", DATA_FILENAME);
        return;
    }
    
    // Read size from header
    int saved_size = 0;
    char line[64];
    if (fgets(line, sizeof(line), fp) && sscanf(line, "DATA_SIZE=%d", &saved_size) == 1 && saved_size >= 0) {
        
        cleanup_memory(); 

        dataset = (int*)malloc(saved_size * sizeof(int));
        if (dataset == NULL) {
            perror("Error allocating memory for loaded data");
            fclose(fp);
            return;
        }

        int count = 0;
        int value;
        while (fgets(line, sizeof(line), fp) && count < saved_size) {
            if (sscanf(line, "%d", &value) == 1) {
                dataset[count++] = value;
            }
        }
        data_size = count;
        printf("Successfully loaded %d elements from %s.\n", data_size, DATA_FILENAME);
    } else {
        printf("File %s corrupted or empty.\n", DATA_FILENAME);
    }
    
    fclose(fp);
}

// Menu & Main Function

void view_dataset() {
    if (data_size == 0) {
        printf("Dataset is empty.\n");
        return;
    }
    printf("\n-- Current Dataset (Size: %d) --\n", data_size);
    for (int i = 0; i < data_size; i++) {
        printf("[%d]: %d\n", i, dataset[i]);
    }
    printf("----------------------------------\n");
}

void display_menu() {
    printf("\n-- Dynamic Math Engine Menu --\n");
    printf("-- Data Management --\n");
    printf("  1. Add Element\n");
    printf("  2. Delete Element (by index)\n");
    printf("  3. View Dataset\n");
    printf("-- Dynamic Operations --\n");
    printf("  4. Compute Sum\n");
    printf("  5. Compute Average\n");
    printf("  6. Find Min/Max\n");
    printf("  7. Sort Dataset (Ascending)\n");
    printf("  8. Search Value\n");
    printf("-- Persistence --\n");
    printf("  9. Save Results to File\n");
    printf(" 10. Load Data from File\n");
    printf("  0. Exit\n");
    printf("Enter choice: ");
}

// Array of function pointers for dynamic dispatch
OperationFunc math_operations[] = {
    compute_sum,       
    compute_average,   
    find_min_max,      
    sort_dataset,   
    search_value       
};

int main() {
    load_data(); 

    int choice;
    do {
        display_menu();
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }
        
        // Dispatch logic
        if (choice >= 4 && choice <= 8) {
            int operation_index = choice - 4; 
            math_operations[operation_index](dataset, data_size);
        } else {
            switch (choice) {
                case 1: add_element(); break;
                case 2: delete_element(); break;
                case 3: view_dataset(); break;
                case 9: save_results(); break;
                case 10: load_data(); break;
                case 0: break;
                default: printf("Invalid choice. Try again.\n");
            }
        }
    } while (choice != 0);

    cleanup_memory();
    return 0;
}
