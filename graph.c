#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_EXPR_LEN 256
#define WIDTH 80
#define HEIGHT 25
#define PI 3.14159265358979323846

// Стек для RPN
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

void clear_stack() {
    while (!is_stack_empty()) {
        free(pop());
    }
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
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
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
        } else if (expr[i] == 't' && i + 2 < strlen(expr) &&
                   expr[i+1] == 'a' && expr[i+2] == 'n') {
            formatted[j++] = 't';
            formatted[j++] = 'a';
            formatted[j++] = 'n';
            i += 2;
        } else if (expr[i] == 'c' && i + 2 < strlen(expr) &&
                   expr[i+1] == 't' && expr[i+2] == 'g') {
            formatted[j++] = 'c';
            formatted[j++] = 't';
            formatted[j++] = 'g';
            i += 2;
        } else if (expr[i] == 's' && i + 3 < strlen(expr) &&
                   expr[i+1] == 'q' && expr[i+2] == 'r' && expr[i+3] == 't') {
            formatted[j++] = 's';
            formatted[j++] = 'q';
            formatted[j++] = 'r';
            formatted[j++] = 't';
            i += 3;
        } else if (expr[i] == 'l' && i + 1 < strlen(expr) && expr[i+1] == 'n') {
            formatted[j++] = 'l';
            formatted[j++] = 'n';
            i += 1;
        } else if ((expr[i] >= '0' && expr[i] <= '9') || expr[i] == '.' ||
                  expr[i] == '+' || expr[i] == '*' || expr[i] == '/' ||
                  expr[i] == '(' || expr[i] == ')') {
            formatted[j++] = expr[i];
        } else if (expr[i] == '-' && (i == 0 || expr[i-1] == '(' || is_operator(expr[i-1]))) {
            formatted[j++] = '0';
            formatted[j++] = '-';
        } else {
            return 0;
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
                pop();
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
        else if (i + 2 < strlen(input) &&
                 input[i] == 't' && input[i+1] == 'a' && input[i+2] == 'n') {
            push("tan");
            i += 2;
        }
        else if (i + 2 < strlen(input) &&
                 input[i] == 'c' && input[i+1] == 't' && input[i+2] == 'g') {
            push("ctg");
            i += 2;
        }
        else if (i + 3 < strlen(input) &&
                 input[i] == 's' && input[i+1] == 'q' && input[i+2] == 'r' && input[i+3] == 't') {
            push("sqrt");
            i += 3;
        }
        else if (i + 1 < strlen(input) &&
                 input[i] == 'l' && input[i+1] == 'n') {
            push("ln");
            i += 1;
        }
        else {
            return 0;
        }
    }

    while (!is_stack_empty()) {
        char* op = pop();
        strcpy(output + j, op);
        j += strlen(op);
        output[j++] = ' ';
        free(op);
    }
    if (j > 0) output[j-1] = '\0';
    else output[0] = '\0';

    return 1;
}

double evaluate_rpn(char* rpn, double x) {
    double stack[100];
    int top = -1;
    char* token;
    char rpn_copy[MAX_EXPR_LEN * 2];

    strcpy(rpn_copy, rpn);
    token = strtok(rpn_copy, " ");

    while (token != NULL) {
        if (strcmp(token, "x") == 0) {
            stack[++top] = x;
        }
        else if ((token[0] >= '0' && token[0] <= '9') || token[0] == '.') {
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
        else if (strcmp(token, "tan") == 0) {
            if (top < 0) return NAN;
            double a = stack[top--];
            // Проверка на асимптоты тангенса
            if (cos(a) == 0) return NAN;
            stack[++top] = tan(a);
        }
        else if (strcmp(token, "ctg") == 0) {
            if (top < 0) return NAN;
            double a = stack[top--];
            // Проверка на асимптоты котангенса
            if (sin(a) == 0) return NAN;
            stack[++top] = 1.0 / tan(a);
        }
        else if (strcmp(token, "sqrt") == 0) {
            if (top < 0) return NAN;
            double a = stack[top--];
            // Проверка на отрицательное число под корнем
            if (a < 0) return NAN;
            stack[++top] = sqrt(a);
        }
        else if (strcmp(token, "ln") == 0) {
            if (top < 0) return NAN;
            double a = stack[top--];
            // Проверка на неположительное число под логарифмом
            if (a <= 0) return NAN;
            stack[++top] = log(a);
        }
        else {
            return NAN;
        }

        token = strtok(NULL, " ");
    }

    if (top != 0) return NAN;
    return stack[top];
}

int round_value(double value) {
    return (int)(value + (value >= 0 ? 0.5 : -0.5));
}

int main() {
    char expr[MAX_EXPR_LEN];
    char formatted_expr[MAX_EXPR_LEN];
    char rpn_expr[MAX_EXPR_LEN * 2];

    fgets(expr, MAX_EXPR_LEN, stdin);
    expr[strcspn(expr, "\n")] = 0;

    // Форматирование выражения
    if (!format_expression(expr, formatted_expr)) {
        printf("n/a\n");
        return 0;
    }

    // Перевод в RPN
    if (!to_rpn(formatted_expr, rpn_expr)) {
        printf("n/a\n");
        return 0;
    }

    // Инициализация экрана (только точки)
    char screen[HEIGHT][WIDTH];
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            screen[i][j] = '.';
        }
    }

    // Построение графика
    for (int x_px = 0; x_px < WIDTH; x_px++) {
        double x = (4 * PI) * ((double)x_px / (WIDTH - 1));
        char rpn_copy[MAX_EXPR_LEN * 2];
        strcpy(rpn_copy, rpn_expr);
        double y = evaluate_rpn(rpn_copy, x);

        if (!isnan(y) && y >= -1.0 && y <= 1.0) {
            // Преобразование координат:
            // Ось Y направлена вниз, ось X направлена направо
            // Центр координат: (0, 12) - середина левой границы
            int y_px = round_value(12.0 + y * 12.0);

            if (y_px >= 0 && y_px < HEIGHT) {
                screen[y_px][x_px] = '*';
            }
        }
    }

    // Вывод графика
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            printf("%c", screen[i][j]);
        }
        printf("\n");
    }

    clear_stack();
    return 0;
}
