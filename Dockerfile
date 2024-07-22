FROM ubuntu:22.04

# Install yarp prerequisites
RUN apt update && apt install -y wget unzip \
    build-essential git cmake-curses-gui swig \
    libeigen3-dev \
    libace-dev \
    libedit-dev \
    libsqlite3-dev \
    libtinyxml-dev \
    qtbase5-dev qtdeclarative5-dev qtmultimedia5-dev \
    qml-module-qtquick2 qml-module-qtquick-window2 \
    qml-module-qtmultimedia qml-module-qtquick-dialogs \
    qml-module-qtquick-controls qml-module-qt-labs-folderlistmodel \
    qml-module-qt-labs-settings \
    libqcustomplot-dev \
    libgraphviz-dev \
    libjpeg-dev \
    libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-libav \
    python3.10-dev \
    pip \
    curl \
    zip \
    sudo && \
    rm -rf /var/lib/apt/lists/*

RUN useradd -l -G sudo -md /home/user -s /bin/bash -p user user && \
    sed -i.bkp -e 's/%sudo\s\+ALL=(ALL\(:ALL\)\?)\s\+ALL/%sudo ALL=NOPASSWD:ALL/g' /etc/sudoers

USER user
WORKDIR /home/user

# Install ycm
RUN git clone https://github.com/robotology/ycm.git && mkdir robotology && mv ycm/ robotology/ \
    && cd robotology/ycm && mkdir build \
    && cd build && cmake .. && make && sudo make install && \
    rm -rf /ycm

ENV PYTHONPATH=/usr/local/lib/python3/dist-packages

# Install yarp
RUN cd robotology && git clone https://github.com/robotology/yarp.git && \
    cd yarp && git remote add fbrand-new https://github.com/fbrand-new/yarp.git && \
    git fetch fbrand-new feat/gpt_functions && \
    git checkout fbrand-new/feat/gpt_functions && \
    mkdir build && cd build \ 
    && cmake .. \
    && make -j8 && sudo make install && \
    rm -rf /yarp

# Install openai c++ dependencies
RUN git clone https://github.com/Microsoft/vcpkg.git && \
    cd vcpkg && ./bootstrap-vcpkg.sh -disableMetrics && \
    ./vcpkg integrate install && ./vcpkg install curl[tool] && \
    ./vcpkg install nlohmann-json

# Install openai c++ community library
RUN git clone https://github.com/D7EAD/liboai.git && \
    cd liboai/liboai && \
    cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/home/user/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DCMAKE_POSITION_INDEPENDENT_CODE=true && \
    cd build && make -j8 && sudo make install

# Configure openai credentials
RUN echo "set -a" >> /home/user/.bashrc && \
    echo "source ~/.env/config.env" >> /home/user/.bashrc && \
    echo "set +a" >> /home/user/.bashrc
    
# Install yarpGPT
COPY --chown=user:user . /home/user/yarp-devices-llm

RUN cd /home/user/yarp-devices-llm && \
    rm -rf build && \
    cmake -B build -S . \
    -DCMAKE_TOOLCHAIN_FILE=/home/user/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DLIBOAI_INSTALL_PATH=/home/user/liboai && cd build && \ 
    sudo make install -j8

