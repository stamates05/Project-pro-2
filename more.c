#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_LEN 256
#define INITIAL_CAPACITY 10

typedef struct {
    char **words;
    int size;
    int capacity;
} WordList;

char *trim(char *str);
void initWordList(WordList *list);
void resizeWordList(WordList *list);
void freeWordList(WordList *list);
int strcasecmp(const char *s1, const char *s2);
char *strcasestr(const char *haystack, const char *needle);
int isAlphanumeric(const char *str);
void insert(WordList *list, const char *word);
void findfwd(WordList *list, const char *pattern, int n);
void findrev(WordList *list, const char *pattern, int n);
void showrev(WordList *list, int n);
void load(WordList *list, const char *filename);
void save(WordList *list, const char *filename);
void printGuidance();
int compareWords(const void *a, const void *b);

char *trim(char *str)
{
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = 0;
    return str;
}

void initWordList(WordList *list)
{
    list->capacity = INITIAL_CAPACITY;
    list->size = 0;
    list->words = (char **)malloc(list->capacity * sizeof(char *));
    if (!list->words)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
}

void resizeWordList(WordList *list)
{
    if (list->size >= list->capacity)
    {
        list->capacity *= 2;
        list->words = (char **)realloc(list->words, list->capacity * sizeof(char *));
        if (!list->words)
        {
            fprintf(stderr, "Memory reallocation failed\n");
            exit(1);
        }
    }
}

void freeWordList(WordList *list)
{
    for (int i = 0; i < list->size; i++)
    {
        free(list->words[i]);
    }
    free(list->words);
    list->size = 0;
    list->capacity = 0;
}

int strcasecmp(const char *s1, const char *s2)
{
    while (*s1 && *s2)
    {
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2))
        {
            return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
        }
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

char *strcasestr(const char *haystack, const char *needle)
{
    if (!*needle) return (char *)haystack;
    size_t nlen = strlen(needle);
    for (size_t i = 0; haystack[i]; i++)
    {
        size_t j = 0;
        while (j < nlen && haystack[i + j] &&
               tolower((unsigned char)haystack[i + j]) == tolower((unsigned char)needle[j]))
        {
            j++;
        }
        if (j == nlen) return (char *)(haystack + i);
    }
    return NULL;
}

int isAlphanumeric(const char *str)
{
    for (int i = 0; str[i]; i++)
    {
        if (!isalnum((unsigned char)str[i]))
        {
            return 0;
        }
    }
    return 1;
}

void insert(WordList *list, const char *word)
{
    char *trimmed = trim((char *)word);
    if (strlen(trimmed) == 0)
    {
        printf("Error: Cannot insert empty word\n");
        return;
    }
    if (isAlphanumeric(trimmed))
    {
        printf("Error: Cannot insert purely alphanumeric word: %s\n", trimmed);
        return;
    }
    resizeWordList(list);
    list->words[list->size] = strdup(trimmed);
    if (!list->words[list->size])
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    list->size++;
    printf("Inserted: %s\n", trimmed);
}

void findfwd(WordList *list, const char *pattern, int n)
{
    if (n <= 0)
    {
        printf("Error: Invalid occurrence number %d\n", n);
        return;
    }
    int count = 0;
    for (int i = 0; i < list->size; i++)
    {
        if (strcasestr(list->words[i], pattern))
        {
            count++;
            if (count == n)
            {
                printf("Found '%s' at index %d: %s\n", pattern, i, list->words[i]);
                return;
            }
        }
    }
    printf("No %dth occurrence of '%s' found.\n", n, pattern);
}

void findrev(WordList *list, const char *pattern, int n)
{
    if (n <= 0)
    {
        printf("Error: Invalid occurrence number %d\n", n);
        return;
    }
    int count = 0;
    for (int i = list->size - 1; i >= 0; i--)
    {
        if (strcasestr(list->words[i], pattern))
        {
            count++;
            if (count == n)
            {
                printf("Found '%s' at index %d: %s\n", pattern, i, list->words[i]);
                return;
            }
        }
    }
    printf("No %dth occurrence of '%s' found.\n", n, pattern);
}

int compareWords(const void *a, const void *b)
{
    return strcasecmp(*(char **)b, *(char **)a);
}

void showrev(WordList *list, int n)
{
    if (n <= 0)
    {
        printf("Error: Invalid number of words %d\n", n);
        return;
    }
    n = (n > list->size) ? list->size : n;
    if (n == 0)
    {
        printf("No words to display.\n");
        return;
    }
    char **temp = (char **)malloc(n * sizeof(char *));
    if (!temp)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    for (int i = 0; i < n; i++)
    {
        temp[i] = list->words[list->size - n + i];
    }
    qsort(temp, n, sizeof(char *), compareWords);
    printf("Last %d words in reverse alphabetical order:\n", n);
    for (int i = 0; i < n; i++)
    {
        printf("%-4d %s\n", i + 1, temp[i]);
    }
    free(temp);
}

void load(WordList *list, const char *filename)
{
    char *trimmed = trim((char *)filename);
    if (strlen(trimmed) == 0)
    {
        printf("Error: Invalid filename\n");
        return;
    }
    FILE *file = fopen(trimmed, "r");
    if (!file)
    {
        printf("Cannot open file '%s'.\n", trimmed);
        return;
    }
    char buffer[MAX_WORD_LEN];
    while (fgets(buffer, MAX_WORD_LEN, file))
    {
        buffer[strcspn(buffer, "\n")] = 0;
        if (strlen(trim(buffer)) > 0)
        {
            insert(list, buffer);
        }
    }
    fclose(file);
    printf("Loaded words from '%s'.\n", trimmed);
}

void save(WordList *list, const char *filename)
{
    char *trimmed = trim((char *)filename);
    if (strlen(trimmed) == 0)
    {
        printf("Error: Invalid filename\n");
        return;
    }
    FILE *file = fopen(trimmed, "w");
    if (!file)
    {
        printf("Cannot open file '%s'.\n", trimmed);
        return;
    }
    for (int i = 0; i < list->size; i++)
    {
        fprintf(file, "%s\n", list->words[i]);
    }
    fclose(file);
    printf("Saved words to '%s'.\n", trimmed);
}

void printGuidance()
{
    printf("\nAvailable commands:\n");
    printf("  insert <word/phrase>         : Insert a word or phrase into the list\n");
    printf("  findfwd <pattern> <n>        : Find the nth occurrence of pattern (forward)\n");
    printf("  findrev <pattern> <n>        : Find the nth occurrence of pattern (reverse)\n");
    printf("  showrev <n>                  : Show last n words in reverse alphabetical order\n");
    printf("  load <filename>              : Load words from a file\n");
    printf("  save <filename>              : Save word list to a file\n");
    printf("  exit                         : Quit the program\n");
}

int main()
{
    WordList list;
    initWordList(&list);
    char line[1024];
    while (1)
    {
        printGuidance();
        printf("Enter commands (type 'exit' to quit):\n");
        if (!fgets(line, sizeof(line), stdin))
        {
            break;
        }
        line[strcspn(line, "\n")] = 0;
        char *trimmed_line = trim(line);
        if (strcmp(trimmed_line, "exit") == 0)
        {
            break;
        }
        if (strlen(trimmed_line) == 0)
        {
            printf("Error: Empty command\n");
            continue;
        }
        char command[20], arg1[256];
        int n;
        if (sscanf(trimmed_line, "%s %s %d", command, arg1, &n) == 3)
        {
            char *trimmed_arg = trim(arg1);
            if (strlen(trimmed_arg) == 0)
            {
                printf("Error: Invalid pattern\n");
                continue;
            }
            if (strcmp(command, "findfwd") == 0)
            {
                findfwd(&list, trimmed_arg, n);
            }
            else if (strcmp(command, "findrev") == 0)
            {
                findrev(&list, trimmed_arg, n);
            }
            else
            {
                printf("Invalid command: %s\n", trimmed_line);
            }
        }
        else if (sscanf(trimmed_line, "%s %d", command, &n) == 2 && strcmp(command, "showrev") == 0)
        {
            showrev(&list, n);
        }
        else if (sscanf(trimmed_line, "%s %[^\n]", command, arg1) == 2)
        {
            char *trimmed_arg = trim(arg1);
            if (strlen(trimmed_arg) == 0)
            {
                printf("Error: Invalid argument\n");
                continue;
            }
            if (strcmp(command, "insert") == 0)
            {
                insert(&list, trimmed_arg);
            }
            else if (strcmp(command, "load") == 0)
            {
                load(&list, trimmed_arg);
            }
            else if (strcmp(command, "save") == 0)
            {
                save(&list, trimmed_arg);
            }
            else
            {
                printf("Invalid command: %s\n", trimmed_line);
            }
        }
        else
        {
            printf("Invalid command: %s\n", trimmed_line);
        }
    }
    freeWordList(&list);
    return 0;
}