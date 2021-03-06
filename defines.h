// This header file includes common lines of both c files
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <math.h> // only for calculate block number with ceil()

#define KEY_CODE "fatih"
#define SLP_TIME 10

// ARR_LEN is size of random array. You can change it but dont forget to change it in server.c too
#define ARR_LEN 1000
// BLOCK_LEN is size of integer array in the message of array structure
#define BLOCK_LEN 100
// BLOCK_LEN is size of text messages
#define MSG_LEN 1000
#define PID_LEN 10

// Message code definations, don't need to change
#define MSG_FRST_CNNCT 1
#define MSG_CNNCT_OK 2
#define MSG_SPCL_Q_OK 3
#define MSG_ARR_SND 4
#define MSG_ARR_RCVD 5
#define MSG_ARR_SRTD 6

// structure for thread function's parameter
struct thread_info {
    int pid;
};

// structure for messages
struct buf_msg_txt{
    long type_msg;
    char txt_msg[MSG_LEN];
} message_txt;

struct buf_msg_arr{
    long type_msg;
    int arr_msg[BLOCK_LEN];
} message_arr;

// check this pid is alive or not
int is_alive(int pid){
    
    if (getpgid(pid) == -1)
        return 0;
    return 1;
}
