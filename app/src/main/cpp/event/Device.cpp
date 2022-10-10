#include "Event.h"

#define test_bit(bit, array) (array[bit / 8] & (1 << (bit % 8)))

int touch_width;
int touch_height;
const char *reg_ptn = "([0-9]+)  : value 0, min 0, max ([0-9]+)";
;

bool getTouchSize(const char *dev_name)
{
    char cmd[256];
    sprintf(cmd, "getevent -p %s", dev_name);
    char buf[1024];
    FILE *fd;
    printf("%s\n", cmd);
    //test read from shell
    fd = popen(cmd, "r");
    if (fd)
    {
        while (fgets(buf, sizeof(buf), fd))
        {
            // printf("fgets success\n");
            char num_buf[64];
            regex_t reg;
            if (regcomp(&reg, reg_ptn, REG_EXTENDED) != 0)
            {
                // printf("regcomp fail\n");
                return false;
            }

            size_t nmatch = 16;
            regmatch_t pmatch[16];

            if (regexec(&reg, buf, nmatch, pmatch, 0) != REG_NOMATCH)
            {
                memset(num_buf, 0, sizeof(num_buf));
                strncpy(num_buf, buf + pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
                int type = atoi(num_buf);
                switch (type)
                {
                case 35:
                    memset(num_buf, 0, sizeof(num_buf));
                    strncpy(num_buf, buf + pmatch[2].rm_so, pmatch[2].rm_eo - pmatch[2].rm_so);
                    touch_width = atoi(num_buf);
                    break;
                case 36:
                    memset(num_buf, 0, sizeof(num_buf));
                    strncpy(num_buf, buf + pmatch[2].rm_so, pmatch[2].rm_eo - pmatch[2].rm_so);
                    touch_height = atoi(num_buf);
                    break;
                default:
                    break;
                }
            }
            else
            {
                // printf("regexec fail\n");
            }
            regfree(&reg);
        }
        pclose(fd);
    }
    else
    {
        printf("open fd fail\n");
    }
    return true;
}

int getTouchDevice()
{
    int i, fd;
    char dev_name[256];

    unsigned char keybit[(KEY_MAX + 1) / 8] = {0};
    unsigned char absbit[(ABS_MAX + 1) / 8] = {0};
    unsigned char relbit[(REL_MAX + 1) / 8] = {0};

    for (i = 0; i < 255; i++)
    {
        sprintf(dev_name, "/dev/input/event%d", i);

        fd = open(dev_name, O_RDWR);
        if (fd == -1)
            continue;
        // printf("open success:%d\n", i);

        ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keybit)), keybit);
        ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(absbit)), absbit);

        if (test_bit(ABS_MT_POSITION_X, absbit) && test_bit(ABS_MT_POSITION_Y, absbit))
        {
            printf("event:%d\n", i);
            getTouchSize(dev_name);
            return fd;
        }
        else if (test_bit(BTN_TOUCH, keybit) && test_bit(ABS_X, absbit) && test_bit(ABS_Y, absbit))
        {
            printf("event:%d\n", i);
            getTouchSize(dev_name);
            return fd;
        }
        close(fd);
    }
    return 0;
}

void getLinuxRelease(int *major, int *minor)
{
    struct utsname info;
    if (uname(&info) || sscanf(info.release, "%d.%d", major, minor) <= 0)
    {
        *major = 0, *minor = 0;
        printf("Could not get linux version: %s\n", strerror(errno));
    }
}

int createEpoll(int fd)
{
    int major, minor;
    getLinuxRelease(&major, &minor);
    // EPOLLWAKEUP was introduced in kernel 3.5
    bool mUsingEpollWakeup = major > 3 || (major == 3 && minor >= 5);

    int mEpollFd = epoll_create(1);
    // Register with epoll.
    struct epoll_event eventItem;
    memset(&eventItem, 0, sizeof(eventItem));
    eventItem.events = mUsingEpollWakeup ? EPOLLIN : EPOLLIN | EPOLLWAKEUP;
    if (epoll_ctl(mEpollFd, EPOLL_CTL_ADD, fd, &eventItem))
    {
        printf("Could not add device fd to epoll instance.  errno=%d\n", errno);
        return 0;
    }
    return mEpollFd;
}

int hasEvent(int mEpollFd, epoll_event *events, int size)
{
    int pollResult = epoll_wait(mEpollFd, events, size, 0);
    return pollResult;
}
