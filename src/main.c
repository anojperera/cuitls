#include <stdlib.h>
#include <stdio.h>
#include "../inc/buffer_types.h"
#include "../inc/buffer.h"

int main(int argc, char** argv)
{
        struct buffer buf;
        const char tmp_str[] = "This is a string";

        /* initialise the buffer  */
        init_buffer(&buf, 20, buffer_type_str);
        buffer_set_value(&buf, tmp_str, sizeof(tmp_str));

        printf("Buffer Contents: %s\n", buf.buf);
        destroy_buffer(&buf);

        return 0;
}
