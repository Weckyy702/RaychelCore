FROM rikorose/gcc-cmake:latest

#Copy source files and set the working directory
COPY . /usr/src/myapp
WORKDIR /usr/src/myapp

#Configure step
RUN cmake .
RUN cmake --build . --target all test

LABEL Name=raychelcore Version=0.0.1
