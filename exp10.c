#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int fifo(int pages[], int n, int frames) {
    int frame[frames], i, j, k, faults = 0, index = 0, found;

    for (i = 0; i < frames; i++)
        frame[i] = -1;

    for (i = 0; i < n; i++) {
        found = 0;

        for (j = 0; j < frames; j++) {
            if (frame[j] == pages[i]) {
                found = 1;
                break;
            }
        }

        if (!found) {
            frame[index] = pages[i];
            index = (index + 1) % frames;
            faults++;
        }
    }

    return faults;
}

int lru(int pages[], int n, int frames) {
    int frame[frames], used[frames];
    int i, j, k, faults = 0, time = 0, found;

    for (i = 0; i < frames; i++) {
        frame[i] = -1;
        used[i] = 0;
    }

    for (i = 0; i < n; i++) {
        found = 0;

        for (j = 0; j < frames; j++) {
            if (frame[j] == pages[i]) {
                time++;
                used[j] = time;
                found = 1;
                break;
            }
        }

        if (!found) {
            int min = used[0], pos = 0;

            for (j = 0; j < frames; j++) {
                if (frame[j] == -1) {
                    pos = j;
                    break;
                }
                if (used[j] < min) {
                    min = used[j];
                    pos = j;
                }
            }

            frame[pos] = pages[i];
            time++;
            used[pos] = time;
            faults++;
        }
    }

    return faults;
}

int optimal(int pages[], int n, int frames) {
    int frame[frames];
    int i, j, k, faults = 0, found;

    for (i = 0; i < frames; i++)
        frame[i] = -1;

    for (i = 0; i < n; i++) {
        found = 0;

        for (j = 0; j < frames; j++) {
            if (frame[j] == pages[i]) {
                found = 1;
                break;
            }
        }

        if (!found) {
            int pos = -1, farthest = i + 1;

            for (j = 0; j < frames; j++) {
                int k;
                for (k = i + 1; k < n; k++) {
                    if (frame[j] == pages[k])
                        break;
                }

                if (k == n) {
                    pos = j;
                    break;
                }

                if (k > farthest) {
                    farthest = k;
                    pos = j;
                }
            }

            if (pos == -1)
                pos = 0;

            frame[pos] = pages[i];
            faults++;
        }
    }

    return faults;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <reference_length> <max_frames>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int max_frames = atoi(argv[2]);

    if (max_frames < 1 || max_frames > 7) {
        printf("Number of frames must be between 1 and 7\n");
        return 1;
    }

    int pages[n];
    int i;

    srand(time(NULL));

    printf("Reference String:\n");
    for (i = 0; i < n; i++) {
        pages[i] = rand() % 10;
        printf("%d ", pages[i]);
    }
    printf("\n\n");

    printf("Frames\tFIFO\tLRU\tOptimal\n");

    for (i = 1; i <= max_frames; i++) {
        printf("%d\t%d\t%d\t%d\n",
               i,
               fifo(pages, n, i),
               lru(pages, n, i),
               optimal(pages, n, i));
    }

    return 0;
}
