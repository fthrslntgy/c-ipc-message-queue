// Fatih Arslan Tugay
// 181101008
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
// ARR_LEN is size of random array. You can change it but dont forget to change it in server.c too
#define ARR_LEN 1000
#define MSG_LEN 1000
#define PID_LEN 10
#define SLP_TIME 10
// RNDM_BOUND is upper bound of array's random elements. Now they're between 0 and 100
#define RNDM_BOUND 100 

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
    char pid_str[PID_LEN];
    char filename[PID_LEN + 4];
    sprintf(pid_str,"%d",pid);
    strcpy(filename, pid_str);
    strcat(filename, ".txt");
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
    strcpy(message_txt.txt_msg, pid_str);
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
        printf("\n\nRandom Array: ");
        int random_array[ARR_LEN];
        for(int i = 0; i < ARR_LEN; i++){
            random_array[i] = rand() % RNDM_BOUND;
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

        // read context of file
        FILE *file;
        if ((file = fopen(filename, "r")) == NULL) {
            printf("File can not be opened: %s!\n", filename);
            exit(1);
        }

        // read file context and save it as string, delimiter is COMMA
        int size_of_file = ARR_LEN*RNDM_BOUND;
        char file_context [size_of_file];
        int sorted_array [ARR_LEN];
        fscanf(file, "%s", file_context);
        fclose(file);
        char * token = strtok(file_context, ",");
        int i = 0;
        while( token != NULL ) {
            sorted_array[i++] = atoi(token);
            token = strtok(NULL, ",");
        }

        // print sorted array
        for(int i = 0; i < ARR_LEN; i++){
            if(i != ARR_LEN -1)
                printf("%d,", sorted_array[i]);
            else
                printf("%d\n", sorted_array[i]);
        }

        // sleep
        sleep(SLP_TIME);
    }

    // close the mailbox
    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}
