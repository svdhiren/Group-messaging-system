#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <ctype.h>
#include <sys/shm.h>
#include <signal.h>
#include <poll.h>
#include <pthread.h>
#include <sys/msg.h>



int main()
{
    int num = 12345;
    char msg[128];

    sprintf(msg, "%d", num);
    strcat(msg, ": ");
    char newmsg[128];
    fgets(newmsg, sizeof(newmsg), stdin);
    int n = strlen(newmsg);
    if(newmsg[n-1] == '\n')
        newmsg[n-1] = '\0';
    strcat(msg, newmsg);

    printf("The number (length = %d): %s", (int)strlen(msg), msg);

    int yo=0;
    for(int i=0;msg[i]!=':';i++)
    yo = (yo*10) + (msg[i]-'0');

    printf("The number is: %d", yo);
}