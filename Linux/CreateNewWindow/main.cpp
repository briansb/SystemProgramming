#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("Create new window\n");
    // https://stackoverflow.com/questions/14532496/try-to-execute-command-line-codes-from-c-linux
    //system("gnome-terminal");
    system("gnome-terminal -x sh -c './stressor; exec bash'");
    system("gnome-terminal -x sh -c './stressor; exec bash'");

    // this works
    //system("terminator -x sh -c './stressor; exec bash'");

    //system("./stressor &");
    
    return 0;
} 
