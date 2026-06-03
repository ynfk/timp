FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libssl-dev \
    qt6-base-dev \
    libqt6core6 \
    libqt6network6 \
    libqt6sql6 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN rm -rf build && mkdir build && cd build && \
    cmake .. -DCMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/cmake/Qt6 && \
    make -j4

EXPOSE 33333

CMD ["./build/server"]