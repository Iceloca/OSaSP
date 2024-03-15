#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "my_environment.h"

#define FILE_PATH "../environment.txt"
#define MAX_LEN 256

extern char **__environ;

int compare(const void* a, const void* b) {
    return strcoll((const char *) a,(const char *) b);
}

char ** get_variable_names(void) {
    FILE* file = fopen(FILE_PATH, "r");
    char ** environment_variable_names = (char**)malloc(sizeof(char*) *1);
    if(environment_variable_names == NULL) {
        printf("Not enough memmory for envp");
        exit(1);
    }
    char * environment_variable_name = (char *)malloc(MAX_LEN * sizeof(char));
    int count = 0;
    if(file == NULL) {
        printf("File open error \n");
        exit(errno);
    }
    while((fscanf(file,"%[^\n] ",environment_variable_name)) != EOF) {
        count++;
        environment_variable_names = (char**) realloc(environment_variable_names,sizeof(char*) * (count + 1));
        if(environment_variable_names == NULL) {
            printf("Not enough memmory for envp");
            exit(1);
        }
        environment_variable_names[count-1] = environment_variable_name;
        environment_variable_name = (char *)malloc(MAX_LEN * sizeof(char));
    }
    environment_variable_names[count] = NULL;
    free(environment_variable_name);
    return environment_variable_names;
}

int main(int argc, char* argv[], char* envp[]) {
    if(argc < 2) {
        printf("Not enough argumetns \n");
        exit(1);
    }
    char** variable_names = get_variable_names();
    printf("MY NAME :%s \n", argv[0]);
    int count = 0;
    char** environment = (char**)malloc(sizeof(char*));
    printf("PID PROGRAM: %d\n", getpid());
    printf("PPID PROGRAM: %d\n", getppid());
    for(;*variable_names != NULL;variable_names ++) {
        count++;
        environment[count -1] = (char*)malloc(sizeof(char) * MAX_LEN);
        printf("%s", (*variable_names));
        environment = (char**)realloc(environment,sizeof(char*) *( count + 1));
        if(argv[1][0] =='+') {
            strcpy(environment[count - 1], *variable_names);
            strcat(environment[count -1 ], "=");
            strcat(environment[count - 1], variable_from_getenv(*variable_names));
        }
        if(argv[1][0] =='*') {
            strcpy(environment[count - 1], parth_variable_in_environment(envp,*variable_names));
        }
        if(argv[1][0] =='&') {
            strcpy(environment[count - 1], parth_variable_in_environment(__environ,*variable_names));
        }
    }
    qsort(environment,count,sizeof(char*),compare);
    for(int i = 0; i < count; i++ ) {
        printf("%s \n", environment[i]);
        free(environment[i]);
        free(variable_names[i]);
    }
    free(variable_names);
    free(environment);
    return 0;
}
