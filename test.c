#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_EXPR_LEN 256
#define WIDTH 80
#define HEIGHT 25
#define PI 3.14159265358979323846

// Стек для алгоритма Дейкстры
typedef struct StackNode {
    char data[32];
    struct StackNode* next;
} StackNode;

StackNode* stack = NULL;

void push(char* data) {
    StackNode* node = (StackNode*)malloc(sizeof(StackNode));
    strcpy(node->data, data);
    node->next = stack;
    stack = node;
}

char* pop() {
    if (stack == NULL) return NULL;

    StackNode* temp = stack;
    char* result = strdup(temp->data);
    stack = temp->next;
    free(temp);
    return result;
}

int is_stack_empty() {
    return stack == NULL;
}

int precedence(char op) {
    switch(op) {
        case '+':
        case '-': return 1;
        case '*':
        case '/': return 2;
        case '^': return 3;
        default: return 0;
    }
}

int is_operator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^');
}

int format_expression(char* expr, char* formatted) {
    int i, j = 0;
    for (i = 0; expr[i]; i++) {
        if (expr[i] == 'x') {
            formatted[j++] = 'x';
        } else if (expr[i] == '^') {
            formatted[j++] = '^';
        } else if (expr[i] == 's' && i + 2 < strlen(expr) &&
                   expr[i+1] == 'i' && expr[i+2] == 'n') {
            formatted[j++] = 's';
            formatted[j++] = 'i';
            formatted[j++] = 'n';
            i += 2;
        } else if (expr[i] == 'c' && i + 2 < strlen(expr) &&
                   expr[i+1] == 'o' && expr[i+2] == 's') {
            formatted[j++] = 'c';
            formatted[j++] = 'o';
            formatted[j++] = 's';
            i += 2;
        } else if ((expr[i] >= '0' && expr[i] <= '9') || expr[i] == '.' ||
                  expr[i] == '+' || expr[i] == '*' || expr[i] == '/' ||
                  expr[i] == '(' || expr[i] == ')') {
            formatted[j++] = expr[i];
        } else if (expr[i] == '-' && (i == 0 || expr[i-1] == '(' || is_operator(expr[i-1]))) {
            // Обработка унарного минуса
            formatted[j++] = '0';
            formatted[j++] = '-';
        } else {
            return 0; // Недопустимый символ
        }
    }
    formatted[j] = '\0';
    return 1;
}

int to_rpn(char* input, char* output) {
    int i, j = 0;
    char temp[2] = {0};

    for (i = 0; input[i]; i++) {
        if (input[i] == ' ') continue;

        if ((input[i] >= '0' && input[i] <= '9') || input[i] == '.') {
            while ((input[i] >= '0' && input[i] <= '9') || input[i] == '.') {
                output[j++] = input[i++];
            }
            output[j++] = ' ';
            i--;
        }
        else if (input[i] == 'x') {
            output[j++] = 'x';
            output[j++] = ' ';
        }
        else if (is_operator(input[i])) {
            while (!is_stack_empty() && precedence(stack->data[0]) >= precedence(input[i]) &&
                   stack->data[0] != '(') {
                char* op = pop();
                strcpy(output + j, op);
                j += strlen(op);
                output[j++] = ' ';
                free(op);
            }
            temp[0] = input[i];
            temp[1] = '\0';
            push(temp);
        }
        else if (input[i] == '(') {
            temp[0] = '(';
            temp[1] = '\0';
            push(temp);
        }
        else if (input[i] == ')') {
            while (!is_stack_empty() && stack->data[0] != '(') {
                char* op = pop();
                strcpy(output + j, op);
                j += strlen(op);
                output[j++] = ' ';
                free(op);
            }
            if (!is_stack_empty() && stack->data[0] == '(') {
                pop(); // Удалить '('
            }
        }
        else if (i + 2 < strlen(input) &&
                 input[i] == 's' && input[i+1] == 'i' && input[i+2] == 'n') {
            push("sin");
            i += 2;
        }
        else if (i + 2 < strlen(input) &&
                 input[i] == 'c' && input[i+1] == 'o' && input[i+2] == 's') {
            push("cos");
            i += 2;
        }
        else {
            return 0; // Недопустимый символ
        }
    }

    while (!is_stack_empty()) {
        char* op = pop();
        strcpy(output + j, op);
        j += strlen(op);
        output[j++] = ' ';
        free(op);
    }
    if (j > 0) output[j-1] = '\0'; // Удалить последний пробел
    else output[0] = '\0';

    return 1;
}

double evaluate_rpn(char* rpn, double x) {
    double stack[100];
    int top = -1;
    char* token;
    char rpn_copy[MAX_EXPR_LEN * 2];

    // Создаем копию для strtok
    strcpy(rpn_copy, rpn);
    token = strtok(rpn_copy, " ");

    while (token != NULL) {
        if (strcmp(token, "x") == 0) {
            stack[++top] = x;
        }
        else if ((token[0] >= '0' && token[0] <= '9') ||
                (token[0] == '-' && token[1] >= '0' && token[1] <= '9') ||
                token[0] == '.') {
            stack[++top] = atof(token);
        }
        else if (is_operator(token[0])) {
            if (top < 1) return NAN;
            double b = stack[top--];
            double a = stack[top--];

            switch(token[0]) {
                case '+': stack[++top] = a + b; break;
                case '-': stack[++top] = a - b; break;
                case '*': stack[++top] = a * b; break;
                case '/':
                    if (b == 0) return NAN;
                    stack[++top] = a / b;
                    break;
                case '^': stack[++top] = pow(a, b); break;
            }
        }
        else if (strcmp(token, "sin") == 0) {
            if (top < 0) return NAN;
            double a = stack[top--];
            stack[++top] = sin(a);
        }
        else if (strcmp(token, "cos") == 0) {
            if (top < 0) return NAN;
            double a = stack[top--];
            stack[++top] = cos(a);
        }
        else {
            return NAN;
        }

        token = strtok(NULL, " ");
    }

    if (top != 0) return NAN;
    return stack[top];
}

int main() {
    printf("Введите выражение (например: sin(x)): ");

    char expr[MAX_EXPR_LEN];
    char formatted_expr[MAX_EXPR_LEN];
    char rpn_expr[MAX_EXPR_LEN * 2];

    fgets(expr, MAX_EXPR_LEN, stdin);
    expr[strcspn(expr, "\n")] = 0;

    // Форматирование выражения
    if (!format_expression(expr, formatted_expr)) {
        printf("Ошибка: недопустимые символы в выражении\n");
        return 0;
    }

    // Перевод в обратную польскую запись
    if (!to_rpn(formatted_expr, rpn_expr)) {
        printf("Ошибка: неверный формат выражения\n");
        return 0;
    }

    // Инициализация экрана
    char screen[HEIGHT][WIDTH + 1]; // +1 для нулевого символа

    // Заполняем экран пробелами
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            screen[i][j] = ' ';
        }
        screen[i][WIDTH] = '\0';
    }

    // Рисуем координатные оси
    int x_axis = HEIGHT / 2;    // Ось X посередине по вертикали
    int y_axis = WIDTH / 4;     // Ось Y смещена влево (т.к. x от -2π до 2π)

    // Ось X (горизонтальная линия)
    for (int j = 0; j < WIDTH; j++) {
        screen[x_axis][j] = '-';
    }

    // Ось Y (вертикальная линия)
    for (int i = 0; i < HEIGHT; i++) {
        screen[i][y_axis] = '|';
    }

    // Пересечение осей
    screen[x_axis][y_axis] = '+';

    // Подписи осей
    if (x_axis > 0) screen[x_axis - 1][WIDTH - 1] = 'X';
    if (y_axis < WIDTH - 1) screen[0][y_axis + 1] = 'Y';

    // Построение графика
    double x_min = -2 * PI;
    double x_max = 2 * PI;
    double y_min = -1.5;
    double y_max = 1.5;

    for (int x_px = 0; x_px < WIDTH; x_px++) {
        double x = x_min + (x_max - x_min) * x_px / (WIDTH - 1);
        char rpn_copy[MAX_EXPR_LEN * 2];
        strcpy(rpn_copy, rpn_expr);
        double y = evaluate_rpn(rpn_copy, x);

        if (!isnan(y) && y >= y_min && y <= y_max) {
            // Преобразуем координаты: ось Y направлена вверх
            int y_px = (int)((y_max - y) * (HEIGHT - 1) / (y_max - y_min));

            if (y_px >= 0 && y_px < HEIGHT) {
                screen[y_px][x_px] = '*';
            }
        }
    }

    // Вывод графика
    printf("\nГрафик функции: %s\n", expr);
    printf("Диапазон: x ∈ [%.1f, %.1f], y ∈ [%.1f, %.1f]\n\n", x_min, x_max, y_min, y_max);

    for (int i = 0; i < HEIGHT; i++) {
        printf("%s\n", screen[i]);
    }

    // Очистка стека
    while (!is_stack_empty()) {
        free(pop());
    }

    return 0;
}
