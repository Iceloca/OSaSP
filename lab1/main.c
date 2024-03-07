#define _DEFAULT_SOURCE
#define _GNU_SOURCE

#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

void dirwalk(char *directory, const char options) {
    struct dirent **file_names;
    int files_count;

    //Sorting acording to -s
    if (options & 8)
        files_count = scandir(directory, &file_names, NULL, alphasort);
    else
        files_count = scandir(directory, &file_names, NULL, versionsort);

    //Error check
    if (files_count == -1) {
        if (errno == ENOENT)
            perror("path doesn't exist\n");
        if (errno == ENOMEM)
            perror("Insuficient memmory to complete the operation\n");
        if (errno == ENOTDIR)
            perror("Path is not dirrectory\n");

        exit(errno);
    }

    //Displaying current dir files
    for (int i = 0; i < files_count; i++) {
        char *file_name = file_names[i]->d_name;
        const unsigned char file_type = file_names[i]->d_type;

        //Ignoring self and parent
        if (!(strcmp(file_name, ".") && strcmp(file_name, "..")))
            continue;

        if ((options == 0) || (options == 8) || ((options & 1) && (file_type == DT_LNK))
            || ((options & 2) && (file_type == DT_DIR)) || ((options & 4) && (file_type == DT_REG)))
            printf("%s/%s\n", directory, file_name);
    }
    //Checking sub-dirs
    for (int i = 0; i < files_count; i++) {
        const unsigned char file_type = file_names[i]->d_type;
        const char *file_name = file_names[i]->d_name;

        if (file_type == DT_DIR && (strcmp(file_name, ".") && strcmp(file_name, ".."))) {
            char newName[PATH_MAX];
            //Creating new name
            strcpy(newName, directory);
            strcat(newName, "/");
            strcat(newName, file_name);
            dirwalk(newName, options);
        }
    }
    free(file_names);
    return;
}

int main(const int argc, char **argv) {
    char options = 0;
    int arguments;
    char *directory;

    //Getting start dirrectory
    if (argv[argc - 1][0] != '-')
        directory = argv[argc - 1];
    if (argv[1][0] != '-')
        directory = argv[1];
    else {
        directory = ".";
    }

    //Getting options
    while ((arguments = getopt(argc, argv, "ldfs")) != -1) {
        switch (arguments) {
            //links
            case 'l':
                options = options | (char) 1;
                break;
            //directories
            case 'd':
                options = options | (char) 2;
                break;
            //files
            case 'f':
                options = options | (char) 4;
                break;
            //sorting according to LC_COLLATE
            case 's':
                options = options | (char) 8;
                break;
            default: break;
        }
    }


    dirwalk(directory, options);

    return 0;
}
