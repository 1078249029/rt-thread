FROM ubuntu:22.04

# 1. 基础设置（root用户操作）
RUN useradd -m dev && \
    apt update && \
    apt install -y --no-install-recommends \
        sudo git wget python3 python3-pip scons vim xz-utils && \
    pip3 install esptool && \
    # 允许dev用户免密码sudo（仅用于容器内部）
    echo "dev ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

# 2. 切换到dev用户
USER dev
WORKDIR /home/dev

# 3. 安装工具链（无需sudo，直接以root下载到/opt）
RUN wget -q https://github.com/espressif/crosstool-NG/releases/download/esp-2022r1-RC1/riscv32-esp-elf-gcc11_2_0-esp-2022r1-RC1-linux-amd64.tar.xz && \
    sudo tar -xf riscv32-esp-elf-gcc11_2_0-esp-2022r1-RC1-linux-amd64.tar.xz -C /opt && \
    rm riscv32-esp-elf-*.tar.xz

# 4. 克隆RT-Thread
RUN git clone https://github.com/RT-Thread/rt-thread.git && \
    cd rt-thread && \
    git switch master

# 5. 安装env工具
WORKDIR /home/dev/rt-thread
RUN wget https://raw.githubusercontent.com/RT-Thread/env/master/install_ubuntu.sh && \
    chmod +x install_ubuntu.sh && \
    ./install_ubuntu.sh

# 6. 修改工具链路径
RUN sed -i "s|^.*EXEC_PATH.*|    EXEC_PATH   = r'/opt/riscv32-esp-elf/bin'|" bsp/ESP32_C3/rtconfig.py

# 7. 设置环境变量
ENV PATH="/opt/riscv32-esp-elf/bin:/home/dev/.env/tools/scripts:$PATH"
WORKDIR /home/dev/rt-thread/bsp/ESP32_C3

