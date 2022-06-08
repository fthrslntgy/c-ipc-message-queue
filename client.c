// Fatih Arslan Tugay
// 181101008
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define ARR_LEN 10
#define MSG_LEN 1000
#define PID_LEN 10
#define SLP_TIME 10
#define RNDM_SIZE 100

// structure for messages
struct buf_msg_txt{
    long type_msg;
    char txt_msg[MSG_LEN];
} message_txt;

struct buf_msg_arr{
    long type_msg;
    int arr_msg[ARR_LEN];
} message_arr;

int main(){

    // get pid and write it in a string
    int pid = getpid();
    char pidString[PID_LEN];
    sprintf(pidString,"%d",pid);
    printf("My PID: %d.\n", pid);

    // key and msgid for mailbox
    key_t key;
    int msgid;
    key = ftok("fatih", 25);

    // create the mailbox
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {  
        printf("Error in creating queue!\n");  
        exit(0);  
    }

    // send pid to server
    message_txt.type_msg = 1;
    strcpy(message_txt.txt_msg, pidString);
    msgsnd(msgid, &message_txt, sizeof(message_txt), 0);

    // get servers message
    msgrcv(msgid, &message_txt, sizeof(message_txt), 2, 0);
    printf("Connected to server, it says: %s\n", message_txt.txt_msg);

    // mailbox with this clients pid
    msgid = msgget(pid, 0666 | IPC_CREAT);
    if (msgid == -1) {  
        printf("Error in creating queue!\n");  
        exit(0);  
    } 

    // get servers message
    msgrcv(msgid, &message_txt, sizeof(message_txt), 3, 0);
    printf("Server says: %s\n", message_txt.txt_msg);

    while(1){
        // Create random array and print it
        printf("Random Array: ");
        int random_array[ARR_LEN];
        for(int i = 0; i < ARR_LEN; i++){
            random_array[i] = rand() % RNDM_SIZE;
            if(i != ARR_LEN -1)
                printf("%d,", random_array[i]);
            else
                printf("%d\n", random_array[i]);
        }

        // Send random array
        message_arr.type_msg = 4;
        for(int i = 0; i < ARR_LEN; i++) {
            message_arr.arr_msg[i] = random_array[i];
        }
        msgsnd(msgid, &message_arr, sizeof(message_arr), 0);

        // get servers message
        msgrcv(msgid, &message_txt, sizeof(message_txt), 5, 0);
        printf("Server says: %s\n", message_txt.txt_msg);

        // sleep
        sleep(SLP_TIME);
    }

    // close the mailbox
    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}
