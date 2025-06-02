#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEN 5         // 리드 하나의 길이
#define MAX_READ 1000 // 최대 리드 수

int main() {
    FILE* reference = fopen("reference.txt", "r");
    if (reference == NULL) {
        perror("파일 열기 실패");
        return 1;
    }

    char sequence[100000]; // 충분히 큰 버퍼
    if (fscanf(reference, "%s", sequence) != 1) {
        fprintf(stderr, "시퀀스 읽기 실패\n");
        fclose(reference);
        return 1;
    }

    fclose(reference);

    int seq_len = strlen(sequence);
    int read_count = 0;

    printf("생성된 리드 목록:\n");
    for (int i = 0; i <= seq_len - LEN && read_count < MAX_READ; i++) {
        for (int j = 0; j < LEN; j++) {
            putchar(sequence[i + j]);
        }
        putchar('\n');
        read_count++;
    }

    return 0;
}
