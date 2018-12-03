# FROM gcc:8.2
# FROM gcc:7.3
# FROM ubuntu:14.04
FROM ubuntu:16.04
WORKDIR /sources_docker
RUN apt-get update
RUN apt-get install -y build-essential python3 python3-pip zsh curl wget cmake ninja-build
RUN apt-get install -y software-properties-common
RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test
RUN apt-get update
RUN apt-get install -y gcc-7 g++-7
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 80 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 80
RUN apt-get install -y git
RUN wget https://github.com/robbyrussell/oh-my-zsh/raw/master/tools/install.sh -O - | zsh || true

# update cmake
RUN cd / && \
    wget --no-verbose https://github.com/Kitware/CMake/releases/download/v3.13.1/cmake-3.13.1-Linux-x86_64.tar.gz && \
    tar xvf cmake-3.13.1-Linux-x86_64.tar.gz && \
    echo "export PATH=/cmake-3.13.1-Linux-x86_64/bin:$PATH" >> /root/.zshrc

RUN echo "export LC_ALL=C.UTF-8" >> /root/.zshrc && \
    echo "export LANG=C.UTF-8" >> /root/.zshrc && \
    echo "alias python=python3" >> /root/.zshrc


# download cling & add to the PATH
RUN cd / && \
    curl https://root.cern.ch/download/cling/cling_2018-12-03_ubuntu16.tar.bz2 -o cling.tgz
RUN cd / && tar xvf cling.tgz
RUN echo "export PATH=$PATH:/cling_2018-12-03_ubuntu16/bin" >> /root/.zshrc

# install opencv requirements
# ADD install_scripts/install_opencv_requirements.sh /install_opencv_requirements.sh
# RUN /install_opencv_requirements.sh

# ADD install_scripts/build_opencv.sh /build_opencv.sh

# compile opencv
RUN cd / && git clone https://github.com/opencv/opencv.git
RUN cd /opencv && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=RELEASE -GNinja ..
RUN cd /opencv/build && ninja
RUN cd /opencv/build && ninja install

ADD install_scripts/install_x11vnc.sh /install_scripts/install_x11vnc.sh
RUN /install_scripts/install_x11vnc.sh
