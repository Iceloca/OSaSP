#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "my_environment.h"
#define CHILD_PATH "MY_CHILD_PATH"
#define FILE_PATH "../environment.txt"
#define MAX_LEN 256

extern char **__environ;



char ** make_environment_from_file() {
    FILE* file = fopen(FILE_PATH, "r");
    char ** envp = (char**)malloc(sizeof(char*) *1);
    if(envp == NULL) {
        printf("Not enough memmory for envp");
        exit(1);
    }
    char** environment = __environ;
    char * environment_variable_name = (char *)malloc(MAX_LEN * sizeof(char));
    char * environment_variable_value = NULL;
    int count = 0;
    if(file == NULL) {
        printf("File open error \n");
        exit(errno);
    }
    while((fscanf(file,"%[^\n] ",environment_variable_name)) != EOF) {
        environment = __environ;
        strcat(environment_variable_name, "=");
        for (; *environment!= NULL; ++environment)
        {
            if(strstr(*environment, environment_variable_name) == (*environment)) {
                count++;
                environment_variable_value = (char *)malloc(MAX_LEN * sizeof(char));
                if(environment_variable_value == NULL) {
                    printf("Not enough memmory for environment variable value");
                    exit(1);
                }
                envp = (char**) realloc(envp,sizeof(char*) * (count + 1));
                if(envp == NULL) {
                    printf("Not enough memmory for envp");
                    exit(1);
                }
                strcpy(environment_variable_value,*environment);
                envp[count-1] = environment_variable_value;
            }
        }
    }
    envp[count] = NULL;
    return  envp;
}

char ** make_argument_for_child(char parent_symbol, int count_child) {
    char str[3];
    sprintf(str, "%d%d",count_child / 10, count_child % 10);
    char* name = malloc(sizeof(char) * (strlen("CHILD_") + 2 +1));
    strcpy(name,"CHILD_");
    strcat(name,str);
    char symbol[2];
    symbol[0] = parent_symbol;
    symbol[1] = '\0';
    char** argv = (char**)malloc(sizeof(char*)*3);
    argv[0] = name;
    argv[1] = symbol;
    argv[2] = NULL;
    return  argv;
}

void create_child(char * path, char parent_symbol, int child_count) {
    pid_t pid = fork();
    if (pid == -1) {
        printf("Error occured, error code - %d\n", errno);
        exit(errno);
    }
    if (pid == 0) {
        printf("Child process created. Please, wait...\n");
        execve(path,make_argument_for_child(parent_symbol, child_count),make_environment_from_file());
    }
}

void process_controller(char * envp[]) {
    char option = 0;
    char child_count = 1;
    char* child_path= NULL;
    while(option != 'q') {
        child_path= NULL;
        option = getchar();
        getchar();
        switch (option) {
            case '+':
                child_path = variable_from_getenv(CHILD_PATH);
                break;
            case '&':
                child_path = parth_variable_in_environment(envp, CHILD_PATH) + strlen(CHILD_PATH) + 1;
                printf("\n %s \n ",child_path);
                break;
            case '*':
                child_path = parth_variable_in_environment(__environ, CHILD_PATH) + strlen(CHILD_PATH) + 1;
                break;
            default:
                printf("Wrong line\n");
                break;
        }
        if( child_path != NULL)
            create_child(child_path, option, child_count++);
    }
}


int main(int argc, char* argv[], char* envp[]) {
    int child_count = 1;
    process_controller(envp);
    return 0;
}
