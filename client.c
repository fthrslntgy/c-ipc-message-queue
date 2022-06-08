// Fatih Arslan Tugay
// 181101008
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// structure for messages
struct buf_msg_txt{
    long type_msg;
    char txt_msg[1000];
} message_txt;

struct buf_msg_arr{
    long type_msg;
    int arr_msg[1000];
} message_arr;

int main(){

    int pid = getpid();
    char pidString[10];
    sprintf(pidString,"%d",pid);
    printf("My PID: %d\n", pid);

    // send pid to server
    key_t key;
    int msgid;
    key = ftok("fatih", 25);
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {  
        printf("Error in creating queue\n");  
        exit(0);  
    }  
    message_txt.type_msg = 1;
    strcpy(message_txt.txt_msg, pidString);
    msgsnd(msgid, &message_txt, sizeof(message_txt), 0);

    msgrcv(msgid, &message_txt, sizeof(message_txt), 2, 0);
    printf("Connected to server, it says: %s\n", message_txt.txt_msg);

    // after here, the communication will be provided with the queue that is created with pid of this client
    msgid = msgget(pid, 0666 | IPC_CREAT);
    if (msgid == -1) {  
        printf("Error in creating queue\n");  
        exit(0);  
    }  
    msgrcv(msgid, &message_txt, sizeof(message_txt), 3, 0);
    printf("Server says: %s\n", message_txt.txt_msg);

    while(1){
        // Create random array
        printf("Random Array: \n");
        int random_array[1000];
        for(int i = 0; i < 10; i++){
            random_array[i] = rand();
            printf("%d, ", random_array[i]);
        }
        // Send random array
        message_arr.type_msg = 4;
        for(int i = 0; i < 10; i++) {
            message_arr.arr_msg[i] = random_array[i];
        }
        msgsnd(msgid, &message_arr, sizeof(message_arr), 0);
        printf("\nRandom Array sended\n");
        sleep(10);
    }

    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}
