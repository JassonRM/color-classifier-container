#!/bin/bash

docker build -t color-classifier .
docker run -it -v /home/jasson/Documents/color-classifier-container/volume:/myapp/output color-classifier
