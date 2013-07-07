#ifndef COMMUNICATION_H
#define COMMUNICATION_H

/* There are two buffers of this size to allow for frame decoding. */
#define COMMUNICATION_UART_IN_BUFF_SIZE     1024
#define COMMUNICATION_UART_OUT_BUFF_SIZE    1024

#define COMMUNICATION_CHECK_MAX_TIME 100

// Send data in afproto frame
void CommunicationSend(const char *data, unsigned int data_size);

// Received a char of input, handle it
void CommunicationGotChar(char ch);

void CommunicationInit(void);

#endif
