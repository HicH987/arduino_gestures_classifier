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
        <td>clap_R.csv, clap_L.csv </td>
    </tr>
    <tr>
        <td>Hi</td>
        <td>39</td>
        <td>hi_R.csv, hi_L.csv </td>
    </tr>
    <tr>
        <td>Up</td>
        <td>22</td>
        <td>up_R.csv, up_L.csv </td>
    </tr>
    <tr>
        <td>Down</td>
        <td>20</td>
        <td>down_R.csv, down_L.csv </td>
    </tr>
    </tbody>
</table>
<br>

## **2. Training of The 2 Models**

Here, we trained 2 neural network models, one for left hand and one for right. We reuse the provided code, and we applied some modification to get 2 models in one run.

The modifications:

1.  Crafted useful functions

    ```python
    def get_input_output(arr_gestures)
    ```

    **summary**:

    This function accepts the list of csv file names of one hand (arr_gestures) and perform data preprocessing and normalization and return inputs and outputs data

    **Args**:

        arr_gestures (list): list of gesture names

    **Returns**:

        inputs(numpy array): features set
        outputs(numpy array): label set

    ---
    ```python
    splitted_data(inputs, outputs, train_split=0.6, test_split=0.2)
    ```

    **summary**:

    This function spilt the data (input & output) to inputs_train, inputs_test, inputs_validate, outputs_train, outputs_test, outputs_validate.

    **Args**:

          arr_gestures (list): list of gesture names

    **Returns**:

          inputs(numpy array): features set
          outputs(numpy array): label set
