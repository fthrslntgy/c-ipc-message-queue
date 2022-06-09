// Fatih Arslan Tugay
// 181101008
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

#define KEY_CODE "fatih"
// ARR_LEN is size of random array. You can change it but dont forget to change it in server.c too
#define ARR_LEN 1000
#define MSG_LEN 1000
#define PID_LEN 10
// Message code definations
#define MSG_FRST_CNNCT 1
#define MSG_CNNCT_OK 2
#define MSG_SPCL_Q_OK 3
#define MSG_ARR_SND 4
#define MSG_ARR_SRTD 5

void *matrixOrdering(void *args);

// structure for messages
struct buf_msg_txt{
    long type_msg;
    char txt_msg[MSG_LEN];
} message_txt;

struct buf_msg_arr{
    long type_msg;
    int arr_msg[ARR_LEN];
} message_arr;

// structure for thread function's parameter
struct thread_info {
    int pid;
};

int main(){

    // key and msgid for mailbox
    key_t key;
    int msgid;
    key = ftok(KEY_CODE, 25);

    // create the mailbox
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {  
        printf("Error in creating queue!\n");  
        exit(0);  
    }  

    // in a loop, wait for a connection and if there is a connection from a client, create a thread for it
    while (1){

        printf("Waiting for connection...\n");
        // a client's pid received at here
        msgrcv(msgid, &message_txt, sizeof(message_txt), MSG_FRST_CNNCT, 0);
        int pid;
        pid = atoi(message_txt.txt_msg);
        printf("Connection received from client with PID: %d.\n", pid);
        
        // send an info message to client
        message_txt.type_msg = MSG_CNNCT_OK;
        char message[MSG_LEN]  = "I got your PID. I am opening a special queue with your PID as its key.";
        strcpy(message_txt.txt_msg, message);
        msgsnd(msgid, &message_txt, sizeof(message_txt), 0);

        // create a thread for this client
        struct thread_info *info;
        info = malloc(sizeof(struct thread_info));
        info->pid = pid;
        pthread_t thread;
        pthread_create(&thread, NULL, matrixOrdering, info);
    }

    // close the mailbox
    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}

void *matrixOrdering(void *args){

    // get pid from argument and set filename for save this client's array as sorted
    struct thread_info *info = args;
    int pid = info->pid;
    char pid_str[PID_LEN];
    char filename[PID_LEN + 4];
    sprintf(pid_str,"%d",pid);
    strcpy(filename, pid_str);
    strcat(filename, ".txt");

    // create new special message queue for the client with its pid
    int msgid;
    msgid = msgget(pid, 0666 | IPC_CREAT);
    if (msgid == -1) {  
        printf("Error in creating queue!\n");  
        exit(0);  
    } 

    // send an info message to client
    message_txt.type_msg = MSG_SPCL_Q_OK;
    char message[MSG_LEN]  = "Special queue has been created for you.";
    strcpy(message_txt.txt_msg, message);
    msgsnd(msgid, &message_txt, sizeof(message_txt), 0);
    
    // in loop, wait arrays from client
    while(1){
        
        // in here, client sends array
        msgrcv(msgid, &message_arr, sizeof(message_arr), MSG_ARR_SND, 0);
        int *random_array;
        random_array = message_arr.arr_msg;

        // sort array in "not descending" order
        int a;
        for (int i = 0; i < ARR_LEN; ++i) {
            for (int j = i + 1; j < ARR_LEN; ++j) {
                if (random_array[i] > random_array[j]) {
                    a =  random_array[i];
                    random_array[i] = random_array[j];
                    random_array[j] = a;
                }
            }
        }

        // open file and write sorted array, delimiter is COMMA
        FILE *file;
        if ((file = fopen(filename, "w")) == NULL) {
            printf("File can not be opened: %s!\n", filename);
            exit(1);
        }
        for(int i = 0; i < ARR_LEN-1; i++){
            fprintf(file, "%d,", random_array[i]);
        }
        fprintf(file, "%d", random_array[ARR_LEN-1]);
        fclose(file);

        // send client an info message
        message_txt.type_msg = MSG_ARR_SRTD;
        char message[MSG_LEN]  = "Your array sorted and writed into <YOUR_PID>.txt.";
        strcpy(message_txt.txt_msg, message);
        msgsnd(msgid, &message_txt, sizeof(message_txt), 0);
    }

    // close the mailbox
    msgctl(msgid, IPC_RMID, NULL);
}