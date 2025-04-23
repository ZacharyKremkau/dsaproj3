# ─── Builder Stage ──────────────────────────────────────────────────────────────
FROM ubuntu:22.04 AS builder

# Install build essentials, Asio headers, git & wget (for CMake installer)
RUN apt-get update && \
    apt-get install -y build-essential libasio-dev git wget && \
    rm -rf /var/lib/apt/lists/*

# Install CMake 3.30 (Ubuntu 22.04 ships 3.22 by default)
RUN wget https://github.com/Kitware/CMake/releases/download/v3.30.2/cmake-3.30.2-linux-x86_64.sh && \
    mkdir /opt/cmake && \
    sh cmake-3.30.2-linux-x86_64.sh --skip-license --prefix=/opt/cmake && \
    ln -s /opt/cmake/bin/cmake /usr/local/bin/cmake && \
    rm cmake-3.30.2-linux-x86_64.sh

WORKDIR /app

# Copy your source & assets
COPY . .

# Configure & build
RUN mkdir build && cd build && \
    cmake .. && \
    make

# ─── Runtime Stage ──────────────────────────────────────────────────────────────
FROM ubuntu:22.04

# Only runtime deps
RUN apt-get update && \
    apt-get install -y libasio-dev && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy the compiled binary and static assets from builder
COPY --from=builder /app/build/MovieWebsite    ./MovieWebsite
COPY --from=builder /app/public                ./public
COPY --from=builder /app/data                  ./data

EXPOSE 18080

CMD ["./MovieWebsite"]
