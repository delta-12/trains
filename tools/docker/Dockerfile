FROM build-tools-ubuntu:latest

WORKDIR /
RUN apt update

# Install Rust
RUN apt install -y curl
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs -o rustup
RUN chmod +x rustup
RUN ./rustup -y
RUN rm rustup

# Install Slint
RUN apt install -y libxcb-shape0-dev libxcb-xfixes0-dev libxkbcommon-dev libfontconfig-dev qtbase5-dev qt5-qmake
RUN git clone https://github.com/slint-ui/slint.git
WORKDIR /slint
RUN cmake -DSLINT_FEATURE_RENDERER_SOFTWARE=ON -B build -G Ninja
RUN cmake --build build -j$(nproc)
RUN cmake --build build -t install -j$(nproc)
RUN /root/.cargo/bin/cargo install slint-viewer

# Cleanup
WORKDIR /root
RUN apt clean
RUN rm -rf /tmp/* /var/tmp/* /var/lib/apt/lists/*
