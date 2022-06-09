// Fatih Arslan Tugay
// 181101008
#include "defines.h"
#include <pthread.h>

void *matrixOrdering(void *args);

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
        int random_array[ARR_LEN];
        for (int i = 0; i < (ARR_LEN/BLOCK_LEN)+1; i++){ // loops for every blocks, number of blocks is exactly (ARR_LEN/BLOCK_LEN)+1
            msgrcv(msgid, &message_arr, sizeof(message_arr), MSG_ARR_SND, 0); // get block
                for (int j = i*BLOCK_LEN; j < (i+1)*BLOCK_LEN && j < ARR_LEN; j++)
                    random_array[j] = message_arr.arr_msg[j-(i*BLOCK_LEN)]; // write block index by index to random array
            message_txt.type_msg = MSG_ARR_RCVD;
            char message[MSG_LEN];
            sprintf(message, "%d. block of array has been received.", i+1); 
            strcpy(message_txt.txt_msg, message);
            msgsnd(msgid, &message_txt, sizeof(message_txt), 0); // send the message that i'th block has been received
        }

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