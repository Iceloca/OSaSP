//
// Created by iceloca on 15.03.24.
//

#ifndef MY_ENVIRONMENT_H
#define MY_ENVIRONMENT_H
char * parth_variable_in_environment (char ** environment, char const* variable) {
    for (; *environment != NULL; ++environment) {
        if(strstr(*environment, variable) != NULL) {
            printf("%s \n", strstr(*environment, variable));
            return strstr(*environment, variable);
        }
    }
    printf("Can't find %s in environ", variable);
    exit(5);
}
char * variable_from_getenv(char* variable) {
    char* child_path = getenv(variable);
    if (child_path == NULL){
        printf("There is no CHILD_PATH variable\n");
        exit(3);
    }
    return child_path;
}
#endif //MY_ENVIRONMENT_H
