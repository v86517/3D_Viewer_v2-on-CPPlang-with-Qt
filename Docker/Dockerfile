FROM ubuntu:22.04

RUN apt update && apt install -y \
    gcc \
    g++ \
    git \
    tree \
    curl \
    zsh \
    autojump \
    micro \
    valgrind \
    clang-tools \
    clang-format \
    lcov \
    gzip \
    zip \
    unzip \
    doxygen \
    python3 \
    python3-pip \
    python3-venv \
    graphviz \
    make \
    cmake \
    cppcheck \
    flawfinder \
    libgtest-dev \
    libgmock-dev \
    libtbb-dev \
    shellcheck \
    shfmt \
    libxml2-utils

RUN apt-get install -y \
    qtbase5-dev \
    qtbase5-dev-tools \
    qtchooser \
    qt5-qmake \
    libqt5widgets5 \
    libqt5gui5 \
    libqt5core5a \
    qt6-base-dev \
    qt6-base-dev-tools \
    libqt6widgets6 \
    libqt6gui6 \
    libqt6core6

RUN cd /usr/src/googletest && \
    mkdir -p build && \
    cd build && \
    cmake .. && \
    make && \
    cp lib/*.a /usr/lib && \
    cd /usr/src/googletest && \
    rm -rf build

# Создаем пользователь developer
RUN useradd -u 1000 -m -s /bin/zsh developer

# Настраиваем zsh для пользователя developer
USER developer
RUN sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)" "" --unattended && \
    git clone https://github.com/zsh-users/zsh-autosuggestions /home/developer/.oh-my-zsh/custom/plugins/zsh-autosuggestions && \
    git clone https://github.com/zsh-users/zsh-syntax-highlighting.git /home/developer/.oh-my-zsh/custom/plugins/zsh-syntax-highlighting && \
    sed -i 's/plugins=(git)/plugins=(git zsh-autosuggestions zsh-syntax-highlighting autojump)/g' /home/developer/.zshrc

# Создаем Python venv для пользователя developer
RUN python3 -m venv /home/developer/venv && \
    /home/developer/venv/bin/pip install markdown lizard jinja2 cssbeautifier

WORKDIR /project

CMD ["/usr/bin/zsh"]
