# Darknet 

This repository contains the code for Darknet, an open-source neural network framework. Follow the steps below to clone the repository, build Darknet, download the weights, and run the object detector.

## Installation and Benchmarking 

### On Host 

1. Clone the [darknet](https://github.com/pjreddie/darknet) repository

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

#### 1. Modify the Makefile on Your Laptop

On your laptop, navigate to the Darknet source code directory. Open the `Makefile` and locate the following lines:

```make
CC=gcc
CPP=g++
```
Replace them with 

```bash
CC=aarch64-linux-gnu-gcc
CPP=aarch64-linux-gnu-gcc
```
These changes configure Darknet to use the aarch64-linux-gnu-gcc cross-compiler for compilation.

#### 2. Cross-Compile Darknet

With the Makefile configured for cross-compilation, you can now compile Darknet on BeagleBoard AI64:

```bash
CC=arm-linux-gnueabi-gcc make -j
```

#### 3. Transfer Compiled Files to BeagleBoard AI64

To transfer the compiled Darknet files from your laptop to BeagleBoard AI64, use the SCP (Secure Copy Protocol). Run the following command on your laptop: 
 
 ```bash
 scp /path/to/compiled/darknet beagle@beagleboard-ai64:/destination/on/beagleboard
```
In my case, the command is

```bash 
scp -r ./darknet debian@beaglebone.local:/home/debian
```
#### 4. Run Darknet on BeagleBoard AI64

You can now execute Darknet on your BeagleBoard AI64:

```bash
./darknet detect cfg/yolov3.cfg yolov3.weights data/dog.jpg
```