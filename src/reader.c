#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include "../inc/status.h"
#include "../inc/buffer.h"
#include "../inc/buffer_types.h"
#include "../inc/log.h"


int read_file(const char* file_path, struct buffer* buf)
{
        int fd;
        off_t buf_sz;
        size_t read_bytes;

        fd = open(file_path, O_RDONLY);
        if (fd == CCSVCUBE_STATUS_FAILED) {
                /* File failed to open, get the error code and report to user */
                LOG_MESSAGE_ARGS("%s\n", strerror(errno));
                goto ERROR_HANDLER;
        }

        /* set the file pointer to zero */
        lseek(fd, 0, SEEK_SET);
        buf_sz = lseek(fd, 0, SEEK_END);

        if (buf_sz == 0) {
                LOG_MESSAGE("File size is zero");
                goto ERROR_HANDLER;
        }

        /* Reset and read the bites */
        lseek(fd, 0, SEEK_SET);
        init_buffer(buf, buf_sz+1, buffer_type_str);

        read_bytes = read(fd, buf->buf, buf_sz);
        if (read_bytes < buf_sz) {
                LOG_MESSAGE_ARGS("Number of bytes read (%ul) does not match expected\n", read_bytes);
                goto ERROR_HANDLER;
        }

        LOG_MESSAGE_ARGS("File %s read successfully", file_path);
        close(fd);
        return CCSVCUBE_STATUS_SUCCESS;

ERROR_HANDLER:
        if (fd != CCSVCUBE_STATUS_FAILED) {
                close(fd);
        }

        return CCSVCUBE_STATUS_FAILED;
}
