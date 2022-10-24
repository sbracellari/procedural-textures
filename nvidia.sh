#!/bin/bash
sudo docker run --rm -it --gpus all --user $(id -u):$(id -g) --volume $(pwd):/source --workdir /source nvcr.io/nvidia/nvhpc:22.7-devel-cuda11.7-ubuntu22.04
