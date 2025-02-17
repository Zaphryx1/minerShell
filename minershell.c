#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

// Function to properly tokenize user input
char **tokenize(char *line) {
    char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
    int tokenNo = 0;
    char *token = strtok(line, " \t\n"); // Split by space, tab, or newline

    while (token != NULL) {
        tokens[tokenNo] = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
        strcpy(tokens[tokenNo], token);
        tokenNo++;
        token = strtok(NULL, " \t\n"); // Continue splitting
    }

    tokens[tokenNo] = NULL; // End of tokens
    return tokens;
}

int main(int argc, char *argv[]) {
    char line[MAX_INPUT_SIZE];
    char **tokens;
    int i;

    while (1) {            
        printf("minersh$ "); // Shell prompt
        if (!fgets(line, sizeof(line), stdin)) { 
            break;  // Handle EOF (Ctrl+D)
        }

        if (line[0] == '\n') { 
            continue; // Ignore empty input
        }

        line[strcspn(line, "\n")] = 0; // Remove trailing newline
        printf("Command entered: %s\n", line); // Debug message for user input
        tokens = tokenize(line);

        if (tokens[0] == NULL) {
            continue;
        }

        printf("Parsed tokens:\n"); // Debug message for tokenization
        for (i = 0; tokens[i] != NULL; i++) {
            printf("Token %d: %s\n", i, tokens[i]);
        }

        // Handle the "exit" command
        if (strcmp(tokens[0], "exit") == 0) {
            printf("Exiting minershell...\n");
            break;
        }

        // Handle the "cd" command properly
        if (strcmp(tokens[0], "cd") == 0) {
            if (tokens[1] == NULL) {
                printf("Shell: Incorrect command\n");
            } else if (chdir(tokens[1]) == 0) {
                printf("Directory changed to: %s\n", tokens[1]);
            } else {
                perror("cd failed");
            }
            continue;
        }

        // Fork a child process to execute commands
        pid_t pid = fork();

        if (pid == 0) {  // Child process
            printf("Executing command: %s\n", tokens[0]); // Debug message before execution
            if (execvp(tokens[0], tokens) == -1) {
                perror("Shell Error");
            }
            exit(EXIT_FAILURE);
        } else if (pid > 0) {  // Parent process
            waitpid(pid, NULL, 0);
            printf("Command execution completed.\n"); // Debug message after execution
        } else {
            perror("Fork failed");
        }

        // Free allocated memory
        for (i = 0; tokens[i] != NULL; i++) {
            free(tokens[i]);
        }
        free(tokens);
    }
    return 0;
}
