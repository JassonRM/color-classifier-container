FROM centos:latest
RUN yum install gcc -y
COPY /src /myapp
WORKDIR /myapp
RUN gcc -o app main.c
CMD ["./app"]