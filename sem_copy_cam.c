#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHM_KEY_PATH "/home/pi"
#define SHM_KEY_ID 65
#define SEM_KEY_PATH "/home/pi"
#define SEM_KEY_ID 66

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

int main() {
    const char *input_path = "/dev/video0";

    // Each pixel is 3 bytes in RGB24, resolution is 640x480
    const size_t frame_size = 640 * 480 * 3;
    char *buffer;
    int shmid, semid;
    struct sembuf sb = {0, -1, 0};  // Semaphore structure for locking

    // Generate unique keys for shared memory and semaphore
    key_t shm_key = ftok(SHM_KEY_PATH, SHM_KEY_ID);
    key_t sem_key = ftok(SEM_KEY_PATH, SEM_KEY_ID);

    // Create shared memory
    shmid = shmget(shm_key, frame_size, 0644|IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        return 1;
    }

    // Attach shared memory
    buffer = (char *)shmat(shmid, NULL, 0);
    if (buffer == (char *)(-1)) {
        perror("shmat failed");
        return 1;
    }

    // Create semaphore
    semid = semget(sem_key, 1, 0644|IPC_CREAT);
    if (semid == -1) {
        perror("semget failed");
        return 1;
    }

    // Initialize semaphore
    union semun arg;
    arg.val = 1;  // Binary semaphore
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        perror("semctl failed");
        return 1;
    }

    // Open the video device with read-only access
    int video_fd = open(input_path, O_RDONLY);
    if (video_fd == -1) {
        perror("Error opening video device");
        return 1;
    }

    // Main loop to copy frames to shared memory
    while (1) {
        // Lock the semaphore
        sb.sem_op = -1;
        semop(semid, &sb, 1);

        // Read a frame from the video device
        if (read(video_fd, buffer, frame_size) != frame_size) {
            fprintf(stderr, "Incomplete read from video device\n");
            // Unlock the semaphore before breaking
            sb.sem_op = 1;
            semop(semid, &sb, 1);
            break;
        }

        // Unlock the semaphore
        sb.sem_op = 1;
        semop(semid, &sb, 1);
    }

    // Cleanup
    close(video_fd);
    shmdt(buffer);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);

    return 0;
}
