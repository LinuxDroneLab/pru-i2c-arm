
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/poll.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE         512
char readBuf[MAX_BUFFER_SIZE];

#define NUM_MESSAGES            10000
#define DEVICE_NAME             "/dev/rpmsg_pru30"


using namespace std;

int main() {
    cout << "pru-i2c-arm started" << endl; // prints !!!Hello World!!!
    struct pollfd pollfds[1];
    int i;
    int result = 0;

    /* Open the rpmsg_pru character device file */
    pollfds[0].fd = open(DEVICE_NAME, O_RDWR);

    /*
     * If the RPMsg channel doesn't exist yet the character device
     * won't either.
     * Make sure the PRU firmware is loaded and that the rpmsg_pru
     * module is inserted.
     */
    if (pollfds[0].fd < 0) {
            printf("Failed to open %s\n", DEVICE_NAME);
            return -1;
    }

    /* The RPMsg channel exists and the character device is opened */
    printf("Opened %s, sending %d messages\n\n", DEVICE_NAME, NUM_MESSAGES);

    /* Send 'hello world!' to the PRU through the RPMsg channel */
    result = write(pollfds[0].fd, "START", 6);
    if (result > 0)
            printf("Message START sent to PRU\n");

    result = read(pollfds[0].fd, readBuf, 8);
    if (result > 0)
            printf("Message received from PRU:%s\n\n", readBuf);

    struct EcapData
    {
        char cmd[8];
        uint32_t reg1;
        uint32_t reg2;
        uint32_t reg3;
        uint32_t reg4;
    };
    printf("Sizeof struct: %d ", sizeof(struct EcapData));

    for (i = 0; i < NUM_MESSAGES; i++) {
        result = read(pollfds[0].fd, readBuf, sizeof(struct EcapData));
        struct EcapData *data = (struct EcapData *) readBuf;
            uint32_t reg1 = data->reg1;
            uint32_t reg2 = data->reg2;
            uint32_t reg3 = data->reg3;
            uint32_t reg4 = data->reg4;
            if (result > 0)
                    printf("Message received from PRU:%s, 1-%lu, 2-%lu, 3-%lu, 4-%lu\n", data->cmd, (unsigned long)reg1, (unsigned long)reg2, (unsigned long)reg3, (unsigned long)reg4);
    }
    /* Received all the messages the example is complete */
    printf("Received %d messages, closing %s\n", NUM_MESSAGES, DEVICE_NAME);

    /* Close the rpmsg_pru character device file */
    close(pollfds[0].fd);

    return 0;
}


