FROM centos:latest
RUN yum install gcc -y
RUN yum install -y libpng-devel
COPY /src /myapp
WORKDIR /myapp
RUN gcc -o app main.c image.c -lpng
CMD ["./app"]