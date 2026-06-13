#include "iris_ai.h"

#include "main.h"
#include "network.h"
#include "network_data.h"
#include "iris_params.h"

#include <stddef.h>

#if (AI_NETWORK_IN_1_SIZE != 4)
#error "The imported model must have exactly 4 float input values."
#endif

#if (AI_NETWORK_OUT_1_SIZE != 3)
#error "The imported model must have exactly 3 output values."
#endif

AI_ALIGNED(32)
static ai_u8 s_activations[AI_NETWORK_DATA_ACTIVATIONS_SIZE];

static ai_handle s_network = AI_HANDLE_NULL;
static ai_buffer *s_ai_input = NULL;
static ai_buffer *s_ai_output = NULL;
static float s_normalized_input[4];
static uint32_t s_last_error = 0U;

static void save_error(ai_error err)
{
    s_last_error = (((uint32_t)err.type) << 16) | ((uint32_t)err.code);
}

uint32_t IrisAI_LastError(void)
{
    return s_last_error;
}

int IrisAI_Init(void)
{
    ai_error err;
    const ai_handle activation_addresses[] = {
        AI_HANDLE_PTR(s_activations)
    };

    if (s_network != AI_HANDLE_NULL) {
        return 0;
    }

    /* X-CUBE-AI runtime requires the STM32 CRC peripheral clock. */
    __HAL_RCC_CRC_CLK_ENABLE();

    err = ai_network_create_and_init(&s_network, activation_addresses, NULL);
    if (err.type != AI_ERROR_NONE) {
        save_error(err);
        s_network = AI_HANDLE_NULL;
        return -1;
    }

    s_ai_input = ai_network_inputs_get(s_network, NULL);
    s_ai_output = ai_network_outputs_get(s_network, NULL);
    if ((s_ai_input == NULL) || (s_ai_output == NULL)) {
        s_last_error = 0xFFFFFFFFU;
        return -2;
    }

    s_last_error = 0U;
    return 0;
}

int IrisAI_Predict(const float raw_input[4], float probabilities[3])
{
    ai_i32 batches;
    ai_error err;
    int predicted = 0;
    int i;

    if ((raw_input == NULL) || (probabilities == NULL)) {
        return -10;
    }

    if (s_network == AI_HANDLE_NULL) {
        if (IrisAI_Init() != 0) {
            return -11;
        }
    }

    for (i = 0; i < 4; ++i) {
        s_normalized_input[i] = (raw_input[i] - IRIS_MEAN[i]) / IRIS_SCALE[i];
    }

    s_ai_input[0].data = AI_HANDLE_PTR(s_normalized_input);
    s_ai_output[0].data = AI_HANDLE_PTR(probabilities);

    batches = ai_network_run(s_network, &s_ai_input[0], &s_ai_output[0]);
    if (batches != 1) {
        err = ai_network_get_error(s_network);
        save_error(err);
        return -12;
    }

    for (i = 1; i < 3; ++i) {
        if (probabilities[i] > probabilities[predicted]) {
            predicted = i;
        }
    }

    return predicted;
}
