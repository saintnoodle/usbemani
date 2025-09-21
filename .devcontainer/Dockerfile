FROM saintnoodle/usbemani-mini-5:latest

RUN groupadd -r builder && useradd -m -r -g builder builder

RUN apt-get update && apt-get install -y sudo clangd-19 git gnupg2 bear openssh-client && \
    echo "builder ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/builder && \
    update-alternatives --install /usr/bin/clangd clangd /usr/bin/clangd-19 100

WORKDIR /usbemani

USER builder