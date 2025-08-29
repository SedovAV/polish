#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_EXPR_LEN 256
#define WIDTH 80
#define HEIGHT 25
#define PI 3.14159265358979323846

typedef enum {
    LEX_TYPE_NUMBER,
    LEX_TYPE_OPERATOR,
    LEX_TYPE_FUNCTION,
    LEX_TYPE_PARENTHESIS
} LexType;

typedef struct {
    LexType type;
    char value[32];
} Lexeme;

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

int to_rpn(char* input, char* output) {
    int i, j = 0;
    for (i = 0; input[i]; i++) {
        if (isspace(input[i])) continue;

        if (isdigit(input[i]) || (input[i] == '-' && isdigit(input[i+1]))) {
            while (isdigit(input[i]) || input[i] == '.' || input[i] == '-') {
                output[j++] = input[i++];
            }
            output[j++] = ' ';
            i--;
        } else if (is_operator(input[i])) {
            while (!is_stack_empty() && precedence(stack->data[0]) >= precedence(input[i])) {
                strcpy(output + j, pop());
                j += strlen(output + j);
                output[j++] = ' ';
            }
            char op[2] = {input[i], '\0'};
            push(op);
        } else if (input[i] == '(') {
            char paren[2] = {input[i], '\0'};
            push(paren);
        } else if (input[i] == ')') {
            while (!is_stack_empty() && stack->data[0][0] != '(') {
                strcpy(output + j, pop());
                j += strlen(output + j);
                output[j++] = ' ';
            }
            if (!is_stack_empty()) pop();
        } else {
            return 0;
        }
    }

    while (!is_stack_empty()) {
        strcpy(output + j, pop());
        j += strlen(output + j);
        output[j++] = ' ';
    }
    output[j-1] = '\0';

    return 1;
}

double evaluate_rpn(char* rpn, double x) {
    double stack[100];
    int top = -1;

    char* token = strtok(rpn, " ");
    while (token != NULL) {
        if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))) {
            stack[++top] = atof(token);
        } else if (strcmp(token, "x") == 0) {
            stack[++top] = x;
        } else if (is_operator(token[0])) {
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
        } else {
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

int format_expression(char* expr, char* formatted) {
    int i, j = 0;
    for (i = 0; expr[i]; i++) {
        if (expr[i] == 'x') {
            formatted[j++] = 'x';
        } else if (expr[i] == '^') {
            formatted[j++] = '^';
        } else if (expr[i] == 's' && expr[i+1] == 'i' && expr[i+2] == 'n') {
            formatted[j++] = 's';
            formatted[j++] = 'i';
            formatted[j++] = 'n';
            i += 2;
        } else if (expr[i] == 'c' && expr[i+1] == 'o' && expr[i+2] == 's') {
            formatted[j++] = 'c';
            formatted[j++] = 'o';
            formatted[j++] = 's';
            i += 2;
        } else if (isdigit(expr[i]) || isspace(expr[i]) || is_operator(expr[i]) ||
                  expr[i] == '(' || expr[i] == ')') {
            formatted[j++] = expr[i];
        } else {
            return 0;
        }
    }
    formatted[j] = '\0';
    return 1;
}

int main() {
    char expr[MAX_EXPR_LEN];
    char formatted_expr[MAX_EXPR_LEN];
    char rpn_expr[MAX_EXPR_LEN * 2];

    printf("Enter expression: ");
    fgets(expr, MAX_EXPR_LEN, stdin);

    expr[strcspn(expr, "\n")] = 0;

    if (!format_expression(expr, formatted_expr)) {
        printf("n/a\n");
        return 0;
    }

    if (!to_rpn(formatted_expr, rpn_expr)) {
        printf("n/a\n");
        return 0;
    }

    char screen[HEIGHT][WIDTH];
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            screen[i][j] = '.';
        }
        screen[i][WIDTH] = '\0';
    }

    for (int x_px = 0; x_px < WIDTH; x_px++) {
        double x = (4 * PI) * ((double)x_px / (WIDTH - 1));
        double y = evaluate_rpn(rpn_expr, x);

        if (isnan(y) || y < -1 || y > 1) continue;

        int y_px = round_value((y + 1) * (HEIGHT - 1) / 2);
        if (y_px >= 0 && y_px < HEIGHT) {
            screen[y_px][x_px] = '*';
        }
    }

    for (int i = 0; i < HEIGHT; i++) {
        printf("%s\n", screen[i]);
    }

    return 0;
}
