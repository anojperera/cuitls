#include "../inc/writer.h"
#include "../inc/log.h"
#include <../inc/status.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * Writing buffer to file blocking
 */
int writer_write_file_blocking(struct buffer *buf, const char *fp)
{
        int fd;
        int stat = CCSVCUBE_STATUS_SUCCESS;
        size_t bw = 0;
        mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

        LOG_MESSAGE("Getting file descriptor");

        fd = open(fp, O_WRONLY | O_CREAT, mode);
        if (fd == -1) {
                LOG_MESSAGE_ARGS("Unable to get a file descriptor :%s\n",
                                 strerror(errno));

                return CCSVCUBE_STATUS_FAILED;
        }

        bw = write(fd, get_buffer(buf), get_buffer_sz(buf));
        if (bw == -1) {
                LOG_MESSAGE_ARGS("Unable to write file %s due to: %s", fp,
                                 strerror(errno));
                stat = CCSVCUBE_STATUS_FAILED;
                goto ERROR_HANDLER;
        }

        LOG_MESSAGE_ARGS("Successfully written %u bytes to file %s", bw, fp);

ERROR_HANDLER:
        LOG_MESSAGE("Closing file");
        close(fd);

        return stat;
}
