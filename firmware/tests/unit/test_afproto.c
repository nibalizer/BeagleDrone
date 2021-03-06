#include "tests/test_help.h"
#include "app/afproto.h"
#include "app/buffer.h"
#include "app/crc16.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int escaped_bytes_in(const char *str) {
    int ret = 0;
    while(*str) {
        if(*str == AFPROTO_START_BYTE || *str == AFPROTO_ESC_BYTE ||
           *str == AFPROTO_END_BYTE)
            ++ret;
        ++str;
    }

    return ret;
}

void test_pop_invalid_crc(void) {
    char in_data[512];
    char out_data[256];

    RingBuffer rb_in;
    Buffer output;

    RingBufferInit(&rb_in, in_data, 512);
    BufferInit(&output, out_data, 256);

    RingBufferPushString(&rb_in, "\x7d\x48\x65\x6c\x6c\x6f\xd7\xcb\x7f");
    assert(!afproto_ringbuffer_pop_frame(&rb_in, &output));
    assert(output.used == 0);
}

void test_pop_no_end_byte(void) {
    char in_data[512];
    char out_data[256];

    RingBuffer rb_in;
    Buffer output;

    RingBufferInit(&rb_in, in_data, 512);
    BufferInit(&output, out_data, 256);

    RingBufferPushString(&rb_in, "\x7d\x48\x65\x6c\x6c\x6f\xd6\xcb");
    assert(!afproto_ringbuffer_pop_frame(&rb_in, &output));
    assert(output.used == 0);
}

void test_pop_frame(void) {
    char in_data[512];
    char out_data[256];

    RingBuffer rb_in;
    Buffer output;

    RingBufferInit(&rb_in, in_data, 512);
    BufferInit(&output, out_data, 256);

    RingBufferPushString(&rb_in, "\x7d\x48\x65\x6c\x6c\x6f\xd6\xcb\x7f");

    assert(!afproto_ringbuffer_pop_frame(&rb_in, &output));
    assert(output.used == strlen("Hello"));
    assert(!strncmp(out_data, "Hello", output.used));
}

void test_pop_escaped_crc(void) {
    char in_data[512];
    char out_data[256];

    RingBuffer rb_in;
    Buffer output;

    RingBufferInit(&rb_in, in_data, 512);
    BufferInit(&output, out_data, 256);

    RingBufferPushString(&rb_in, "\x7d\x68\x69\x0c\x7e\x5f\x7f");
    assert(!afproto_ringbuffer_pop_frame(&rb_in, &output));
    assert(output.used == strlen("hi"));
    assert(!strncmp(out_data, "hi", output.used));
}


void test_push_frame(void) {
    char out_data[512];
    const char *test_str = "Hello";
    RingBuffer rb_out;

    RingBufferInit(&rb_out, out_data, 512);

    afproto_ringbuffer_push_frame(&rb_out, test_str, 5);

    int framed_len = strlen(test_str) + escaped_bytes_in(test_str) + 4;

    assert(RingBufferSize(&rb_out) == framed_len);
    assert(out_data[0] == AFPROTO_START_BYTE);
    assert(out_data[framed_len - 1] == AFPROTO_END_BYTE);
    unsigned short framed_crc = *(
            (unsigned short*)(out_data + framed_len - 3));
    assert(framed_crc == crc16_buff(test_str, strlen(test_str)));
}

void test_push_escape_crc_frame(void) {
    char out_data[512];
    const char *test_str = "hi";
    RingBuffer rb_out;

    RingBufferInit(&rb_out, out_data, 512);

    afproto_ringbuffer_push_frame(&rb_out, test_str, 2);

    int framed_len = strlen(test_str) + escaped_bytes_in(test_str) + 5;

    assert(RingBufferSize(&rb_out) == framed_len);
    assert(!strncmp(out_data, "\x7d\x68\x69\x0c\x7e\x5f\x7f", framed_len));
}

int main(int argc, char **argv) {
    TestInfo tests[] = {
        { "Push frame", test_push_frame },
        { "Push escape crc frame", test_push_escape_crc_frame },
        { "Pop frame", test_pop_frame },
        { "Pop no end byte", test_pop_no_end_byte },
        { "Pop invalid crc", test_pop_invalid_crc },
        { "Pop escaped crc", test_pop_escaped_crc },
        { 0, 0 }
    };

    run_tests(tests);
    return 0;
}

