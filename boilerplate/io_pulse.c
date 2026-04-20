#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Define the default target file for the continuous I/O operations
#define DEFAULT_OUTPUT "/tmp/io_pulse.out"

int main(int argc, char *argv[])
{
    // Default sleep duration between I/O bursts is set to 200 milliseconds
    unsigned int sleep_ms = 200;

    // If a sleep duration is provided as a command-line argument, override the default
    // Note: Expecting the duration as the third argument (argv[2])
    if (argc > 2)
        sleep_ms = atoi(argv[2]);

    // Open the target file with specific flags:
    // O_CREAT: Create the file if it doesn't already exist
    // O_WRONLY: Open strictly for writing
    // O_TRUNC: Truncate file to zero length if it already exists
    // 0644: Standard file permissions (Read/write for owner, read-only for others)
    int fd = open(DEFAULT_OUTPUT, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    
    // Check for file opening errors and exit gracefully if it fails
    if (fd < 0) {
        perror("Failed to open output file");
        return 1;
    }

    // Initialize an iteration counter to track the number of I/O pulses
    unsigned int i = 0;

    // Infinite loop to continuously generate disk I/O load
    while (1) {
        i++;

        char buf[64];
        
        // Format the string payload that will be written to the file
        int len = sprintf(buf, "io_pulse iteration=%u\n", i);

        // Execute the write system call to push the buffer to the file descriptor
        write(fd, buf, len);
        
        // Force the filesystem to flush the buffered data to the physical disk immediately.
        // This is crucial; without fsync, we would just be writing to RAM cache, 
        // defeating the purpose of an I/O hog/pulse program.
        fsync(fd);

        // Log the operation to standard output for real-time monitoring
        printf("io_pulse wrote iteration=%u\n", i);
        
        // Ensure the standard output buffer is flushed so it prints to the terminal immediately
        fflush(stdout);

        // Pause execution for the specified time before the next I/O burst.
        // usleep expects microseconds, so we multiply the millisecond value by 1000.
        usleep(sleep_ms * 1000);
    }

    // Close the file descriptor 
    // (Note: this is technically unreachable due to the infinite loop, but kept for good practice)
    close(fd);
    
    return 0;
}
