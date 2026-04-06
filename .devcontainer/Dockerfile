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
    linux-headers-generic \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN apt-get update && apt-get install -y \
    clang-18 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
    && ln -s /usr/bin/clang-18 /usr/local/bin/clang

ARG USERNAME=developer
ARG USER_UID=1000
ARG USER_GID=1000

RUN set -eux; \
    uid="$USER_UID"; \
    while getent passwd "$uid" > /dev/null; do uid=$((uid + 1)); done; \
    gid="$USER_GID"; \
    while getent group "$gid" > /dev/null; do gid=$((gid + 1)); done; \
    groupadd --gid "$gid" "$USERNAME"; \
    useradd --uid "$uid" --gid "$gid" -m "$USERNAME"; \
    echo "$USERNAME ALL=(root) NOPASSWD:ALL" > "/etc/sudoers.d/$USERNAME"; \
    chmod 0440 "/etc/sudoers.d/$USERNAME"

USER $USERNAME

WORKDIR /workspaces

CMD ["/bin/bash"]
