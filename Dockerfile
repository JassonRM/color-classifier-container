FROM centos:latest
ENV NAME CONFIG_PATH
RUN yum install gcc -y
COPY . /myapp
WORKDIR /myapp
RUN gcc -o myapp main.c
CMD ["/bin/sh", "./run.sh"]

