#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/poll.h>
#include <stdint.h>
#include <sys/time.h>

#define MAX_BUFFER_SIZE         512
unsigned char readBuf[MAX_BUFFER_SIZE];

#define NUM_MESSAGES            10000
#define DEVICE_NAME             "/dev/mpu-605030"

using namespace std;

int main()
{
    cout << "pru-i2c-arm started" << endl; // prints !!!Hello World!!!
    struct pollfd pollfds[1];
    int result = 0;

    /* Open the rpmsg_pru character device file */
    pollfds[0].fd = open(DEVICE_NAME, O_RDWR);

    /*
     * If the RPMsg channel doesn't exist yet the character device
     * won't either.
     * Make sure the PRU firmware is loaded and that the rpmsg_pru
     * module is inserted.
     */
    if (pollfds[0].fd < 0)
    {
        printf("Failed to open %s\n", DEVICE_NAME);
        return -1;
    }

    /* The RPMsg channel exists and the character device is opened */
    printf("Opened %s, sending %d messages\n\n", DEVICE_NAME, NUM_MESSAGES);

    /* Send 'hello world!' to the PRU through the RPMsg channel */
    result = write(pollfds[0].fd, "ST", 3);
    if (result > 0)
    {
        printf("Message START sent to PRU\n");
    }
    else
    {
        printf("Sorry!! cannot send message START sent to PRU\n");
        close(pollfds[0].fd);
        return -1;
    }
    result = read(pollfds[0].fd, readBuf, 3);
    if (result > 0)
    {
        printf("Received %s from PRU\n", readBuf);
    }
    else
    {
        printf("Sorry!! cannot receive confirm on START from PRU\n");
        close(pollfds[0].fd);
        return -1;
    }
    int16_t counter = -1;
    struct  timeval now; // wall clock times
    struct  timeval later;
    gettimeofday(&now, NULL); // wall clock time when CPU time first read
    gettimeofday(&later, NULL); // wall clock time when CPU time has ticked
    uint32_t usec;
    while (1)
    {
        counter == 1000 ? counter = 0 : counter++;
        {
            uint16_t j;
            for (j = 0; j < MAX_BUFFER_SIZE; j++)
            {
                readBuf[j] = '\0';
            }
        }
        result = read(pollfds[0].fd, readBuf, MAX_BUFFER_SIZE);
        if (result > 0 && (counter == 0))
        {
            if (readBuf[0] == 'T')
            {
                if (readBuf[1] == 'T')
                {
                    printf("Received %s from PRU\n", readBuf);
                }
                else
                {
                    printf("Received KO message from PRU [%s]\n", readBuf);
                }
            } else if (readBuf[0] == 'M')
            {
                if (readBuf[1] == 'P')
                {
                    printf("Received %s from PRU\n", readBuf);
                } else if (readBuf[1] == '6')
                {

                    gettimeofday(&later, NULL); // wall clock time when CPU time has ticked
                    usec = (((unsigned long long)later.tv_sec) * 1000000ULL + later.tv_usec) - (((unsigned long long)now.tv_sec) * 1000000ULL + now.tv_usec);
                    uint16_t* values = (uint16_t*)(readBuf + 2);
                    printf("used=%d Received %c%c ax=[%d], ay=[%d] az=[%d], gx=[%d], gy=[%d] gz=[%d] from PRU\n",
                           usec, readBuf[0], readBuf[1], (int16_t)values[0], (int16_t)values[1],
                           (int16_t)values[2], (int16_t)values[3], (int16_t)values[4],
                           (int16_t)values[5]);
                    gettimeofday(&now, NULL); // wall clock time when CPU time first read
                }
               else
                {
                    printf("Received KO message from PRU [%s]\n", readBuf);
                }
            }
            else if (readBuf[0] == 'H')
            {
                if (readBuf[1] == 'T')
                {
                    if(readBuf[2] == 'K') {
                        uint32_t values[3] = {0,0,0};
                        values[0] = *((uint32_t*)(readBuf + 4));
                        values[1] = *((uint32_t*)(readBuf + 8));
                        values[2] = *((uint32_t*)(readBuf + 12));
                        printf("Received %c%c%c A=[%lu], B=[%lu] C=[%lu] from PRU\n",
                               readBuf[0], readBuf[1], readBuf[2], (unsigned long)values[0], (unsigned long)values[1],
                               (unsigned long)values[2]);
                    } else {
                        printf("Received %s from PRU\n", readBuf);
                    }
                }
                else if (readBuf[1] == 'D')
                {
                    uint16_t values[3] = {0,0,0};
                    uint16_t* valBuf = (uint16_t*)(readBuf + 2);
                    values[0] = (0xFF00 & ((*valBuf) << 8)) | (0x00FF & ((*valBuf) >> 8));
                    values[1] = (0xFF00 & ((*(valBuf+1)) << 8)) | (0x00FF & ((*(valBuf+1)) >> 8));
                    values[2] = (0xFF00 & ((*(valBuf+2)) << 8)) | (0x00FF & ((*(valBuf+2)) >> 8));
                    printf("Received %c%c X=[%d], Y=[%d] Z=[%d] from PRU\n",
                           readBuf[0], readBuf[1], (int16_t)values[0], (int16_t)values[1],
                           (int16_t)values[2]);
                }
                else if (readBuf[1] == 'S')
                {
                    uint16_t values[3] = {0,0,0};
                    uint16_t* valBuf = (uint16_t*)(readBuf + 2);
                    values[0] = (0xFF00 & ((*valBuf) << 8)) | (0x00FF & ((*valBuf) >> 8));
                    values[1] = (0xFF00 & ((*(valBuf+1)) << 8)) | (0x00FF & ((*(valBuf+1)) >> 8));
                    values[2] = (0xFF00 & ((*(valBuf+2)) << 8)) | (0x00FF & ((*(valBuf+2)) >> 8));
                    printf("SelfTests %c%c X=[%d], Y=[%d] Z=[%d] from PRU\n",
                           readBuf[0], readBuf[1], (int16_t)values[0], (int16_t)values[1],
                           (int16_t)values[2]);
                }
                else
                {
                    printf("Received KO message from PRU [%s]\n", readBuf);
                }
            }
            else
            {
                printf("Sorry!! cannot invalid message format from PRU. [%s]\n",
                       readBuf);
                close(pollfds[0].fd);
                return -1;
            }
        }
    }
    /* Received all the messages the example is complete */
    printf("Received %d messages, closing %s\n", NUM_MESSAGES, DEVICE_NAME);

    /* Close the rpmsg_pru character device file */
    close(pollfds[0].fd);

    return 0;
}

