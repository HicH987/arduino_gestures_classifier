# Arduino 2 Hands Gestures Classification

## **Project Files**

```bash
|_arduino_src
|   |_Arduino_TensorFlowLite # tenserflowlite library for arduino, this folder must be copied into the arduino library folder
|   |
|   |_IMU_Capture
|   |   |_IMU_Capture.ino # arduino code that capture hand gesture data
|   |
|   |_IMU_Classifier
|   | |_Receiver
|   | | |_model_R.h # right hand model
|   | | |_Receiver.ino # arduino code that do the classification to right hand and recive
|   | |
|   | |_Sender
|   | | |_model_L.h # left hand model
|   | | |_Sender.ino
|
|_data # left and right hand gestures data
|   |_clap_L.csv
|   |_clap_R.csv
|   |_down_L.csv
|   |_down_R.csv
|   |_hi_L.csv
|   |_hi_R.csv
|   |_up_L.csv
|   |_up_R.csv
|
|_creation_two_models.ipynb # notebook use to train and create the 2 models

```

## **0. Intro**

In this project, we have realized a motion detection of two hands using two `Arduino nano BLE 33`. The objective is to acquire the data of the gyroscope of the two arduino by applying 4 gestures chosen with the 2 hands, then train the model with these data and use it for the classification of the gestures. A cable connection between the two arduino has been made to use the `UART` method in order to send a data from one arduino to another during the classification step.

## **1. Data Acquisition**

In this part, we capture the data of 4 different movements with 2 hands and we copy the date in 8 csv files (4 for left hand and 4 for the right), the files are in `data` folder.

The gestures performed are:

<p align="center">
  <table style="width: 50%;"  cellpadding="20">
    <tbody>
    <tr>
        <td><strong>Gesture</strong></td>
        <td><strong>Number of actions performed</strong></td>
        <td><strong>Files</strong></td>
    </tr>
    <tr>
        <td>Clap</td>
        <td>39</td>
        <td>clap_R.csv,<br>clap_L.csv </td>
    </tr>
    <tr>
        <td>Hi</td>
        <td>39</td>
        <td>hi_R.csv,<br>hi_L.csv </td>
    </tr>
    <tr>
        <td>Up</td>
        <td>22</td>
        <td>up_R.csv,<br>up_L.csv </td>
    </tr>
    <tr>
        <td>Down</td>
        <td>20</td>
        <td>down_R.csv,<br>down_L.csv </td>
    </tr>
    </tbody>
</table>
<br>
</p>

## **2. Training of The 2 Models**

Here, we trained 2 neural network models, one for left hand and one for right. We reuse the provided code, and we applied some modification to get 2 models `[model_R.h & model_L.h]` in one run, and we also changed the training optimizer to `sgd` (gradient descent) to avoid overfilling.

The modifications:

- **Crafted useful functions**

  ```python
  def get_input_output(arr_gestures)
  """
  summary:
      This function accepts the list of csv file names of one hand
      (arr_gestures) and perform data preprocessing and
      normalization and return inputs and outputs data

  Args:
      arr_gestures (list): list of gesture names

  Returns:
      inputs(numpy array): features set
      outputs(numpy array): label set
  """
  ```

  ```python
  def splitter_data(inputs, outputs, train_split=0.6, test_split=0.2)
  """
  summary:
      This function spilt the data (input & output) to inputs_train,
      inputs_test, inputs_validate, outputs_train, outputs_test,
      outputs_validate.

  Args:
      inputs(numpy array): features set
      outputs(numpy array): label set

  Returns:
      inputs_train(numpy array): training data
      inputs_test(numpy array):  test data
      inputs_validate(numpy array): validation data
      outputs_train(numpy array): training label
      outputs_test(numpy array): test label
      outputs_validate(numpy array): validation label
  """
  ```

- **Change Optimizer**

  We changed the optimization method of the model from `rmsprop` to `sgd`, because we had an overfitting with `rmsprop`.

  - `rmsprop` : is an optimization algorithm that accelerates the gradient descent.
  - `sgd` : is the classical gradient descent

  We have also modified the metric used to evaluate the models to `accuracy`.

  ```python
  # learning rate
  learn_rate = 0.001

  # gradient decent initialization
  sgd = SGD(learning_rate=learn_rate, momentum=0.9, nesterov=False)

  model.compile(optimizer=sgd, loss="categorical_crossentropy", metrics=["accuracy"])
  ```

  After the training we get 2 models with the following accuracy:

  - Right model:

  ```cmd
  Test loss: 0.01
  Test accuracy: 0.98
  ```

  - Left model:

  ```cmd
  Test loss: 0.015
  Test accuracy: 0.97
  ```

At the end of the notebook execution we will have the two models `model_R.h` & `model_L.h` as final result.

## **3. Two Arduino Connection & Classification**

In this part we connect 2 Arduino by using the `UART` method as shown in the following circuit diagram blow,
the sending data is performed by the instruction `Serial1.print(<some byte>)`.

<p align="center">
<img src="https://user-images.githubusercontent.com/62667537/207033575-91d463d7-9e63-4508-9255-d21490b800a0.png" width="500" height="400">
</p>

The 4 gestures are stored in a list 
```C++
const char *GESTURES[] = {
    "clap",
    "hi",
    "up",
    "down"};
```
The sender arduino will perform a prediction of left-hand gesture and save the index of the gesture with the highest prediction rate, then print the results as shown in the example below.
```cmd
- the prediction percentage of each gesture:
	clap: 0.726154
	hi: 0.233689
	up: 0.000689
	down: 0.039468
--> The current hand gesture is: clap
```
The receiver arduino will make the same prediction for the right-hand.

After the prediction the sender will transmit the predicted index to the receiver as a `char` (Because it's easier to send than an `int`).

```c++
// array of gestures index char
char gestures_idx[4] = {'0', '1', '2', '3'};

// gestures_idx declared as global variable
```
```c++
// send the index of the left gesture to the right arduino
Serial1.print(gestures_idx[argMax]);

// argMax is the index of the gesture with the highest prediction rate
```

The receiver will receive this index and convert it to `int` and then compare it with its own predicted index as shown in the example below.

- if the 2 are equal print:
  ```cmd
  ==> The 2 hand gesture performed is: -> [clap]
  ```
- else:
  ```cmd
  ==> !! This 2 hand gesture is unknown !!
  ```