//
//  NewDirs.c
//  
//
//  Created by Ammar Abu Shamleh on 24/08/2016.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdbool.h>
#include <strings.h>

void compare(char **strings1, char **strings2, int len1, int len2);
void freeMem(char **strings, int len);

int main(int argc, char *argv[]) {
    //Check to ensure correct number of arguments
    if(argc != 3) {
        printf("Error. Two directory names required\n");
        exit(EXIT_FAILURE);
    }
    
    //Store list of directory names for dir 1 and dir 2
    char **dir1Entries = (char **) malloc(sizeof(char *));
    char **dir2Entries = (char **) malloc(sizeof(char *));
    
    //Attempt to open directory pointed to by argv[1]
    DIR *dp;
    struct dirent *ep;
    
    dp = opendir(argv[1]);
    //If directory can't be opened, print error message
    if(dp == NULL) {
        printf("Error. Directory %s couldn't be opened\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    
    //Read each entry in dir 1 into dir1Entries
    int nEntries = 0;
    while(   (ep = readdir(dp))    ) {
        dir1Entries = realloc(dir1Entries, (nEntries+1) * sizeof(char*));
        dir1Entries[nEntries++] = strdup(ep->d_name);
    }
    

    //Attempt to open directory pointed to by argv[2]
    dp = opendir(argv[2]);
    //If directory can't be opened, print error message
    if(dp == NULL) {
        printf("Error. Directory %s couldn't be opened\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    
    //Read each entry in dir 2 into dir2Entries
    int nEntries2 = 0;
    while(   (ep = readdir(dp))  ) {
        dir2Entries = realloc(dir2Entries, (nEntries2+1) * sizeof(char *));
        dir2Entries[nEntries2++] = strdup(ep->d_name);
    }
    
    //Print list of all directory names found in dir 1 and not dir 2
    compare(dir1Entries, dir2Entries, nEntries, nEntries2);
    
    //Free allocated memory
    freeMem(dir1Entries, nEntries);
    freeMem(dir2Entries, nEntries2);
    
    //Exit successfully
    exit(EXIT_SUCCESS);
}

//Print all string names found in strings1 and not strings2
void compare(char **strings1, char **strings2, int len1, int len2) {
    //Loop through all entries in dir1, and check if they exist in dir2
    for(int i=0; i<len1; i++) {
        bool curEntryExistsDir2 = false;
        for(int j=0; j<len2; j++) {
            //Check if they have the same directory name
            if(!strcmp(strings1[i],strings2[j])) {
                curEntryExistsDir2 = true;
            }
        }
        //If entry doesn't exist in dir2, print it's directory name
        if(!curEntryExistsDir2) {
            printf("%s\n", strings1[i]);
        }
    }
}

//Free allocated memory for string pointer
void freeMem(char **strings, int len) {
    for(int i=0; i<len; i++) {
        free(strings[i]);
    }
    free(strings);
}
