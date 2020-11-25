
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h> // for opendir(), readdir(), closedir()
#include <sys/stat.h> // for stat()
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdarg>

#define PROC_DIRECTORY "/proc/"
#define CASE_SENSITIVE    1
#define CASE_INSENSITIVE  0
#define EXACT_MATCH       1
#define INEXACT_MATCH     0


int IsNumeric(const char* ccharptr_CharacterList)
{
    for ( ; *ccharptr_CharacterList; ccharptr_CharacterList++)
        if (*ccharptr_CharacterList < '0' || *ccharptr_CharacterList > '9')
            return 0; // false
    return 1; // true
}


int strcmp_Wrapper(const char *s1, const char *s2, int intCaseSensitive)
{
    if (intCaseSensitive)
        return !strcmp(s1, s2);
    else
        return !strcasecmp(s1, s2);
}

// int strstr_Wrapper(const char* haystack, const char* needle, int intCaseSensitive)
// {
//     if (intCaseSensitive)
//         return (int) strstr(haystack, needle);
//     else
//         return (int) strcasestr(haystack, needle);
// }


// pid_t GetPIDbyName(const char* cchrptr_ProcessName, int intCaseSensitiveness, int intExactMatch)
// {
//     char chrarry_CommandLinePath[100]  ;
//     char chrarry_NameOfProcess[300]  ;
//     char* chrptr_StringToCompare = NULL ;
//     pid_t pid_ProcessIdentifier = (pid_t) -1 ;
//     struct dirent* de_DirEntity = NULL ;
//     DIR* dir_proc = NULL ;

//     int (*CompareFunction) (const char*, const char*, int) ;

//     if (intExactMatch)
//         CompareFunction = &strcmp_Wrapper;
//     //else
//         //CompareFunction = &strstr_Wrapper;


//     dir_proc = opendir(PROC_DIRECTORY) ;
//     if (dir_proc == NULL)
//     {
//         perror("Couldn't open the " PROC_DIRECTORY " directory") ;
//         return (pid_t) -2 ;
//     }

//     // Loop while not NULL
//     while ( (de_DirEntity = readdir(dir_proc)) )
//     {
//         if (de_DirEntity->d_type == DT_DIR)
//         {
//             if (IsNumeric(de_DirEntity->d_name))
//             {
//                 strcpy(chrarry_CommandLinePath, PROC_DIRECTORY) ;
//                 strcat(chrarry_CommandLinePath, de_DirEntity->d_name) ;
//                 strcat(chrarry_CommandLinePath, "/cmdline") ;
//                 FILE* fd_CmdLineFile = fopen (chrarry_CommandLinePath, "rt") ;  // open the file for reading text
//                 if (fd_CmdLineFile)
//                 {
//                     fscanf(fd_CmdLineFile, "%s", chrarry_NameOfProcess) ; // read from /proc/<NR>/cmdline
//                     fclose(fd_CmdLineFile);  // close the file prior to exiting the routine

//                     if (strrchr(chrarry_NameOfProcess, '/'))
//                         chrptr_StringToCompare = strrchr(chrarry_NameOfProcess, '/') +1 ;
//                     else
//                         chrptr_StringToCompare = chrarry_NameOfProcess ;

//                     printf("Process name: %s\n", chrarry_NameOfProcess);
//                     //printf("Pure Process name: %s\n", chrptr_StringToCompare );

//                     if ( CompareFunction(chrptr_StringToCompare, cchrptr_ProcessName, intCaseSensitiveness) )
//                     {
//                         pid_ProcessIdentifier = (pid_t) atoi(de_DirEntity->d_name) ;
//                         closedir(dir_proc) ;
//                         return pid_ProcessIdentifier ;
//                     }
//                 }
//             }
//         }
//     }
//     closedir(dir_proc) ;
//     return pid_ProcessIdentifier ;
// }

// pid_t GetPIDbyName(const char* cchrptr_ProcessName)
// {
//     return GetPIDbyName(cchrptr_ProcessName, CASE_INSENSITIVE, EXACT_MATCH) ;
// }

/*************************************************************************/

pid_t bb1(const char* cchrptr_ProcessName)
{
    char chrarry_CommandLinePath[100]  ;
    char chrarry_NameOfProcess[300]  ;
    char* chrptr_StringToCompare = NULL ;
    pid_t pid_ProcessIdentifier = (pid_t) -1 ;
    pid_t PID;
    struct dirent* de_DirEntity = NULL ;
    DIR* dir_proc = NULL ;

    int (*CompareFunction) (const char*, const char*, int) ;

    
    CompareFunction = &strcmp_Wrapper;  /* Exact match */
    // CompareFunction = &strstr_Wrapper;  /* Inexact match */


    dir_proc = opendir(PROC_DIRECTORY) ;
    if (dir_proc == NULL)
    {
        perror("Couldn't open the " PROC_DIRECTORY " directory") ;
        return (pid_t) -2 ;
    }

    // Loop while not NULL
    while ( (de_DirEntity = readdir(dir_proc)) )
    {
        if (de_DirEntity->d_type == DT_DIR)
        {
            if (IsNumeric(de_DirEntity->d_name))
            {
                strcpy(chrarry_CommandLinePath, PROC_DIRECTORY) ;
                strcat(chrarry_CommandLinePath, de_DirEntity->d_name) ;
                strcat(chrarry_CommandLinePath, "/cmdline") ;
                FILE* fd_CmdLineFile = fopen (chrarry_CommandLinePath, "rt") ;  // open the file for reading text
                if (fd_CmdLineFile)
                {
                    fscanf(fd_CmdLineFile, "%s", chrarry_NameOfProcess) ; // read from /proc/<NR>/cmdline
                    fclose(fd_CmdLineFile);  // close the file prior to exiting the routine

                    if (strrchr(chrarry_NameOfProcess, '/'))
                        chrptr_StringToCompare = strrchr(chrarry_NameOfProcess, '/') +1 ;
                    else
                        chrptr_StringToCompare = chrarry_NameOfProcess ;

                    PID = (pid_t) atoi(de_DirEntity->d_name) ;
                    printf("Process name: %s    PID:  %d\n", chrarry_NameOfProcess, PID);
                    //printf("Pure Process name: %s\n", chrptr_StringToCompare );

                    if ( CompareFunction(chrptr_StringToCompare, cchrptr_ProcessName, 0) )  /* case insensitive */
                    {
                        pid_ProcessIdentifier = (pid_t) atoi(de_DirEntity->d_name) ;
                        closedir(dir_proc) ;
                        return pid_ProcessIdentifier ;
                    }
                }
            }
        }
    }
    closedir(dir_proc) ;
    return pid_ProcessIdentifier ;
}

// }
int main() {

	// pid_t pid = GetPIDbyName("StressorTemplate_v8") ; // If -1 = not found, if -2 = proc fs access error
    // printf("PID %d\n", pid);

    pid_t pid = bb1("StressorTemplate_v8") ; // If -1 = not found, if -2 = proc fs access error
    printf("PID %d\n", pid);
    
    return 0;
}
