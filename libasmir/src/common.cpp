#include <string>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

using namespace std;

#include "common.h"

#ifdef LOG_TO_STDERR

uint32_t log_stderr_mask = LOG_MSG;

#endif

#ifdef LOG_TO_FILE

FILE *log_file_fd = NULL;
uint32_t log_file_mask = 0;

#endif

//----------------------------------------------------------------------
// Logging functions
//----------------------------------------------------------------------
uint32_t log_stderr(uint32_t mask)
{

#ifdef LOG_TO_STDERR

    int ret = log_stderr_mask;

    log_stderr_mask = mask;

    return ret;

#else

    return 0;

#endif

}

int log_init(uint32_t mask, const char *path)
{

#ifdef LOG_TO_FILE

    log_close();

    // start new log file
    if (log_file_fd = fopen(path, "w"))
    {
        log_file_mask = mask;
        return 0;
    }
    else
    {
        return errno;
    }

#endif

    return EINVAL;
}

void log_close(void)
{

#ifdef LOG_TO_FILE

    if (log_file_fd)
    {
        fclose(log_file_fd);
        log_file_fd = NULL;
    }

#endif

}

void log_write(uint32_t level, const char *msg, ...)
{
    va_list mylist;
    va_start(mylist, msg);    

    char *buff = NULL;
    int len = vsnprintf(NULL, 0, msg, mylist); 

    va_end(mylist);
    va_start(mylist, msg);

    // allocate buffer for message string
    if (len > 0 && (buff = (char *)malloc(len + 1)))
    {
        if (vsnprintf(buff, len + 1, msg, mylist) < 0)
        {
            free(buff);
            buff = NULL;
        }
    }
    
    va_end(mylist);    

    if (buff)
    {

#ifdef LOG_TO_STDERR

        if (level & log_stderr_mask)
        {
            // write message to stderr
            cerr << buff << endl;
        }        

#endif

#ifdef LOG_TO_FILE

        if (level & log_file_mask)
        {
            ostringstream data;
            data << buff << endl;

            if (log_file_fd)
            {
                const char *str = data.str().c_str();

                // write message to file
                fwrite(str, strlen(str), 1, log_file_fd);                
            }
        }
#endif
        free(buff);
    }
}

//----------------------------------------------------------------------
// A panic function that prints a msg and terminates the program
//----------------------------------------------------------------------
void panic(string msg)
{
    string str = "panic(): " + msg;

    throw BapException(str);
}

void _panic(const char *msg)
{
    panic(string(msg));
}
