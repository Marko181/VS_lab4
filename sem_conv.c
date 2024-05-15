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

int main() {
    int shmid, semid;
    char *src_buffer;
    const size_t src_size = 640 * 480 * 3;
    const size_t dst_size = 640 * 480 * 2;
    char *dst_buffer = malloc(dst_size);
    if (!dst_buffer) {
        perror("Failed to allocate memory for destination buffer");
        return 1;
    }

    // Generate unique keys for shared memory and semaphore
    key_t shm_key = ftok(SHM_KEY_PATH, SHM_KEY_ID);
    key_t sem_key = ftok(SEM_KEY_PATH, SEM_KEY_ID);

    // Access shared memory
    shmid = shmget(shm_key, src_size, 0644);
    if (shmid == -1) {
        perror("shmget failed");
        return 1;
    }

    // Attach to shared memory
    src_buffer = (char *)shmat(shmid, NULL, SHM_RDONLY);
    if (src_buffer == (char *)(-1)) {
        perror("shmat failed");
        return 1;
    }

    // Access semaphore
    semid = semget(sem_key, 1, 0);
    if (semid == -1) {
        perror("semget failed");
        return 1;
    }

    struct sembuf sb = {0, 0, 0};  // Semaphore structure for locking

    while (1) {
        // Lock the semaphore
        sb.sem_op = -1;
        semop(semid, &sb, 1);

        // Process each pixel from RGB24 to RGB16
        for (int i = 0; i < 640 * 480; i++) {
            char R = src_buffer[i * 3];
            char G = src_buffer[i * 3 + 1];
            char B = src_buffer[i * 3 + 2];

            dst_buffer[i * 2 + 1] = (R & 0xF8) | ((G & 0xE0) >> 5);
            dst_buffer[i * 2] = ((G & 0x1C) << 3) | ((B & 0xF8) >> 3);
        }

        // Unlock the semaphore
        sb.sem_op = 1;
        semop(semid, &sb, 1);

        // Optionally, output or save the converted data
        // Placeholder: handle or output `dst_buffer` as needed
    }

    // Cleanup
    shmdt(src_buffer);
    free(dst_buffer);

    return 0;
}
