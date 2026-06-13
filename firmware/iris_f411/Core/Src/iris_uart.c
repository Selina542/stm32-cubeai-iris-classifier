#include "iris_uart.h"

#include "iris_ai.h"
#include "main.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
extern UART_HandleTypeDef huart1;

#define RX_LINE_MAX 128U

static uint8_t s_rx_byte = 0U;
static volatile char s_rx_line[RX_LINE_MAX];
static volatile uint16_t s_rx_index = 0U;
static volatile uint8_t s_line_ready = 0U;

static uint32_t s_total = 0U;
static uint32_t s_correct = 0U;
static uint8_t s_ai_ready = 0U;

static const char *const s_class_names[3] = {
    "setosa", "versicolor", "virginica"
};

static void uart_write(const char *text)
{
    if (text == NULL) {
        return;
    }
    (void)HAL_UART_Transmit(
        &huart1,
        (uint8_t *)text,
        (uint16_t)strlen(text),
        HAL_MAX_DELAY
    );
}

static void skip_spaces(const char **cursor)
{
    while ((**cursor == ' ') || (**cursor == '\t')) {
        ++(*cursor);
    }
}

static int consume_comma(const char **cursor)
{
    skip_spaces(cursor);
    if (**cursor != ',') {
        return 0;
    }
    ++(*cursor);
    return 1;
}

static int parse_int_token(const char **cursor, int *value)
{
    int sign = 1;
    int result = 0;
    int digits = 0;

    skip_spaces(cursor);
    if (**cursor == '-') {
        sign = -1;
        ++(*cursor);
    } else if (**cursor == '+') {
        ++(*cursor);
    }

    while ((**cursor >= '0') && (**cursor <= '9')) {
        result = result * 10 + (**cursor - '0');
        ++(*cursor);
        digits = 1;
    }

    if (!digits) {
        return 0;
    }

    *value = sign * result;
    return 1;
}

/* Small decimal parser: no sscanf/strtof and therefore no float-scanf setting. */
static int parse_float_token(const char **cursor, float *value)
{
    int sign = 1;
    int digits = 0;
    float result = 0.0f;
    float fraction_scale = 0.1f;

    skip_spaces(cursor);
    if (**cursor == '-') {
        sign = -1;
        ++(*cursor);
    } else if (**cursor == '+') {
        ++(*cursor);
    }

    while ((**cursor >= '0') && (**cursor <= '9')) {
        result = result * 10.0f + (float)(**cursor - '0');
        ++(*cursor);
        digits = 1;
    }

    if (**cursor == '.') {
        ++(*cursor);
        while ((**cursor >= '0') && (**cursor <= '9')) {
            result += (float)(**cursor - '0') * fraction_scale;
            fraction_scale *= 0.1f;
            ++(*cursor);
            digits = 1;
        }
    }

    if (!digits) {
        return 0;
    }

    *value = (float)sign * result;
    return 1;
}

static int only_spaces_left(const char *cursor)
{
    skip_spaces(&cursor);
    return (*cursor == '\0');
}

static void probability_parts(float probability, int *whole, int *fraction)
{
    int scaled;

    if (probability < 0.0f) {
        probability = 0.0f;
    }
    if (probability > 1.0f) {
        probability = 1.0f;
    }

    scaled = (int)(probability * 10000.0f + 0.5f);
    *whole = scaled / 10000;
    *fraction = scaled % 10000;
}

static void accuracy_parts(uint32_t correct, uint32_t total,
                           uint32_t *whole, uint32_t *fraction)
{
    uint32_t percent_x100;

    if (total == 0U) {
        *whole = 0U;
        *fraction = 0U;
        return;
    }

    percent_x100 = (uint32_t)(
        (((uint64_t)correct * 10000ULL) + ((uint64_t)total / 2ULL)) /
        (uint64_t)total
    );
    *whole = percent_x100 / 100U;
    *fraction = percent_x100 % 100U;
}

static void send_help(void)
{
    uart_write("Commands:\r\n");
    uart_write("  P,f1,f2,f3,f4              predict only\r\n");
    uart_write("  V,label,f1,f2,f3,f4        predict and update accuracy\r\n");
    uart_write("  S                           show statistics\r\n");
    uart_write("  R                           reset statistics\r\n");
    uart_write("Labels: 0=setosa, 1=versicolor, 2=virginica\r\n");
}

static void send_statistics(void)
{
    char message[96];
    uint32_t acc_whole;
    uint32_t acc_fraction;

    accuracy_parts(s_correct, s_total, &acc_whole, &acc_fraction);
    (void)snprintf(
        message,
        sizeof(message),
        "STAT,total=%lu,correct=%lu,acc=%lu.%02lu%%\r\n",
        (unsigned long)s_total,
        (unsigned long)s_correct,
        (unsigned long)acc_whole,
        (unsigned long)acc_fraction
    );
    uart_write(message);
}

static void predict_and_reply(const float features[4], int true_label, int validation)
{
    float probabilities[3] = {0.0f, 0.0f, 0.0f};
    int predicted;
    int p0w, p0f, p1w, p1f, p2w, p2f;
    char message[256];

    if (!s_ai_ready) {
        uart_write("ERR,AI is not initialized\r\n");
        return;
    }

    predicted = IrisAI_Predict(features, probabilities);
    if (predicted < 0) {
        (void)snprintf(
            message,
            sizeof(message),
            "ERR,inference=%d,ai_error=0x%08lX\r\n",
            predicted,
            (unsigned long)IrisAI_LastError()
        );
        uart_write(message);
        return;
    }

    probability_parts(probabilities[0], &p0w, &p0f);
    probability_parts(probabilities[1], &p1w, &p1f);
    probability_parts(probabilities[2], &p2w, &p2f);

    if (validation) {
        uint32_t acc_whole;
        uint32_t acc_fraction;
        int ok = (predicted == true_label) ? 1 : 0;

        ++s_total;
        if (ok) {
            ++s_correct;
        }
        accuracy_parts(s_correct, s_total, &acc_whole, &acc_fraction);

        (void)snprintf(
            message,
            sizeof(message),
            "VAL,true=%d,pred=%d,name=%s,ok=%d,total=%lu,correct=%lu,"
            "acc=%lu.%02lu%%,p0=%d.%04d,p1=%d.%04d,p2=%d.%04d\r\n",
            true_label,
            predicted,
            s_class_names[predicted],
            ok,
            (unsigned long)s_total,
            (unsigned long)s_correct,
            (unsigned long)acc_whole,
            (unsigned long)acc_fraction,
            p0w, p0f, p1w, p1f, p2w, p2f
        );
    } else {
        (void)snprintf(
            message,
            sizeof(message),
            "PRED,class=%d,name=%s,p0=%d.%04d,p1=%d.%04d,p2=%d.%04d\r\n",
            predicted,
            s_class_names[predicted],
            p0w, p0f, p1w, p1f, p2w, p2f
        );
    }

    uart_write(message);
}

static void process_line(const char *line)
{
    const char *cursor;
    float features[4];
    int label;
    int i;

    if ((line == NULL) || (*line == '\0')) {
        return;
    }

    if ((line[0] == 'H') || (line[0] == 'h') || (line[0] == '?')) {
        send_help();
        return;
    }

    if ((line[0] == 'S') || (line[0] == 's')) {
        send_statistics();
        return;
    }

    if ((line[0] == 'R') || (line[0] == 'r')) {
        s_total = 0U;
        s_correct = 0U;
        uart_write("RESET,total=0,correct=0\r\n");
        return;
    }

    if ((line[0] == 'P') || (line[0] == 'p')) {
        cursor = &line[1];
        if (!consume_comma(&cursor)) {
            uart_write("ERR,format; use P,f1,f2,f3,f4\r\n");
            return;
        }
        for (i = 0; i < 4; ++i) {
            if (!parse_float_token(&cursor, &features[i])) {
                uart_write("ERR,number parsing failed\r\n");
                return;
            }
            if ((i < 3) && !consume_comma(&cursor)) {
                uart_write("ERR,missing comma\r\n");
                return;
            }
        }
        if (!only_spaces_left(cursor)) {
            uart_write("ERR,extra characters after f4\r\n");
            return;
        }
        predict_and_reply(features, -1, 0);
        return;
    }

    if ((line[0] == 'V') || (line[0] == 'v')) {
        cursor = &line[1];
        if (!consume_comma(&cursor) || !parse_int_token(&cursor, &label)) {
            uart_write("ERR,format; use V,label,f1,f2,f3,f4\r\n");
            return;
        }
        if ((label < 0) || (label > 2)) {
            uart_write("ERR,label must be 0, 1 or 2\r\n");
            return;
        }
        if (!consume_comma(&cursor)) {
            uart_write("ERR,missing comma after label\r\n");
            return;
        }
        for (i = 0; i < 4; ++i) {
            if (!parse_float_token(&cursor, &features[i])) {
                uart_write("ERR,number parsing failed\r\n");
                return;
            }
            if ((i < 3) && !consume_comma(&cursor)) {
                uart_write("ERR,missing comma\r\n");
                return;
            }
        }
        if (!only_spaces_left(cursor)) {
            uart_write("ERR,extra characters after f4\r\n");
            return;
        }
        predict_and_reply(features, label, 1);
        return;
    }

    uart_write("ERR,unknown command. Send H for help.\r\n");
}

void IrisApp_Init(void)
{
    char message[96];
    int result = IrisAI_Init();

    if (result == 0) {
        s_ai_ready = 1U;
    } else {
        s_ai_ready = 0U;
    }

    (void)HAL_UART_Receive_IT(&huart1, &s_rx_byte, 1U);

    uart_write("\r\nIRIS Cube.AI UART demo\r\n");
    if (s_ai_ready) {
        uart_write("AI_INIT,OK,input=4,output=3\r\n");
    } else {
        (void)snprintf(
            message,
            sizeof(message),
            "AI_INIT,FAIL,result=%d,error=0x%08lX\r\n",
            result,
            (unsigned long)IrisAI_LastError()
        );
        uart_write(message);
    }
    send_help();
}

void IrisApp_Process(void)
{
    char local_line[RX_LINE_MAX];
    uint16_t i;

    if (!s_line_ready) {
        return;
    }

    __disable_irq();
    for (i = 0U; i < RX_LINE_MAX; ++i) {
        local_line[i] = s_rx_line[i];
        if (local_line[i] == '\0') {
            break;
        }
    }
    local_line[RX_LINE_MAX - 1U] = '\0';
    s_line_ready = 0U;
    __enable_irq();

    process_line(local_line);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    char received;

    if ((huart == NULL) || (huart->Instance != USART1)) {
        return;
    }

    received = (char)s_rx_byte;

    if (!s_line_ready) {
        if ((received == '\r') || (received == '\n')) {
            if (s_rx_index > 0U) {
                s_rx_line[s_rx_index] = '\0';
                s_line_ready = 1U;
                s_rx_index = 0U;
            }
        } else if (s_rx_index < (RX_LINE_MAX - 1U)) {
            s_rx_line[s_rx_index] = received;
            ++s_rx_index;
        } else {
            s_rx_index = 0U;
        }
    }

    (void)HAL_UART_Receive_IT(&huart1, &s_rx_byte, 1U);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if ((huart != NULL) && (huart->Instance == USART1)) {
        s_rx_index = 0U;
        (void)HAL_UART_Receive_IT(&huart1, &s_rx_byte, 1U);
    }
}
