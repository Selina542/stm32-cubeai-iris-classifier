#ifndef IRIS_AI_H
#define IRIS_AI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Returns 0 on success, negative on failure. */
int IrisAI_Init(void);

/*
 * raw_input order:
 *   [0] sepal length
 *   [1] sepal width
 *   [2] petal length
 *   [3] petal width
 *
 * probabilities receives the three softmax outputs.
 * Returns predicted class 0..2, or a negative value on failure.
 */
int IrisAI_Predict(const float raw_input[4], float probabilities[3]);

/* Packed X-CUBE-AI error: high 16 bits=type, low 16 bits=code. */
uint32_t IrisAI_LastError(void);

#ifdef __cplusplus
}
#endif

#endif /* IRIS_AI_H */
