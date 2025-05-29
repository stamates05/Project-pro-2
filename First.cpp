#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_LEN 256
#define INITIAL_CAPACITY 10

// Structure to hold the word list
typedef struct {
    char **words;
    int size;
    int capacity;
} WordList;

// Initialize the word list
void initWordList(WordList *list) {
    list->capacity = INITIAL_CAPACITY;
    list->size = 0;
    list->words = (char **)malloc(list->capacity * sizeof(char *));
    if (!list->words) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
}

// Resize the word list if needed
void resizeWordList(WordList *list) {
    if (list->size >= list->capacity) {
        list->capacity *= 2;
        list->words = (char **)realloc(list->words, list->capacity * sizeof(char *));
        if (!list->words) {
            fprintf(stderr, "Memory reallocation failed\n");
            exit(1);
        }
    }
}

// Free the word list
void freeWordList(WordList *list) {
    for (int i = 0; i < list->size; i++) {
        free(list->words[i]);
    }
    free(list->words);
    list->size = 0;
    list->capacity = 0;
}

// Case-insensitive string comparison
int strcasecmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (tolower(*s1) != tolower(*s2)) {
            return tolower(*s1) - tolower(*s2);
        }
        s1++;
        s2++;
    }
    return tolower(*s1) - tolower(*s2);
}

// Case-insensitive substring search
char *strcasestr(const char *haystack, const char *needle) {
    char *h = strdup(haystack);
    char *n = strdup(needle);
    for (char *p = h; *p; p++) *p = tolower(*p);
    for (char *p = n; *p; p++) *p = tolower(*p);
    char *result = strstr(h, n);
    if (result) {
        result = (char *)(haystack + (result - h));
    }
    free(h);
    free(n);
    return result;
}

// Insert a word into the list
void insert(WordList *list, const char *word) {
    resizeWordList(list);
    list->words[list->size] = strdup(word);
    if (!list->words[list->size]) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    list->size++;
    printf("Inserted: %s\n", word);
}

// Find the nth occurrence of a pattern (forward direction)
void findfwd(WordList *list, const char *pattern, int n) {
    int count = 0;
    for (int i = 0; i < list->size; i++) {
        if (strcasestr(list->words[i], pattern)) {
            count++;
            if (count == n) {
                printf("Found '%s' at index %d: %s\n", pattern, i, list->words[i]);
                return;
            }
        }
    }
    printf("No %dth occurrence of '%s' found.\n", n, pattern);
}

// Find the nth occurrence of a pattern (reverse direction)
void findrev(WordList *list, const char *pattern, int n) {
    int count = 0;
    for (int i = list->size - 1; i >= 0; i--) {
        if (strcasestr(list->words[i], pattern)) {
            count++;
            if (count == n) {
                printf("Found '%s' at index %d: %s\n", pattern, i, list->words[i]);
                return;
            }
        }
    }
    printf("No %dth occurrence of '%s' found.\n", n, pattern);
}

// Comparison function for qsort (reverse alphabetical order)
int compareWords(const void *a, const void *b) {
    return strcasecmp(*(char **)b, *(char **)a);
}

// Show the last n words in reverse alphabetical order
void showrev(WordList *list, int n) {
    n = (n > list->size) ? list->size : n;
    if (n == 0) {
        printf("No words to display.\n");
        return;
    }

    // Create a temporary array for the last n words
    char **temp = (char **)malloc(n * sizeof(char *));
    if (!temp) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    // Copy the last n words
    for (int i = 0; i < n; i++) {
        temp[i] = list->words[list->size - n + i];
    }

    // Sort in reverse alphabetical order
    qsort(temp, n, sizeof(char *), compareWords);

    // Print the sorted words
    printf("Last %d words in reverse alphabetical order:\n", n);
    for (int i = 0; i < n; i++) {
        printf("%d. %s\n", i + 1, temp[i]);
    }

    free(temp);
}

// Load words from a file
void load(WordList *list, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Cannot open file '%s'.\n", filename);
        return;
    }

    char buffer[MAX_WORD_LEN];
    while (fgets(buffer, MAX_WORD_LEN, file)) {
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline
        insert(list, buffer);
    }
    fclose(file);
    printf("Loaded words from '%s'.\n", filename);
}

// Save words to a file
void save(WordList *list, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Cannot open file '%s'.\n", filename);
        return;
    }

    for (int i = 0; i < list->size; i++) {
        fprintf(file, "%s\n", list->words[i]);
    }
    fclose(file);
    printf("Saved words to '%s'.\n", filename);
}

int main() {
    WordList list;
    initWordList(&list);
    char line[1024];

    printf("Enter commands (type 'exit' to quit):\n");
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; // Remove newline
        if (strcmp(line, "exit") == 0) {
            break;
        }

        char command[20], arg1[256], arg2[256];
        int n;
        if (sscanf(line, "%s %s %d", command, arg1, &n) == 3) {
            if (strcmp(command, "findfwd") == 0) {
                findfwd(&list, arg1, n);
            } else if (strcmp(command, "findrev") == 0) {
                findrev(&list, arg1, n);
            } else if (strcmp(command, "showrev") == 0) {
                sscanf(line, "%s %d", command, &n);
                showrev(&list, n);
            }
        } else if (sscanf(line, "%s %[^\n]", command, arg1) == 2) {
            if (strcmp(command, "insert") == 0) {
                insert(&list, arg1);
            } else if (strcmp(command, "load") == 0) {
                load(&list, arg1);
            } else if (strcmp(command, "save") == 0) {
                save(&list, arg1);
            }
        } else {
            printf("Invalid command: %s\n", line);
        }
    }

    freeWordList(&list);
    return 0;
}
