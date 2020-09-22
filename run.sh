#!/bin/bash

docker run -t -p $1:8080 -v $2:/myapp/output color-classifier