# Arduino 2 Hands Gestures Classification

<!-- <img src="https://user-images.githubusercontent.com/62667537/207033575-91d463d7-9e63-4508-9255-d21490b800a0.png" width="200" height="120"> -->

## **1. Data Acquisition**

In this part, we capture the data of 4 different movements with 2 hands and we copy the date in 8 csv files (4 for left hand and 4 for the right), the files are in `data` folder.

The gestures performed are:

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

At the end of the notebook execution we will have the two models `[model_R.h & model_L.h]` as final result 