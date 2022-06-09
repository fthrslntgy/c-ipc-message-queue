// Fatih Arslan Tugay
// 181101008
#include "defines.h"
#define SLP_TIME 10
// RNDM_BOUND is upper bound of array's random elements. Now they're between 0 and 100
#define RNDM_BOUND 100

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
    key = ftok(KEY_CODE, 25);

    // create the mailbox
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {  
        printf("Error in creating queue!\n");  
        exit(0);  
    }

    // send pid to server
    message_txt.type_msg = MSG_FRST_CNNCT;
    strcpy(message_txt.txt_msg, pid_str);
    msgsnd(msgid, &message_txt, sizeof(message_txt), 0);

    // get servers message
    msgrcv(msgid, &message_txt, sizeof(message_txt), MSG_CNNCT_OK, 0);
    printf("Connected to server, it says: %s\n", message_txt.txt_msg);

    // mailbox with this clients pid
    msgid = msgget(pid, 0666 | IPC_CREAT);
    if (msgid == -1) {  
        printf("Error in creating queue!\n");  
        exit(0);  
    } 

    // get servers message
    msgrcv(msgid, &message_txt, sizeof(message_txt), MSG_SPCL_Q_OK, 0);
    printf("Server says: %s\n", message_txt.txt_msg);

    // calculate number of blocks
    int num_blocks = (int)ceil((float)ARR_LEN/BLOCK_LEN);

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
        // array sending in blocks cause of system variable "msgmax", any big array could not be sized in only one message
        for (int i = 0; i < num_blocks; i++){ // loops for every blocks
            message_arr.type_msg = MSG_ARR_SND;
                for (int j = i*BLOCK_LEN; j < (i+1)*BLOCK_LEN && j < ARR_LEN; j++) 
                    message_arr.arr_msg[j-(i*BLOCK_LEN)] = random_array[j]; // put values into blocks
            msgsnd(msgid, &message_arr, sizeof(message_arr), 0); // send block
            msgrcv(msgid, &message_txt, sizeof(message_txt), MSG_ARR_RCVD, 0); // get the message about i'th block has been received
            printf("Server says: %s\n", message_txt.txt_msg);
        }
        printf("Random array completely sended. Waiting for sorting...\n");

        // get servers message
        msgrcv(msgid, &message_txt, sizeof(message_txt), MSG_ARR_SRTD, 0);
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
            else{
                printf("%d\n", sorted_array[i]);
            }
        }

        // sleep
        printf("Sleeping...\n");
        sleep(SLP_TIME);
    }

    // close the mailbox
    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}
