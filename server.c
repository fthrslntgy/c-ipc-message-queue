// Fatih Arslan Tugay
// 181101008
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

void *matrixOrdering(void *args);

// structure for messages
struct buf_msg_txt{
    long type_msg;
    char txt_msg[1000];
} message_txt;

struct buf_msg_arr{
    long type_msg;
    int arr_msg[1000];
} message_arr;

// structure for thread function's parameter
struct thread_info {
    int pid;
};

int main(){

    // key and msgid for mailbox
    key_t key;
    int msgid;
    key = ftok("fatih", 25);

    // create the mailbox
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {  
        printf("Error in creating queue\n");  
        exit(0);  
    }  

    // in a loop, wait for a connection and if there is a connection from a client, create a thread for it
    while (1){

        printf("Waiting for connection ...\n");
        // a client's pid received at here
        msgrcv(msgid, &message_txt, sizeof(message_txt), 1, 0);
        int pid;
        pid = atoi(message_txt.txt_msg);
        printf("Connection received from client with PID: %d.\n", pid);
        
        // send an info message to client
        message_txt.type_msg = 2;
        char message[1000]  = "I got your request. I am opening a special queue with your PID as its key.";
        strcpy(message_txt.txt_msg, message);
        msgsnd(msgid, &message_txt, sizeof(message_txt), 0);

        // create a thread for this client
        struct thread_info *info;
        info = malloc(sizeof(struct thread_info));
        info->pid = pid;
        pthread_t thread;
        pthread_create(&thread, NULL, matrixOrdering, info);
        // pthread_join(thread, NULL);
    }

    // close the mailbox
    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}

void *matrixOrdering(void *args){

    // get pid from argument
    struct thread_info *info = args;
    int processId = info->pid;
    printf("Thread created for client with PID: %d\n", processId);

    // create new special message queue for the client with its pid
    int msgid;
    msgid = msgget(processId, 0666 | IPC_CREAT);
    if (msgid == -1) {  
        printf("Error in creating queue\n");  
        exit(0);  
    } 

    // send an info message to client
    message_txt.type_msg = 3;
    char message[1000]  = "Special queue has been created for you.";
    strcpy(message_txt.txt_msg, message);
    msgsnd(msgid, &message_txt, sizeof(message_txt), 0);
    
    // in loop, wait arrays from client
    while(1){
        
        printf("Waiting for client's message as array.\n");  
        msgrcv(msgid, &message_arr, sizeof(message_arr), 4, 0);
        int *random_array;
        random_array = message_arr.arr_msg;
        for(int i = 0; i < 10; i++){
            printf("%d ", random_array[i]);
        }
        printf("\nArray has been received.\n");  
    }

    // close the mailbox
    msgctl(msgid, IPC_RMID, NULL);
    //printf("Thread finished\n");
}