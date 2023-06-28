# Darknet Repository

This repository contains the code for Darknet, an open-source neural network framework. Follow the steps below to clone the repository, build Darknet, download the weights, and run the object detector.

## Installation and Benchmarking 

### On Host 

 1. Clone the repository using the following command:

```ini
   git clone https://github.com/Pratham-Bot/darknet.git

```
2. Change your current directory to the cloned repository:

```ini
   cd darknet

```

3. Build Darknet by running the make command:
    ```ini
       make
    ```

4.  Download the weights for the YOLOv3 model using wget:

```ini
   wget https://pjreddie.com/media/files/yolov3.weights
```

5. Run the object detector and benchmark by executing the following command:

 ```ini
     ./darknet detect cfg/yolov3.cfg yolov3.weights data/dog.jpg
  ``` 

6. You will see some output like this:

```ini
layer     filters    size              input                output
    0 conv     32  3 x 3 / 1   416 x 416 x   3   ->   416 x 416 x  32  0.299 BFLOPs
    1 conv     64  3 x 3 / 2   416 x 416 x  32   ->   208 x 208 x  64  1.595 BFLOPs
    .......
  105 conv    255  1 x 1 / 1    52 x  52 x 256   ->    52 x  52 x 255  0.353 BFLOPs
  106 detection
truth_thresh: Using default '1.000000'
Loading weights from yolov3.weights...Done!
data/dog.jpg: Predicted in 0.029329 seconds.
dog: 99%
truck: 93%
bicycle: 99%
```

### On Beaglebone AI-64 

To run the architecture on Beaglebone AI-64, you will need to cross compile the framework for AARCH64 (since, AI-64 is a 64-bit architecture.)

Install the gcc and g++ compilers for aarch64 architecture. Run the following command:
```
sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
```

To cross compile the framework, make the following changes in MAKEFILE:

 ```ini
  ifeq ($(USE_CPP), 1)
  CC=aarch64-linux-gnu-gcc
  else
  CC=aarch64-linux-gnu-gcc
  endif    

 ``` 

And:

```ini
CPP=aarch64-linux-gnu-g++ -std=c++11
```
Now perform ```make``` using the following command:

```
CC=arm-linux-gnueabi-gcc make -j
```
Make sure the ``darknet`` executable is for 64-bit architecture by performing:
```file darknet```.

#### Transfer darkent folder from host to BBAI-64

I used scp protocol to transfer darknet on BBAI64. You can any other method to transfer file.  