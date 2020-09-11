#!/bin/bash

docker build -t color-classifier .
docker run -it -p 8080:8080 -v /home/jasson/Documents/color-classifier-container/volume:/myapp/output color-classifier
