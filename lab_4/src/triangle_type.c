/* Треугольник задан координатами вершин. Определить тип треугольника.
Ввод: x1, y1, x2, y2, x3, y3
Вывод: 0 - остроугольный, 1 - прямоугольный, 2 - тупоугольный */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int check(float a2, float b2, float c2)
{
    float h = 1e-6;
    int result = 0;
    /* 90 degrees check */
    if (fabsf(a2 - b2 - c2) < h || fabsf(b2 - a2 - c2) < h || fabsf(c2 - b2 - a2) < h)
    {
        result = 1;
    }
    /* >90 degrees check */
    else if (b2 + c2 - a2 < 0 || b2 + a2 - c2 < 0 || a2 + c2 - b2 < 0)
    {
        result = 2;
    }
    return result;
}

void output(int res) {
    switch (res) {
        case (0):
            printf("остроугольный\n");
            break;
        case (1):
            printf("прямоугольный\n");
            break;
        case (2):
            printf("тупоугольный\n");
            break;
        default:
            break;
    }
}

int trncheck(float x1, float y1, float x2, float y2, float x3, float y3) {
    if (fabsf((x1 - x3) * (y2 - y3) - (x2 - x3) * (y1 - y3)) < 1e-6) {
        return -1;
    }
    return 0;
}

int trninput(float *x1, float *y1, float *x2, float *y2, float *x3, float *y3) {
    if (scanf("%f %f %f %f %f %f", x1, y1, x2, y2, x3, y3) != 6)
    {
        return -1;
    }
    return 0;
}

int main()
{
    printf("Введите координаты вершин треугольника:\n");
    float x1, y1, x2, y2, x3, y3;
    if (trninput(&x1, &y1, &x2, &y2, &x3, &y3)) {
        perror("Некорректный ввод вершин треугольника\n");
        exit(1);
    }
    
    if (trncheck(x1, y1, x2, y2, x3, y3) == -1) {
        perror("Не является треугольником\n");
        exit(1);
    }

    float a2 = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
    float b2 = (x3 - x2) * (x3 - x2) + (y3 - y2) * (y3 - y2);
    float c2 = (x3 - x1) * (x3 - x1) + (y3 - y1) * (y3 - y1);
    int res = check(a2, b2, c2);

    printf("Треугольник с вершинами (%.2f, %.2f), (%.2f, %.2f), (%.2f, %.2f) - ",
            x1, y1, x2, y2, x3, y3);
    output(res);
    return 0;
}