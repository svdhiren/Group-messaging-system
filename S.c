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

#define max 128
#define n 1

struct mymsg {
    long mtype;
    char mtext[max];
};

int msqid, msqid2, g_msqid;
long pids1[n], pids2[n];

int get_pid(char msg[])
{
    int p = 0;
    for(int j=0;msg[j]!=':';j++)
    {
        p = p*10 + (msg[j]-'0');
    }

    return p;
}

void *group1()
{
    //This thread of the server will receive message from group 1.
    struct mymsg rcvbuf;
    struct mymsg send;
    printf("Waiting for messages in group 1...\n");
    while(1)
    {
        msgrcv(msqid, &rcvbuf, sizeof(rcvbuf.mtext), 1, 0);
        //Now send it to all the clients except the client from which this message was received.
        int pid = get_pid(rcvbuf.mtext);
        printf("Server of group 1 received a message %s\n", rcvbuf.mtext);

        for(int i=0;i<n;i++)
        {
            if(1 || (pid != pids1[i]))
            {                
                send.mtype = pids1[i];
                strcpy(send.mtext, rcvbuf.mtext);
                printf("Sending this message to pid: %ld\n\n", pids1[i]);
                // strcpy(mbuf[i].mtext, rcvbuf.mtext);
                msgsnd(msqid, &send, strlen(send.mtext)+1, 0);
            }
        }

    }
}

void *group2()
{
    //This thread of the server will receive message from group 1.
    struct mymsg rcvbuf;
    struct mymsg send;
    while(1)
    {
        msgrcv(msqid2, &rcvbuf, sizeof(rcvbuf.mtext), 2, 0);
        //Now send it to all the clients except the client from which this message was received.
        int pid = get_pid(rcvbuf.mtext);
        printf("Server of group 2 received a message %s\n", rcvbuf.mtext);

        for(int i=0;i<n;i++)
        {            
            if(1 || (pid != pids2[i]))
            {                
                send.mtype = pids2[i];
                strcpy(send.mtext, rcvbuf.mtext);
                printf("Sending this message to pid: %ld\n\n", pids2[i]);
                // strcpy(mbuf[i].mtext, rcvbuf.mtext);
                msgsnd(msqid2, &send, strlen(send.mtext)+1, 0);
            }
        }

    }
}

// void *global1()
// {
//     struct mymsg send, rcvbuf;
//     while(1)
//     {
//         // printf("Waiting for messages from clients...\n");
//         msgrcv(g_msqid, &rcvbuf, sizeof(rcvbuf.mtext), 4, 0);
//         //Now send it to all the clients except the client from which this message was received.
//         int pid = get_pid(rcvbuf.mtext);
//         printf("Global server received a message %s\n", rcvbuf.mtext);
//         // print_info();
//         for(int i=0;i<n;i++)
//         {            
//             if(1 || (pid != pids2[i]))
//             {               
//                 send.mtype = pids2[i];
//                 strcpy(send.mtext, rcvbuf.mtext);
//                 printf("Sending this message to pid: %ld\n\n", pids2[i]);
//                 // strcpy(mbuf[i].mtext, rcvbuf.mtext);
//                 msgsnd(msqid2, &send, strlen(send.mtext)+1, 0);
//             }
//         }
//     }
// }

// void handler(int sig)
// {     
//     msgctl(msqid, IPC_RMID, NULL);
//     msgctl(msqid2, IPC_RMID, NULL);
//     msgctl(g_msqid, IPC_RMID, NULL);
    
//     printf("Removing message queues and terminating...\n");
//     signal(SIGINT, SIG_DFL);    
//     raise(SIGINT);

// }

int main()
{
    struct mymsg mbuf, rcvbuf;  

    // signal(SIGINT, handler);  

    //Create the message queue.
    key_t key1 = ftok("msq1", 65);
    msqid = msgget(key1, IPC_CREAT | 0644); //This is for
    
    key_t key2 = ftok("msq2", 65);
    msqid2 = msgget(key2, IPC_CREAT | 0644);

    key_t key = ftok("g_msq", 65);
    g_msqid = msgget(key, IPC_CREAT | 0644);

    //Here first we need to receive the pids of the clients. After that the clients
    //will be able to connect with each other.
    for(int i=0;i<n;i++)
    {
        msgrcv(msqid, &mbuf, sizeof(mbuf.mtext), 1, 0); //Server will always read type 1.        
        //Now we need to extract the pid from the character array.
        pids1[i] = get_pid(mbuf.mtext);        
        printf("Server received the pid: %ld\n", pids1[i]);
    }

    for(int i=0;i<n;i++)
    {
        msgrcv(msqid2, &mbuf, sizeof(mbuf.mtext), 2, 0); //Server will always read type 1.        
        //Now we need to extract the pid from the character array.
        pids2[i] = get_pid(mbuf.mtext);
        printf("Server received the pid: %ld\n", pids2[i]);
    }

    //We received pids of the clients.

    pthread_t t1, t2, t3;
    pthread_create(&t1, NULL, group1, NULL);
    pthread_create(&t2, NULL, group2, NULL);
    // pthread_create(&t3, NULL, global1, NULL);
    
    //Start receiving the messages from the clients.
    printf("Waiting for messages from global clients...\n");
    struct mymsg send;
    while(1)
    {
        // printf("Waiting for messages from clients...\n");
        msgrcv(g_msqid, &rcvbuf, sizeof(rcvbuf.mtext), 3, 0);
        //Now send it to all the clients except the client from which this message was received.
        int pid = get_pid(rcvbuf.mtext);
        printf("Global server received a message %s\n", rcvbuf.mtext);
        // print_info();
        for(int i=0;i<n;i++)
        {            
            if(1 || (pid != pids1[i]))
            {                
                send.mtype = pids1[i];
                strcpy(send.mtext, rcvbuf.mtext);
                printf("Sending this message to pid: %ld\n\n", pids1[i]);
                // strcpy(mbuf[i].mtext, rcvbuf.mtext);
                msgsnd(msqid, &send, strlen(send.mtext)+1, 0);
            }

            if(1 || (pid != pids2[i]))
            {                
                send.mtype = pids2[i];
                strcpy(send.mtext, rcvbuf.mtext);
                printf("Sending this message to pid: %ld\n\n", pids2[i]);
                // strcpy(mbuf[i].mtext, rcvbuf.mtext);
                msgsnd(msqid2, &send, strlen(send.mtext)+1, 0);
            }
        }
    }

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    // pthread_join(t3, NULL);
    // printf("Waiting to receive the pid of the client...\n");
    // msgrcv(msqid, &mbuf, sizeof(mbuf.mtext), 0, 0);
    // printf("The pid received is : %ld\n", mbuf.mtype);
    // strcpy(mbuf.mtext, "Hey bro how are you?");    
    // msgsnd(msqid, &mbuf, strlen(mbuf.mtext)+1, 0);
    // printf("Server sent the message...\n");
    
}

