FROM ubuntu:22.04

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive
ENV LANG=en_US.UTF-8
ENV ROS_DISTRO=humble

# Install basic utilities and configure locale
RUN apt-get update && apt-get install -y --no-install-recommends \
    locales \
    curl \
    gnupg2 \
    lsb-release \
    sudo \
    software-properties-common \
    wget \
    udev \
    && locale-gen en_US.UTF-8 \
    && update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8 \
    && rm -rf /var/lib/apt/lists/*

# Install ROS 2 Humble
RUN apt-get update && apt-get install -y --no-install-recommends \
    software-properties-common \
    && add-apt-repository universe \
    && curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg \
    && echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(. /etc/os-release && echo $UBUNTU_CODENAME) main" | tee /etc/apt/sources.list.d/ros2.list > /dev/null \
    && apt-get update && apt-get install -y --no-install-recommends \
    ros-humble-ros-base \
    python3-argcomplete \
    ros-dev-tools \
    && rm -rf /var/lib/apt/lists/*

# Install computer vision dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    libopencv-dev \
    python3-opencv \
    libpcl-dev \
    pcl-tools \
    libboost-all-dev \
    libeigen3-dev \
    libflann-dev \
    && rm -rf /var/lib/apt/lists/*

# Install eCAL
RUN apt-get update && apt-get install -y --no-install-recommends \
    apt-transport-https \
    ca-certificates \
    && add-apt-repository ppa:ecal/ecal-latest \
    && apt-get update && apt-get install -y --no-install-recommends \
    ecal \
    cmake \
    g++ \
    libprotobuf-dev \
    protobuf-compiler \
    && rm -rf /var/lib/apt/lists/*

# Configure eCAL
RUN awk -F"=" '/^network_enabled/{$2="= true"}1' /etc/ecal/ecal.ini > /etc/ecal/ecal.tmp \
    && rm /etc/ecal/ecal.ini \
    && mv /etc/ecal/ecal.tmp /etc/ecal/ecal.ini

# Install XIMEA SDK dependencies and setup
RUN apt-get update && apt-get install -y --no-install-recommends \
    libusb-1.0-0-dev \
    libraw1394-dev \
    gstreamer1.0-plugins-base \
    gstreamer1.0-tools \
    && mkdir -p /etc/udev/rules.d \
    && rm -rf /var/lib/apt/lists/*

# Copy and install XIMEA SDK
COPY ./XIMEA_SDK_Linux /tmp/ximea_sdk
RUN if [ -d "/tmp/ximea_sdk" ]; then \
    # Install main library and create proper symlinks \
    find /tmp/ximea_sdk/api/X64 -name "libm3api.so*" -exec cp -v {} /usr/lib/ \; && \
    cd /usr/lib && \
    # make sure libm3api.so points at the .so.2 that we just copied
    ln -sf libm3api.so.2 libm3api.so && \
    # (optional) if any code expects .so.1
    ln -sf libm3api.so.2 libm3api.so.1 && \
    # Install all the other SDK .so’s and headers
    find /tmp/ximea_sdk/api -name "*.so*" -not -name "libm3api.so*" -exec cp -v {} /usr/lib/ \; && \
    mkdir -p /usr/include/ximea && \
    find /tmp/ximea_sdk/include -name "*.h" -exec cp -v {} /usr/include/ximea/ \; && \
    ln -s /usr/include/ximea /usr/include/m3api && \
    ldconfig; \
    fi && \
    rm -rf /tmp/ximea_sdk


# Verification step
RUN ls -l /usr/lib/libm3api* && \
    ls -l /usr/include/ximea/

# Create non-root user
ARG USERNAME=rosuser
ARG USER_UID=1000
ARG USER_GID=$USER_UID
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd -s /bin/bash --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && usermod -aG plugdev $USERNAME \
    && echo "$USERNAME ALL=(root) NOPASSWD:ALL" > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

# Set up environment
USER $USERNAME
WORKDIR /home/$USERNAME

# Copy project files
COPY --chown=$USERNAME:$USERNAME m2s2_ecal_deserializers-main ./m2s2_ecal_deserializers


# Build project with verification
WORKDIR /home/$USERNAME/m2s2_ecal_deserializers
RUN mkdir -p build && cd build && \
    echo "=== Verifying XIMEA installation ===" && \
    ls -l /usr/lib/libm3api* && ls -l /usr/include/ximea/ && \
    echo "=== Building project ===" && \
    cmake \
      -DM3API_LIBRARY=/usr/lib/libm3api.so \
      -DM3API_INCLUDE_DIR=/usr/include/ximea \
      .. && \
    make


# Set environment variables
# ENV LD_LIBRARY_PATH="/usr/lib:/home/$USERNAME/m2s2_ecal_deserializers/build:$LD_LIBRARY_PATH"
ENV XIMEA_INCLUDE_DIR="/usr/include/ximea"
ENV PATH="/opt/ros/humble/bin:$PATH"
ENV AMENT_PREFIX_PATH="/opt/ros/humble"
ENV ROS_AUTOMATIC_DISCOVERY_RANGE="SUBNET"

CMD ["bash"]