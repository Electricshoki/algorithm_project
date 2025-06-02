/*
* 동국대학교 컴퓨터공학과 2021111972 야기시타쇼키
* reference.txt의 내용을 바탕으로 test_reference를 생성하고, test_reference의 내용으로 리드를 자르는 코드이다. 상단에 #define으로 정의된 내용만 건들면 된다.
*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LEN 30               // 리드 길이
#define MAX_SEQ 2000000      // 전체 시퀀스 버퍼
#define ERROR_RATE 0.03      // 오류율
#define TEST_LEN 25000        // 테스트용 레퍼런스 길이
#define NUM_READS 25000        // 생성할 전체 리드 개수 (복원 보장 포함)

const char BASES[] = "ACGT";

typedef struct {
    char read[LEN + 1];
    int start;
} ReadEntry;

char mutate(char base) {
    char new_base;
    do {
        new_base = BASES[rand() % 4];
    } while (new_base == base);
    return new_base;
}

int rand_int(int max) {
    return rand() % max;
}

void shuffle_reads(ReadEntry* arr, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand_int(i + 1);
        ReadEntry temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

int main() {
    FILE* fp = fopen("reference.txt", "rb");
    if (!fp) {
        perror("reference.txt 열기 실패");
        return 1;
    }

    char* reference = malloc(MAX_SEQ + 1);
    if (!reference) {
        fprintf(stderr, "메모리 할당 실패\n");
        fclose(fp);
        return 1;
    }
    reference[0] = '\0';

    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = '\0';
        strcat(reference, line);
    }
    fclose(fp);

    int seq_len = strlen(reference);
    if (seq_len < TEST_LEN) {
        fprintf(stderr, "reference.txt 길이가 TEST_LEN보다 짧습니다.\n");
        free(reference);
        return 1;
    }

    srand((unsigned int)time(NULL));
    int ref_start = rand_int(seq_len - TEST_LEN + 1);

    char test_reference[TEST_LEN + 1];
    strncpy(test_reference, reference + ref_start, TEST_LEN);
    test_reference[TEST_LEN] = '\0';

    FILE* fp_test_ref = fopen("test_reference.txt", "w");
    if (!fp_test_ref) {
        perror("test_reference.txt 열기 실패");
        free(reference);
        return 1;
    }
    fprintf(fp_test_ref, "%s\n", test_reference);
    fclose(fp_test_ref);

    char my_genome[TEST_LEN + 1];
    for (int i = 0; i < TEST_LEN; i++) {
        if ((rand() % 10000) < (int)(ERROR_RATE * 10000)) {
            my_genome[i] = mutate(test_reference[i]);
        }
        else {
            my_genome[i] = test_reference[i];
        }
    }
    my_genome[TEST_LEN] = '\0';

    FILE* fp_my = fopen("MY_genome.txt", "w");
    if (!fp_my) {
        perror("MY_genome.txt 열기 실패");
        free(reference);
        return 1;
    }
    fprintf(fp_my, "%s\n", my_genome);
    fclose(fp_my);

    int min_required = TEST_LEN - LEN + 1;
    if (NUM_READS < min_required) {
        fprintf(stderr, "NUM_READS는 최소 복원을 위해 %d 이상이어야 합니다.( 무조건 겹치는 부분을 생성하기 위해서.  ) \n", min_required);
        free(reference);
        return 1;
    }

    int extra = NUM_READS - min_required;
    ReadEntry* reads = malloc(sizeof(ReadEntry) * NUM_READS);
    if (!reads) {
        fprintf(stderr, "리드 메모리 할당 실패\n");
        free(reference);
        return 1;
    }

    // 최소 복원 리드 생성 (슬라이딩 윈도우)
    for (int i = 0; i < min_required; i++) {
        strncpy(reads[i].read, my_genome + i, LEN);
        reads[i].read[LEN] = '\0';
        reads[i].start = i;
    }

    // 추가 랜덤 리드 생성
    for (int i = 0; i < extra; i++) {
        int rand_start = rand_int(TEST_LEN - LEN + 1);
        strncpy(reads[min_required + i].read, my_genome + rand_start, LEN);
        reads[min_required + i].read[LEN] = '\0';
        reads[min_required + i].start = rand_start;
    }

    // 셔플
    shuffle_reads(reads, NUM_READS);

    FILE* fp_reads = fopen("reads.txt", "w");
    FILE* fp_map = fopen("reads.map.txt", "w");
    if (!fp_reads || !fp_map) {
        perror("reads/map 파일 열기 실패");
        free(reference); free(reads);
        return 1;
    }

    for (int i = 0; i < NUM_READS; i++) {
        fprintf(fp_reads, "%s\n", reads[i].read);
        fprintf(fp_map, "read_%03d %d\n", i + 1, reads[i].start);
    }

    fclose(fp_reads);
    fclose(fp_map);
    free(reads);
    free(reference);

    printf("완전 복원 + NUM_READS 조절 지원 완료\n");
    return 0;
}
