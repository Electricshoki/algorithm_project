/*
* �������б� ��ǻ�Ͱ��а� 2021111972 �߱��Ÿ��Ű
* reference.txt�� ������ �������� test_reference�� �����ϰ�, test_reference�� �������� ���带 �ڸ��� �ڵ��̴�. ��ܿ� #define���� ���ǵ� ���븸 �ǵ�� �ȴ�.
*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LEN 30               // ���� ����
#define MAX_SEQ 2000000      // ��ü ������ ����
#define ERROR_RATE 0.03      // ������
#define TEST_LEN 25000        // �׽�Ʈ�� ���۷��� ����
#define NUM_READS 25000        // ������ ��ü ���� ���� (���� ���� ����)

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
        perror("reference.txt ���� ����");
        return 1;
    }

    char* reference = malloc(MAX_SEQ + 1);
    if (!reference) {
        fprintf(stderr, "�޸� �Ҵ� ����\n");
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
        fprintf(stderr, "reference.txt ���̰� TEST_LEN���� ª���ϴ�.\n");
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
        perror("test_reference.txt ���� ����");
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
        perror("MY_genome.txt ���� ����");
        free(reference);
        return 1;
    }
    fprintf(fp_my, "%s\n", my_genome);
    fclose(fp_my);

    int min_required = TEST_LEN - LEN + 1;
    if (NUM_READS < min_required) {
        fprintf(stderr, "NUM_READS�� �ּ� ������ ���� %d �̻��̾�� �մϴ�.( ������ ��ġ�� �κ��� �����ϱ� ���ؼ�.  ) \n", min_required);
        free(reference);
        return 1;
    }

    int extra = NUM_READS - min_required;
    ReadEntry* reads = malloc(sizeof(ReadEntry) * NUM_READS);
    if (!reads) {
        fprintf(stderr, "���� �޸� �Ҵ� ����\n");
        free(reference);
        return 1;
    }

    // �ּ� ���� ���� ���� (�����̵� ������)
    for (int i = 0; i < min_required; i++) {
        strncpy(reads[i].read, my_genome + i, LEN);
        reads[i].read[LEN] = '\0';
        reads[i].start = i;
    }

    // �߰� ���� ���� ����
    for (int i = 0; i < extra; i++) {
        int rand_start = rand_int(TEST_LEN - LEN + 1);
        strncpy(reads[min_required + i].read, my_genome + rand_start, LEN);
        reads[min_required + i].read[LEN] = '\0';
        reads[min_required + i].start = rand_start;
    }

    // ����
    shuffle_reads(reads, NUM_READS);

    FILE* fp_reads = fopen("reads.txt", "w");
    FILE* fp_map = fopen("reads.map.txt", "w");
    if (!fp_reads || !fp_map) {
        perror("reads/map ���� ���� ����");
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

    printf("���� ���� + NUM_READS ���� ���� �Ϸ�\n");
    return 0;
}
