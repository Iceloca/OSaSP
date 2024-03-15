#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#define CHILD_PATH "CHILD_PATH"
#define FILE_PATH "../environment.txt"
#define MAX_LEN 256

extern char **__environ;

char * child_from_getenv(void) {
    char* child_path = getenv("CHILD_PATH");
    if (child_path == NULL){
        printf("There is no CHILD_PATH variable\n");
        exit(3);
    }
    return child_path;
}

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
        for (; *environment!= NULL; ++environment)
        {
            if(strstr(*environment, environment_variable_name) != NULL) {
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
                environment_variable_value = strcpy(environment_variable_value,*environment);
                envp[count-1] = environment_variable_value;
            }
        }
    }
    envp[count] = NULL;
    return  envp;
}

char ** make_argument_for_child(void) {
    static int count_child = 1;
    char name[3];
    sprintf(name, "%d", count_child);
    char** argv = (char**)malloc(sizeof(char*)*1);
    argv[0] = name;
    count_child ++;
    return  argv;
}

void create_child(char * path, char* argv[], char* envp[]) {
    pid_t pid = fork();
    if (pid == -1) {
        printf("Error occured, error code - %d\n", errno);
        exit(errno);
    }
    if (pid == 0) {
        printf("Child process created. Please, wait...\n");
        execve(path,argv,envp);
    }
}

char * parth_variable_in_environment (char ** environment, char const* variable) {
    for (; *environment != NULL; ++environment)
    {
        if(strstr(*environment, variable) != NULL) {
            return strstr(*environment, variable);
        }
    }
    return NULL;
}
char * child_from_envp(char* envp[]) {
    char* child_path = parth_variable_in_environment(envp, CHILD_PATH);
    if(child_path == NULL)    {
        printf("Can't find %s in envp", CHILD_PATH);
        exit(4);
    }
    return child_path;
}
char * child_from_environ(void) {
    char* child_path = parth_variable_in_environment(__environ, CHILD_PATH);
    if(child_path == NULL)    {
        printf("Can't find %s in environ", CHILD_PATH);
        exit(5);
    }
    return child_path;
}

void process_controller(char * envp[]) {
    char option = 0;
    char* child_path= NULL;
    char** child_envp = make_environment_from_file(FILE_PATH);
    char** child_argv = make_argument_for_child();
    while(option != 'q') {
        child_path= NULL;
        option = getchar();
        getchar();
        switch (option) {
            case '+':
            child_path = child_from_getenv();
            break;
            case '&':
            child_path = child_from_envp(envp);
            break;
            case '*':
            child_path = child_from_environ();
            break;
            default:
                printf("Waiting \n");
            break;
        }
        if( child_path != NULL)
            create_child(child_path,child_argv,child_envp);
    }
    return;
}


int main(int argc, char* argv[], char* envp[]) {
    process_controller(envp);
    return 0;
}
