#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

// gcc -o copy_camera.c copy_camera

int main() {
    const char *input_path = "/dev/video0";
    const char *output_path = "/home/pi/VS/fifo/fifo_vhod";

    // Each pixel is 3 bytes in RGB24, resolution is 640x480
    const size_t frame_size = 640 * 480 * 3;
    const size_t bytes_per_row = 640 * 3;
    char *buffer = malloc(frame_size);
    if (!buffer) {
        perror("Memory allocation failed");
        return 1;
    }

    // Open the video device with read-only access
    int video_fd = open(input_path, O_RDONLY);
    if (video_fd == -1) {
        perror("Error opening video device");
        free(buffer);
        return 1;
    }

    // Open the output file with read/write, create if it doesn't exist, truncate if it does
    int file_fd = open(output_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_fd == -1) {
        perror("Error opening output file");
        close(video_fd);
        free(buffer);
        return 1;
    }
    
    // Read exactly one frame from the video device
    ssize_t n_read = read(video_fd, buffer, frame_size);
    if (n_read == -1) {
        perror("Error reading from video device");
        close(video_fd);
        close(file_fd);
        free(buffer);
        return 1;
    }
    else if (n_read != frame_size) {
        fprintf(stderr, "Incomplete read: expected %zu bytes, got %zd bytes\n", frame_size, n_read);
    }

    unsigned char *row_pointer = buffer;
    //int test = 0;
    // Write the frame to the output file
    
    // measure time
    double time_start = clock();

	for (size_t i = 0; i < 480; i++){
        ssize_t n_written = write(file_fd, row_pointer, bytes_per_row);
        //test = test +1;
        //printf("Test: %d\n", test);
        if (n_written != bytes_per_row) {
            perror("Error writing to output file");
            close(video_fd);
            close(file_fd);
            free(buffer);
            return 1;
        }

        row_pointer += bytes_per_row;

    }
    
    double time_end = clock();

	double delta_time = ((double)(time_end - time_start)) / CLOCKS_PER_SEC;
	printf("Delta time FIFO write: %f\n", delta_time);

    row_pointer =  buffer;
    
    // Clean up
    free(buffer);
    close(video_fd);
    close(file_fd);

    printf("Done copy!\n");		

    return 0;
}
