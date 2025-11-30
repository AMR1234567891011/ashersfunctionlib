FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    python3 \
    python3-pip \
    nodejs \
    npm \
    wget \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /opt
RUN git clone https://github.com/emscripten-core/emsdk.git
WORKDIR /opt/emsdk
RUN ./emsdk install latest
RUN ./emsdk activate latest


WORKDIR /workspace
ARG GIT_CACHE_BUST=1
RUN git clone https://github.com/AMR1234567891011/ashersfunctionlib.git .
RUN git checkout test
RUN python3 -m pip install flask flask-cors
# RUN emcc -O3 -s WASM=1 -s EXPORTED_FUNCTIONS="[_x3dh_woR,_x3dh_woS, _main_tests,_wasm_init_sessions,_wasm_new_session,_wasm_send_message,_wasm_receive_message,_malloc,_free]" -s EXPORTED_RUNTIME_METHODS="[ccall,cwrap]" -o crypto_wasm.js ../main_test.c ../sha256.c ../X25519.c ../X3DH.c ../session_manager.c ../session_test.c ../wasm_interface.c
# RUN gcc -shared -fPIC -o libcrypto.so ../session_manager.c ../sha256.c ../X25519.c ../X3DH.c -I
EXPOSE 8000
CMD ["/bin/bash", "-c", "source /opt/emsdk/emsdk_env.sh && /bin/bash"]
#docker build . -t signal_test
#docker run -it --rm -p 8000:8000 signal_test
