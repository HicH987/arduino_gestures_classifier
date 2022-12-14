/*
  IMU Classifier_Left_Hand
*/
#include <Arduino_LSM9DS1.h>
#include <TensorFlowLite.h>
#include <tensorflow/lite/micro/all_ops_resolver.h>
#include <tensorflow/lite/micro/micro_error_reporter.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/schema/schema_generated.h>
#include <tensorflow/lite/version.h>

//! import left hand model
#include "model_L.h"

//! array of gestures index
char gestures_idx[4] = {'0', '1', '2', '3'};

const float accelerationThreshold = 2.5; // threshold of significant in G's
const int numSamples = 119;
int samplesRead = numSamples;

// global variables used for TensorFlow Lite (Micro)
tflite::MicroErrorReporter tflErrorReporter;

// pull in all the TFLM ops, you can remove this line and
// only pull in the TFLM ops you need, if would like to reduce
// the compiled size of the sketch.
tflite::AllOpsResolver tflOpsResolver;

const tflite::Model *tflModel = nullptr;
tflite::MicroInterpreter *tflInterpreter = nullptr;
TfLiteTensor *tflInputTensor = nullptr;
TfLiteTensor *tflOutputTensor = nullptr;

// Create a static memory buffer for TFLM, the size may need to
// be adjusted based on the model you are using
constexpr int tensorArenaSize = 8 * 1024;
byte tensorArena[tensorArenaSize] __attribute__((aligned(16)));

//! array to map gesture index to a name
const char *GESTURES[] = {
    "clap",
    "hi",
    "up",
    "down"};

#define NUM_GESTURES (sizeof(GESTURES) / sizeof(GESTURES[0]))

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600); // initialize UART with baud rate of 9600
  while (!Serial)
    ;
  // while (!Serial1);

  // initialize the IMU
  if (!IMU.begin())
  {
    Serial.println("Failed to initialize IMU!");
    while (1)
      ;
  }

  // print out the samples rates of the IMUs
  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");

  Serial.println();

  // get the TFL representation of the model byte array
  tflModel = tflite::GetModel(model);
  if (tflModel->version() != TFLITE_SCHEMA_VERSION)
  {
    Serial.println("Model schema mismatch!");
    while (1)
      ;
  }

  // Create an interpreter to run the model
  tflInterpreter = new tflite::MicroInterpreter(tflModel, tflOpsResolver, tensorArena, tensorArenaSize, &tflErrorReporter);

  // Allocate memory for the model's input and output tensors
  tflInterpreter->AllocateTensors();

  // Get pointers for the model's input and output tensors
  tflInputTensor = tflInterpreter->input(0);
  tflOutputTensor = tflInterpreter->output(0);
}

void loop()
{
  //! where will the index of the gesture with the high 
  //! prediction rate be stored
  int argMax = -1;

  float aX, aY, aZ, gX, gY, gZ;
  // wait for significant motion
  while (samplesRead == numSamples)
  {
    if (IMU.accelerationAvailable())
    {
      // read the acceleration data
      IMU.readAcceleration(aX, aY, aZ);

      // sum up the absolutes
      float aSum = fabs(aX) + fabs(aY) + fabs(aZ);

      // check if it's above the threshold
      if (aSum >= accelerationThreshold)
      {
        // reset the sample read count
        samplesRead = 0;
        break;
      }
    }
  }

  // check if the all the required samples have been read since
  // the last time the significant motion was detected
  while (samplesRead < numSamples)
  {
    // check if new acceleration AND gyroscope data is available
    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable())
    {
      // read the acceleration and gyroscope data
      IMU.readAcceleration(aX, aY, aZ);
      IMU.readGyroscope(gX, gY, gZ);

      // normalize the IMU data between 0 to 1 and store in the model's
      // input tensor
      tflInputTensor->data.f[samplesRead * 6 + 0] = (aX + 4.0) / 8.0;
      tflInputTensor->data.f[samplesRead * 6 + 1] = (aY + 4.0) / 8.0;
      tflInputTensor->data.f[samplesRead * 6 + 2] = (aZ + 4.0) / 8.0;
      tflInputTensor->data.f[samplesRead * 6 + 3] = (gX + 2000.0) / 4000.0;
      tflInputTensor->data.f[samplesRead * 6 + 4] = (gY + 2000.0) / 4000.0;
      tflInputTensor->data.f[samplesRead * 6 + 5] = (gZ + 2000.0) / 4000.0;

      samplesRead++;

      if (samplesRead == numSamples)
      {
        // Run inferencing
        TfLiteStatus invokeStatus = tflInterpreter->Invoke();
        if (invokeStatus != kTfLiteOk)
        {
          Serial.println("Invoke failed!");
          while (1)
            ;
          return;
        }

        float prob_max = 0;
        Serial.println("- The prediction percentage of each gesture: ");
        
        //! displays the prediction percentage of each gesture
        for (int i = 0; i < NUM_GESTURES; i++)
        {
          Serial.print("    ");
          Serial.print(GESTURES[i]);
          Serial.print(": ");
          Serial.println(tflOutputTensor->data.f[i], 6);

          //! get the index of the gesture with the high prediction rate
          //! and put it in argMax
          if (tflOutputTensor->data.f[i] > prob_max)
          {
            argMax = i;
            prob_max = tflOutputTensor->data.f[i];
          }
        }
        Serial.print("--> The current hand gesture is: ");
        Serial.println(GESTURES[argMax]);
        Serial.println("");
      }
    }
  }

  //! send the index of the left gesture to the right arduino
  Serial1.print(gestures_idx[argMax]);
}
