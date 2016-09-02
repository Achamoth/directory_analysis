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
#include <sys/syslimits.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdbool.h>
#include <strings.h>

void compareDirs();
void compare(char **strings1, char **strings2, int len1, int len2);
void freeMem(char **strings, int len);
void parse(char *request);
void findModified(char *dirName, int dayOfMonth, int month, int year);

const char promptMessage[] = "Type 'help' to view a list of commands. Type 'quit' to exit";
const char helpMessage[2][100] = {
    "compare: list all files/directories found in first directory but not second",
    "modified: list all files/directories found in directory that have been modified after specified date"
};
const int nCommands = 2;

int main(int argc, char *argv[]) {
    
    //Main loop of program
    while(true) {
    //Ask user for request
    printf("Type 'help' to view a list of commands. Type 'quit' to exit\n");
    printf("Please enter the desired command : ");
    
    //Read and parse user request
    char request[20];
    scanf("%s", request);
    parse(request);
    }
    
}

//Parses user request and calls appropriate function. Returns false if command is invalid
void parse(char *request) {
    //User asks to quit
    if(!strcmp(request, "quit")) {
        printf("Goodbye\n");
        exit(EXIT_SUCCESS);
    }
    
    //User asks to compare 2 directories
    else if(!strcmp(request, "compare")) {
        compareDirs();
        printf("\n\n");
    }
    
    //User asks to list files in specified directory modified after specified date
    else if(!strcmp(request, "modified")) {
        //Ask user for directory name and
        char dirName[PATH_MAX];
        int day;
        int month;
        int year;
        printf("Please enter the directory name : ");
        scanf("%s", dirName);
        printf("Please enter the date to be checked against (dd/mm/yyyy): ");
        scanf("%d/%d/%d", &day, &month, &year);
        findModified(dirName, day, month, year);
        printf("\n\n");
    }
    
    //User asks for help
    else if(!strcmp(request, "help")) {
        printf("\n\n");
        for(int i=0; i<nCommands; i++) {
            printf("%s\n", helpMessage[i]);
        }
        printf("\n\n");
    }
    
    //Invalid command
    else {
        printf("\nCommand not recognised. Please enter a valid command\n\n");
    }
}

//Called when user requests to view all sub-directory names modified after certain date
void findModified(char *dirName, int dayOfMonth, int month, int year) {
    //Attempt to open directory pointed to by dirName
    DIR *dp;
    struct dirent *ep;
    dp = opendir(dirName);
    
    //If directory can't be opened, print error message
    if(dp == NULL) {
        printf("Error. Directory %s couldn't be opened\n", dirName);
        exit(EXIT_FAILURE);
    }
    
    printf("\n\n");
    
    //Otherwise, read each entry in the directory
    while( (ep = readdir(dp)) ) {
        //Compute complete pathname for current entry
        char curPathname[PATH_MAX + 256];
        strcpy(curPathname, dirName);
        char *fileName = ep -> d_name;
        strcat(curPathname, fileName);
        
        //Find last modified time for file
        struct stat attr;
        stat(curPathname, &attr);
        time_t lastModifiedTime = attr.st_mtime;
        struct tm *lastModified = localtime(&lastModifiedTime);
        
        //Now compare last modified time to provided date
        if(lastModified->tm_year > year-1900) {
            //File modified after specified date; print filename
            printf("%s\n", fileName);
        }
        else if(  (lastModified->tm_year == year-1900) && (lastModified->tm_mon > month-1) ) {
            printf("%s\n", fileName);
        }
        else if(  (lastModified->tm_year == year-1900) && (lastModified->tm_mon == month-1) && (lastModified->tm_mday > dayOfMonth) ) {
            printf("%s\n", fileName);
        }
    }
}

//Called when user requests to compare 2 directories
void compareDirs() {
    char dname1[PATH_MAX];
    char dname2[PATH_MAX];
    
    //Ask user for directory names
    printf("Please enter the first direcory name : ");
    scanf("%s", dname1);
    printf("Please enter the second directory name : ");
    scanf("%s", dname2);
    
    //Store list of directory names for dir 1 and dir 2
    char **dir1Entries = (char **) malloc(sizeof(char *));
    char **dir2Entries = (char **) malloc(sizeof(char *));
    
    //Attempt to open directory pointed to by dname1
    DIR *dp;
    struct dirent *ep;
    
    dp = opendir(dname1);
    //If directory can't be opened, print error message
    if(dp == NULL) {
        printf("Error. Directory %s couldn't be opened\n", dname1);
        exit(EXIT_FAILURE);
    }
    
    //Read each entry in dir 1 into dir1Entries
    int nEntries = 0;
    while(   (ep = readdir(dp))    ) {
        dir1Entries = realloc(dir1Entries, (nEntries+1) * sizeof(char*));
        dir1Entries[nEntries++] = strdup(ep->d_name);
    }
    
    //Attempt to open directory pointed to by dname2
    dp = opendir(dname2);
    //If directory can't be opened, print error message
    if(dp == NULL) {
        printf("Error. Directory %s couldn't be opened\n", dname2);
        exit(EXIT_FAILURE);
    }
    
    //Read each entry in dir 2 into dir2Entries
    int nEntries2 = 0;
    while(   (ep = readdir(dp))  ) {
        dir2Entries = realloc(dir2Entries, (nEntries2+1) * sizeof(char *));
        dir2Entries[nEntries2++] = strdup(ep->d_name);
    }
    
    //Print list of all directory names found in dir 1 and not dir 2
    printf("\n\n");
    compare(dir1Entries, dir2Entries, nEntries, nEntries2);
    
    //Free allocated memory
    freeMem(dir1Entries, nEntries);
    freeMem(dir2Entries, nEntries2);
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
