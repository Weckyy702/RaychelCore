FROM weckyy702/raychel_ci:latest

#Specify which C++ compiler to use. We have both clang and gcc
ARG COMPILER

#Copy source files and set the working directory
COPY . /usr/src/myapp
WORKDIR /usr/src/myapp

#Configure step
ENV CXX=${COMPILER}
RUN cmake -DRAYCHELCORE_BUILD_TESTS=ON .
RUN cmake --build . --target all test

LABEL Name=raychelcore Version=0.0.1