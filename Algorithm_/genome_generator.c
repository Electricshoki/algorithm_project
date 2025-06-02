#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEN 5         // ���� �ϳ��� ����
#define MAX_READ 1000 // �ִ� ���� ��

int main() {
    FILE* reference = fopen("reference.txt", "r");
    if (reference == NULL) {
        perror("���� ���� ����");
        return 1;
    }

    char sequence[100000]; // ����� ū ����
    if (fscanf(reference, "%s", sequence) != 1) {
        fprintf(stderr, "������ �б� ����\n");
        fclose(reference);
        return 1;
    }

    fclose(reference);

    int seq_len = strlen(sequence);
    int read_count = 0;

    printf("������ ���� ���:\n");
    for (int i = 0; i <= seq_len - LEN && read_count < MAX_READ; i++) {
        for (int j = 0; j < LEN; j++) {
            putchar(sequence[i + j]);
        }
        putchar('\n');
        read_count++;
    }

    return 0;
}
