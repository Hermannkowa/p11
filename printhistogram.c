#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct Node {
    struct Node* previous_ptr;
    struct Node* next_ptr;
    char label[25];
    int count;
} Node;

Node* createNode(const char* label) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Fehler: Speicher konnte nicht allokiert werden.\n");
        exit(1);
    }

    newNode->previous_ptr = NULL;
    newNode->next_ptr = NULL;
    strcpy(newNode->label, label);
    newNode->count = 1;

    return newNode;
}

Node* findNode(Node* head, const char* label) {
    Node* currentNode = head;
    while (currentNode != NULL) {
        if (strcmp(currentNode->label, label) == 0) {
            return currentNode;
        }
        currentNode = currentNode->next_ptr;
    }
    return NULL;
}

void insertNode(Node** head, Node* newNode) {
    if (*head == NULL) {
        *head = newNode;
    } else {
        Node* currentNode = *head;
        while (currentNode->next_ptr != NULL) {
            currentNode = currentNode->next_ptr;
        }
        currentNode->next_ptr = newNode;
        newNode->previous_ptr = currentNode;
    }
}

void incrementCount(Node* node) {
    if (node != NULL) {
        node->count++;
    }
}

void freeList(Node* head) {
    Node* currentNode = head;
    while (currentNode != NULL) {
        Node* nextNode = currentNode->next_ptr;
        free(currentNode);
        currentNode = nextNode;
    }
}

void sortListByCount(Node** head) {
    if (*head == NULL || (*head)->next_ptr == NULL) {
        return;
    }

    Node* sorted = NULL;
    Node* current = *head;

    while (current != NULL) {
        Node* next = current->next_ptr;
        current->previous_ptr = current->next_ptr = NULL;

        if (sorted == NULL || current->count >= sorted->count) {
            current->next_ptr = sorted;
            if (sorted != NULL) {
                sorted->previous_ptr = current;
            }
            sorted = current;
        } else {
            Node* temp = sorted;
            while (temp->next_ptr != NULL && temp->next_ptr->count > current->count) {
                temp = temp->next_ptr;
            }
            current->next_ptr = temp->next_ptr;
            if (temp->next_ptr != NULL) {
                temp->next_ptr->previous_ptr = current;
            }
            temp->next_ptr = current;
            current->previous_ptr = temp;
        }

        current = next;
    }

    *head = sorted;
}
static inline int widestrlen(const char *str)
{
    return (int)mbstowcs(NULL, str, strlen(str));
}

static inline int compensation(const char *str)
{
    return strlen(str) - widestrlen(str);
}
void printHistogram(Node* head) {
    Node* currentNode = head;

    // Find the maximum length of the labels for formatting
    int maxLength = 0;
    while (currentNode != NULL) {
        int len = strlen(currentNode->label);
        if (len > maxLength) {
            maxLength = len;
        }
        currentNode = currentNode->next_ptr;
    }

    // Print the histogram with proper formatting
    currentNode = head;
    while (currentNode != NULL) {
        int offset = compensation(currentNode->label) > 0 ? maxLength - widestrlen(currentNode->label) : maxLength;
        printf("%-*s: %d\n", offset, currentNode->label, currentNode->count);
        currentNode = currentNode->next_ptr;
    }
}

int main(int argc, char* argv[]) {
    char* filename = NULL;
    int opt;
    while ((opt = getopt(argc, argv, "f:")) != -1) {
        switch (opt) {
            case 'f':
                filename = optarg;
                break;
            default:
                printf("Ungültige Option.\n");
                return 1;
        }
    }

    if (filename == NULL) {
        printf("Bitte geben Sie einen Dateinamen mit der -f Option an.\n");
        return 1;
    }

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Die Datei konnte nicht geöffnet werden.\n");
        return 1;
    }

    Node* head = NULL;
    char line[25];

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        Node* existingNode = findNode(head, line);
        if (existingNode != NULL) {
            incrementCount(existingNode);
        } else {
            Node* newNode = createNode(line);
            insertNode(&head, newNode);
        }
    }

    fclose(file);

    sortListByCount(&head);
    printHistogram(head);
    freeList(head);

    return 0;
}
