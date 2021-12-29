#include <string.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

int strinput(char *str, int len) {
    for (int i = 0; i < len; i++) {
        scanf("%c", str + i);
    }

    if (str[len - 1] != '\n') {
        perror("Некорректный размер строки\n");
        return -1;
    }
    str[len - 1] = '\0';

    return 0;
}

void uppercase(char *str, int len) {
    for (int i = 0; i < len - 1; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] -= 'a' - 'A';
        }
    }
}

int main() {
    int len;

    printf("Введите длину строки: ");
    scanf("%d", &len);
    getchar();

    if (len < 1) {
        perror("Некорректный размер строки\n");
        exit(1);
    }
    
    len++;
    char str[len];

    printf("Введите строку: ");
    if (strinput(str, len) == -1) {
        perror("Некорректный ввод\n");
        exit(1);
    }
    uppercase(str, len);

    printf("%s\n", str);

    return 0;
}