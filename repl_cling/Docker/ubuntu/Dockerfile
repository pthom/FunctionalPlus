FROM ubuntu:16.04
WORKDIR /sources_docker
RUN apt-get update
RUN apt-get install -y build-essential python3 python3-pip zsh curl wget cmake ninja-build git

# install gcc-7
RUN apt-get install -y software-properties-common
RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test
RUN apt-get update
RUN apt-get install -y gcc-7 g++-7
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 80 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 80

# install zsh & oh-my-zsh (so that we have nice colors)
RUN wget https://github.com/robbyrussell/oh-my-zsh/raw/master/tools/install.sh -O - | zsh || true

# update cmake
RUN cd / && \
    wget --no-verbose https://github.com/Kitware/CMake/releases/download/v3.13.1/cmake-3.13.1-Linux-x86_64.tar.gz && \
    tar xvf cmake-3.13.1-Linux-x86_64.tar.gz && \
    echo "export PATH=/cmake-3.13.1-Linux-x86_64/bin:$PATH" >> /root/.zshrc

RUN echo "export LC_ALL=C.UTF-8" >> /root/.zshrc && \
    echo "export LANG=C.UTF-8" >> /root/.zshrc && \
    echo "alias python=python3" >> /root/.zshrc

# install virtual x11 server
ADD install_scripts/install_x11vnc.sh /install_scripts/install_x11vnc.sh
RUN /install_scripts/install_x11vnc.sh
RUN apt-get install -y libgtk2.0-dev
RUN echo "export DISPLAY=:0" >> /root/.zshrc

# download cling & add to the PATH
RUN cd / && \
    curl https://root.cern.ch/download/cling/cling_2018-12-03_ubuntu16.tar.bz2 -o cling.tgz
RUN cd / && tar xvf cling.tgz
RUN echo "export PATH=$PATH:/cling_2018-12-03_ubuntu16/bin" >> /root/.zshrc

# get opencv source
RUN cd / && git clone https://github.com/opencv/opencv.git
# build opencv
RUN cd /opencv && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=RELEASE -GNinja .. \
    -DBUILD_JAVA=OFF \
    -DBUILD_PROTOBUF=OFF \
    -DBUILD_PERF_TESTS=OFF \
    -DBUILD_TEST=OFF \
    -DBUILD_opencv_apps=OFF \
    -DBUILD_opencv_java_bindings_generator=OFF \
    -DBUILD_opencv_python_bindings_generator=OFF \
    -DBUILD_opencv_gapi=OFF\
    -DWITH_GTK=ON

RUN cd /opencv/build && ninja && ninja install
