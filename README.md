# STM32Cube.AI Iris Classifier

An embedded Iris flower classification project deployed on an ATK-NANO STM32F4 development board using STM32F411RCT6, STM32CubeMX, X-CUBE-AI, Keil MDK, and UART communication.

The neural network is trained in Python, exported to TensorFlow Lite format, converted by STM32Cube.AI, and finally executed on the STM32F411RCT6 microcontroller.

The system supports:

* UART input for prediction data
* UART input for labeled validation data
* On-device classification
* Probability output for three classes
* On-device accuracy statistics
* Automatic serial validation using Python

---

## Features

* Iris flower three-class classification
* TensorFlow/Keras model training
* TensorFlow Lite model export
* STM32Cube.AI deployment
* UART-based prediction interface
* UART-based labeled validation
* On-device accuracy calculation
* Python automatic serial testing
* Pretrained model and STM32 project included

---

## Hardware

| Item              | Configuration    |
| ----------------- | ---------------- |
| Development Board | ATK-NANO STM32F4 |
| MCU               | STM32F411RCT6    |
| CPU               | Arm Cortex-M4F   |
| System Clock      | 100 MHz          |
| UART              | USART1           |
| UART TX           | PA9              |
| UART RX           | PA10             |
| Baud Rate         | 115200           |
| Debug Interface   | SWD / ST-Link    |

---

## Software

* Python 3
* TensorFlow / Keras
* NumPy
* scikit-learn
* pandas
* pyserial
* STM32CubeMX
* X-CUBE-AI 10.2.0
* Keil MDK-ARM
* GitHub Desktop

---

## Dataset

This project uses the Iris dataset provided by scikit-learn.

Each sample contains four input features:

1. Sepal length
2. Sepal width
3. Petal length
4. Petal width

The dataset contains three flower classes:

| Label | Class      |
| ----: | ---------- |
|     0 | Setosa     |
|     1 | Versicolor |
|     2 | Virginica  |

The complete dataset contains 150 samples.

The training script uses:

* 80% of the data for training
* 20% of the data for testing
* Stratified train/test splitting
* StandardScaler feature normalization

---

## Neural Network Architecture

```text
Input: 4 features
        |
        v
Dense: 8 neurons, ReLU
        |
        v
Dense: 8 neurons, ReLU
        |
        v
Dense: 3 neurons, Softmax
```

Training configuration:

| Parameter     | Value                           |
| ------------- | ------------------------------- |
| Optimizer     | Adam                            |
| Learning Rate | 0.01                            |
| Loss Function | Sparse Categorical Crossentropy |
| Epochs        | 300                             |
| Batch Size    | 16                              |
| Input Type    | float32                         |
| Output Type   | float32                         |

---

## STM32Cube.AI Analysis

The TensorFlow Lite model was analyzed using X-CUBE-AI.

| Item              |                  Result |
| ----------------- | ----------------------: |
| Input Shape       |          float32, 1 × 4 |
| Output Shape      |          float32, 1 × 3 |
| Parameters        |                     139 |
| MACC              |                     200 |
| Model Weights     |                   556 B |
| Activations       |                    64 B |
| Total Flash Usage | Approximately 10.45 KiB |
| Total RAM Usage   |  Approximately 2.00 KiB |

The model is small enough to run comfortably on the STM32F411RCT6.

---

## Repository Structure

```text
stm32-cubeai-iris-classifier/
├── README.md
├── LICENSE
├── .gitignore
├── requirements.txt
│
├── dataset/
│   ├── iris_dataset.csv
│   └── iris_test.csv
│
├── python/
│   ├── train_iris.py
│   └── serial_test.py
│
├── models/
│   ├── iris_model.keras
│   ├── iris_model.tflite
│   └── iris_params.h
│
├── firmware/
│   └── iris_f411/
│       ├── iris_f411.ioc
│       ├── Core/
│       ├── Drivers/
│       ├── Middlewares/
│       ├── MDK-ARM/
│       └── X-CUBE-AI/
│
├── docs/
│   ├── report/
│   └── images/
│
└── release/
    └── iris_f411.hex
```

The actual directory structure may vary slightly depending on the STM32CubeMX and X-CUBE-AI versions.

---

## Python Environment Setup

Open PowerShell or a terminal in the repository root directory.

```powershell
cd D:\stm32-cubeai-iris-classifier
```

Create a virtual environment:

```powershell
python -m venv .venv
```

Activate the virtual environment:

```powershell
.\.venv\Scripts\Activate.ps1
```

Install the required packages:

```powershell
python -m pip install --upgrade pip
python -m pip install tensorflow numpy scikit-learn pandas pyserial
```

Alternatively, install dependencies using:

```powershell
python -m pip install -r requirements.txt
```

---

## Model Training

Run the training script:

```powershell
python python\train_iris.py
```

The training script generates the following files:

```text
iris_model.keras
iris_model.tflite
iris_params.h
iris_test.csv
```

File descriptions:

| File                | Purpose                                    |
| ------------------- | ------------------------------------------ |
| `iris_model.keras`  | Original Keras model                       |
| `iris_model.tflite` | TensorFlow Lite model used by STM32Cube.AI |
| `iris_params.h`     | Feature normalization parameters for STM32 |
| `iris_test.csv`     | Test dataset used for serial validation    |

---

## Generating the Complete Dataset CSV

The Iris dataset is loaded directly from scikit-learn, so an external CSV file is not required for training.

To generate `iris_dataset.csv`, the training script can use:

```python
from sklearn.datasets import load_iris
import pandas as pd

iris = load_iris()

dataset = pd.DataFrame(
    iris.data,
    columns=[
        "sepal_length",
        "sepal_width",
        "petal_length",
        "petal_width",
    ],
)

dataset["label"] = iris.target
dataset["class_name"] = [
    iris.target_names[index] for index in iris.target
]

dataset.to_csv(
    "iris_dataset.csv",
    index=False,
)

print("Created iris_dataset.csv")
```

---

## STM32CubeMX Configuration

Open the STM32CubeMX project:

```text
firmware/iris_f411/iris_f411.ioc
```

### MCU

```text
STM32F411RCT6
```

### Debug Interface

```text
System Core
└── SYS
    └── Debug: Serial Wire
```

### USART1

```text
Mode: Asynchronous
TX: PA9
RX: PA10
Baud Rate: 115200
Word Length: 8 Bits
Parity: None
Stop Bits: 1
Hardware Flow Control: None
```

Enable:

```text
USART1 global interrupt
```

### CRC

Enable the CRC peripheral.

### System Clock

The system clock is configured using the internal HSI clock:

```text
HSI = 16 MHz
PLLM = 8
PLLN = 100
PLLP = 2
SYSCLK = 100 MHz
HCLK = 100 MHz
PCLK1 = 50 MHz
PCLK2 = 100 MHz
```

### X-CUBE-AI

Import:

```text
models/iris_model.tflite
```

Use the network name:

```text
network
```

The analysis result should show:

```text
Input: f32(1×4)
Output: f32(1×3)
Analyze complete on AI model
```

---

## Keil Build and Flash

Open the Keil project:

```text
firmware/iris_f411/MDK-ARM/iris_f411.uvprojx
```

Build the project:

```text
Project
└── Build Target
```

The build should finish with:

```text
0 Error(s)
```

Flash the firmware:

```text
Flash
└── Download
```

A successful download should display:

```text
Programming Done
Verify OK
```

Building the project does not automatically program the microcontroller. The Flash Download operation is also required.

---

## UART Configuration

Use the following serial terminal settings:

```text
Baud Rate: 115200
Data Bits: 8
Parity: None
Stop Bits: 1
Flow Control: None
```

Each command must end with one of the following:

```text
CR
LF
CRLF
```

Hexadecimal sending must be disabled.

---

## UART Commands

### Prediction Only

Command format:

```text
P,f1,f2,f3,f4
```

Example:

```text
P,5.1,3.5,1.4,0.2
```

Example response:

```text
PRED,class=0,name=setosa,p0=1.0000,p1=0.0000,p2=0.0000
```

The `P` command performs prediction only and does not update the accuracy statistics.

---

### Labeled Validation

Command format:

```text
V,label,f1,f2,f3,f4
```

Example:

```text
V,0,5.1,3.5,1.4,0.2
```

Example response:

```text
VAL,true=0,pred=0,name=setosa,ok=1,total=1,correct=1,acc=100.00%
```

The `V` command compares the predicted class with the ground-truth label and updates the accuracy statistics.

---

### Show Statistics

Command:

```text
S
```

Example response:

```text
STAT,total=30,correct=29,acc=96.67%
```

---

### Reset Statistics

Command:

```text
R
```

Example response:

```text
RESET,total=0,correct=0
```

---

### Show Help

Command:

```text
H
```

or:

```text
?
```

---

## Example Results

The following representative samples were classified correctly:

| Input Features    | Expected Class | Prediction |
| ----------------- | -------------- | ---------- |
| `5.1,3.5,1.4,0.2` | Setosa         | Setosa     |
| `6.0,2.9,4.5,1.5` | Versicolor     | Versicolor |
| `6.5,3.0,5.8,2.2` | Virginica      | Virginica  |

Example serial output:

```text
IRIS Cube.AI UART demo
AI_INIT,OK,input=4,output=3

PRED,class=0,name=setosa,p0=1.0000,p1=0.0000,p2=0.0000
PRED,class=1,name=versicolor,p0=0.0000,p1=1.0000,p2=0.0000
PRED,class=2,name=virginica,p0=0.0000,p1=0.0000,p2=1.0000
```

---

## Automatic Serial Test

Close all serial terminal software before running the Python serial test script.

Only one program can normally access the serial port at a time.

Run:

```powershell
python python\serial_test.py --port COM3
```

Replace `COM3` with the actual serial port.

The script will:

1. Send `R` to reset the statistics
2. Read samples from `iris_test.csv`
3. Send each sample using the `V` command
4. Read the STM32 response
5. Send `S` after all samples are tested
6. Display the final accuracy

---

## Common Issues

### TensorFlow Is Not Installed

Error:

```text
ModuleNotFoundError: No module named 'tensorflow'
```

Solution:

```powershell
python -m pip install tensorflow
```

Make sure the correct Python virtual environment is active.

---

### X-CUBE-AI Cannot Import the Keras Model

Possible error:

```text
InvalidModelError
could not be deserialized properly
```

Use:

```text
iris_model.tflite
```

instead of:

```text
iris_model.keras
```

---

### Keil Cannot Find `usart.h`

Some STM32CubeMX projects do not generate a separate `usart.h`.

Use the following in `iris_uart.c`:

```c
#include "main.h"

extern UART_HandleTypeDef huart1;
```

---

### No Response After Sending a UART Command

Check whether the command ends with:

```text
CR
LF
or CRLF
```

The STM32 firmware processes the command only after receiving a line ending.

---

### Unknown Command Error

Possible cause:

A previous command was sent without a line ending and remained in the UART receive buffer.

Solution:

1. Reset the development board
2. Send a clean command
3. Make sure the command ends with CR, LF, or CRLF

---

### All Inputs Are Predicted as the Same Class

Check:

* Whether `iris_params.h` matches the current model
* Whether the feature order is correct
* Whether input normalization is applied
* Whether the Cube.AI network is initialized more than once
* Whether the model input is `1 × 4`
* Whether the model output is `1 × 3`

---

## Project Documentation

The detailed experiment report is located in:

```text
docs/report/
```

Experiment screenshots are located in:

```text
docs/images/
```

---

## License

The original source code in this project is intended to be released under the MIT License.

STM32 HAL, CMSIS, X-CUBE-AI, and other third-party components remain subject to their own respective licenses.

---

## Acknowledgements

* STMicroelectronics STM32Cube.AI
* STM32CubeMX
* TensorFlow and Keras
* scikit-learn Iris Dataset
* Keil MDK-ARM
