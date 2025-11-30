FROM ubuntu:22.04

# Set environment variables to avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    python3 \
    nodejs \
    npm \
    wget \
    && rm -rf /var/lib/apt/lists/*

# Install Emscripten
WORKDIR /opt
RUN git clone https://github.com/emscripten-core/emsdk.git
WORKDIR /opt/emsdk
RUN ./emsdk install latest
RUN ./emsdk activate latest

# Set up working directory
WORKDIR /workspace
RUN git clone https://github.com/AMR1234567891011/ashersfunctionlib.git

EXPOSE 8000
# Set the entry point to interactive bash with Emscripten environment
CMD ["/bin/bash", "-c", "source /opt/emsdk/emsdk_env.sh && /bin/bash"]
#docker run -it --rm -p 8000:8000 -v $(pwd):/workspace crypto-dev