/* $Header: /xGate/gencompile.c 5     10/05/01 3:42p Bennylp $
 * 
 * Generate compile information.
 */
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FILENAME	"compileinfo.h"
#define WRITE_INTERVAL	(5*60)

char *strtolower(char *s)
{
    char *start = s;
    while (*s) *s++ = tolower(*s);
    return start;
}

void error(const char *msg)
{
    fprintf(stderr, "Error: %s\n", msg);
    exit(1);
}

int main()
{
    char sdate[32], stime[32], host[128], username[64];
    DWORD size;
    FILE *file;
    time_t now;
    struct tm *tmptr;
    struct stat fstat;
    
    time(&now);
    
    /* Check the file date/time.
     * Only overwrite if the file is older than some interval (WRITE_INTERVAL). 
     */
    if (!_stat(FILENAME, &fstat)) {
	if (fstat.st_mtime <= now && now - fstat.st_mtime < WRITE_INTERVAL) {
	    /* File is not too old.
	       Leave it unmodified.
             */	       
	    printf("File '%s' is up to date\n", FILENAME);	
	    return 0;
	}
    }
    
    size = sizeof(host);
    if (!GetComputerName(host,&size)) {
	error("Unable to get hostname");
    }

    size = sizeof(username);
    if (!GetUserName(username,&size)) {
	error("Unable to get username");
    }
    
    tmptr = localtime(&now);
    strftime(sdate, sizeof(sdate), "%b %d, %Y", tmptr);
    strftime(stime, sizeof(stime), "%H:%M:%S", tmptr);
    
    
    file = fopen(FILENAME, "wt");
    if (!file)
	error("Unable to open output file compileinfo.h for writing");
    
    fprintf(file, "\n");
    fprintf(file, "/************************************************************\\\n"
		  " * This file is generated automatically by gencompile.exe    *\n"
		  " * and will be included in main.cpp                          *\n"
		  " *                                                           *\n"
		  " * DO NOT modify this file.                                  *\n"
		  " * DO NOT add this file to Source Safe                       *\n"
		  "\\************************************************************/\n");
    fprintf(file, "\n");
    fprintf(file, "#define COMPILE_DATE\t\"%s\"\n", sdate);
    fprintf(file, "#define COMPILE_TIME\t\"%s\"\n", stime);
    fprintf(file, "#define COMPILE_HOST\t\"%s\"\n", strtolower(host));
    fprintf(file, "#define COMPILE_USER\t\"%s\"\n", strtolower(username));
    fclose(file);
    printf("File '%s' was successfully updated\n", FILENAME);	
    return 0;
}
