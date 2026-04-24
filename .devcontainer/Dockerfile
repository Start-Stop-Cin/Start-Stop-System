FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    make \
    git \
    curl \
    wget \
    gnupg \
    lsb-release \
    software-properties-common \
    python3 \
    sudo \
    cppcheck \
    jq \
    linux-headers-generic \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN apt-get update && apt-get install -y \
    clang-18 \
    clang-format \
    llvm-18 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
    && ln -s /usr/bin/clang-18 /usr/local/bin/clang \
    && ln -s /usr/bin/llvm-cov-18 /usr/local/bin/llvm-cov \
    && ln -s /usr/bin/llvm-profdata-18 /usr/local/bin/llvm-profdata

RUN wget -qO /tmp/mull.deb \
        https://github.com/mull-project/mull/releases/download/0.33.0/Mull-18-0.33.0-LLVM-18.1.3-ubuntu-amd64-24.04.deb \
    && apt-get install -y /tmp/mull.deb \
    && rm /tmp/mull.deb

# ==============================================================================
# HMI CLUSTER DEPENDENCIES (Qt6 / X11 / OpenGL)
# Instala as bibliotecas gráficas necessárias para rodar o AppImage no Dev Container
# ==============================================================================
RUN apt-get update && apt-get install -y --no-install-recommends \
        libegl1 \
        libgl1 \
        libopengl0 \
        libharfbuzz0b \
        libfontconfig1 \
        libfreetype6 \
        libxcb-cursor0 \
        libxkbcommon-x11-0 \
        libxcb-xinerama0 \
        libxcb-icccm4 \
        libxcb-image0 \
        libxcb-keysyms1 \
        libxcb-render-util0 \
        libxcb-xkb1 \
        libx11-xcb1 \
        libxcb-shape0 \
        libxcb-randr0 \
        libxcb-sync1 \
        libsm6 \
        libice6 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN echo "ubuntu ALL=(root) NOPASSWD:ALL" > /etc/sudoers.d/ubuntu && \
    chmod 0440 /etc/sudoers.d/ubuntu

USER ubuntu

WORKDIR /workspaces

CMD ["/bin/bash"]