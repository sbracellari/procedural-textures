# Procedural Texture Generation in C

## Prerequisites
Before this code is used to generate procedural images, the following requirements must be met when running the code using GCC or NVC respectively.

### GCC
- GCC
- Python
    - jupyter
    - numpy
    - matplotlib

### NVC
In order to run this project using NVC, a computer with a recent NVIDIA graphics card running Ubuntu is required. Please ensure you have enough space in your root directory for the container to be downloaded. 
- Docker
    - https://docs.docker.com/engine/install/ubuntu/
- Nvidia Graphics Drivers
- Nvidia Container Toolkit
    - https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/install-guide.html
- Python
    - jupyter
    - numpy
    - matplotlib


## Setup and execution

1. Make sure you have the GCC/NVC prerequisites installed and set up on your machine
2. Pull this repository onto your machine
    - If you are planning on running using NVC, start the container with the following code: `sudo docker run --rm -it --gpus all --user $(id -u):$(id -g) --volume $(pwd):/source --workdir /source nvcr.io/nvidia/nvhpc:22.7-devel-cuda11.7-ubuntu22.04`
3. Navigate to the repository's directory and call the 'make' command in your terminal
    - GCC: `make`
    - NVC: `make CC=nvc`
4. Run cells by calling './cells'. The output in the terminal should be the time it took to produce each image. 
5. Navigate to and open graphs.ipynb
6. Run the first cell to initialize the images.
7. Run the second cell to iterate through the generated images and display them in the output. 
