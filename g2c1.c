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

#define max 1024

struct mymsg {
    long mtype;
    char mtext[max];
};

struct mymsg mbuf, rcvbuf;
long pid;
int msqid2, g_msqid;

void *reader()
{
    //This thread will be reading from the message queue.    
    while(1)
    {
        struct mymsg rec;        
        msgrcv(msqid2, &rec, sizeof(rec.mtext), pid, 0);

        printf("\t\t%s\n", rec.mtext);
    }
}

void *writer()
{
    char num[128];
    int _pid = (int)getpid(); 
    sprintf(num, "%d", _pid);
    strcat(num, ":");

    char msg[128], newmsg[128], choice;
    

    printf("Type something (Before the message type L for local and G for global): \n");        
    while(1)
    {     

        // scanf("%d", &choice);
        fgets(msg, sizeof(msg), stdin);
        
        choice = msg[0];
        msg[0] = ' ';

        strcpy(newmsg, num);
        strcat(newmsg, msg);
        
        int n = strlen(newmsg);
        if(newmsg[n-1] == '\n')
            newmsg[n-1] = '\0';
        
        strcpy(mbuf.mtext, newmsg);

        if(choice == 'l')
            {
                printf("Sending to local server...\n");
                msgsnd(msqid2, &mbuf, n+1, 0);
            }
        else if(choice == 'g')
            {
                mbuf.mtype = 3;
                printf("Sending to global server...\n");
                msgsnd(g_msqid, &mbuf, n+1, 0);
                mbuf.mtype = 1;
            }
    }
}
int main()
{
    mbuf.mtype = 2;
    int pid = (long)getpid();
    char num[128];    

    key_t key = ftok("msq2", 65);
    msqid2 = msgget(key, IPC_CREAT | 0644); 

    key_t key_g = ftok("g_msq", 65);
    g_msqid = msgget(key_g, IPC_CREAT | 0644);  
        
        
    int _pid = (int)getpid();              
    sprintf(num, "%d", _pid);
    strcat(num, ": pid of g2c1");  
    int len = strlen(num);
    if(num[len-1] == '\n')
        num[len-1] = '\0'; 
    
    strcpy(mbuf.mtext, num);

    printf("Sending: %s\n", mbuf.mtext);    
    msgsnd(msqid2, &mbuf, len+1, 0);
    printf("Message sent from client\n");
    // msgrcv(msqid, &mbuf, sizeof(mbuf.mtext), pid, 0);
    // printf("Client received a message: %s\n", mbuf.mtext);

    pthread_t t1, t2;
    pthread_create(&t1, NULL, reader, NULL);
    pthread_create(&t2, NULL, writer, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;    
}