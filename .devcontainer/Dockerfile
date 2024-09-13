# -*- mode: Dockerfile -*-

ARG BASE=ghcr.io/feelpp/feelpp-dev:jammy

FROM ${BASE}
LABEL maintainer Christophe Trophime <christophe.trophime@lncmi.cnrs.fr>

USER root

ARG USERNAME=feelpp
ARG VGLUSER_GID=1002

# Avoid warnings by switching to noninteractive
ENV DEBIAN_FRONTEND=noninteractive

# Setup demo environment variables
ENV LANG=en_US.UTF-8 \
    LANGUAGE=en_US.UTF-8 \
    LC_ALL=C.UTF-8 \
    XLIB_SKIP_ARGB_VISUALS=1

# Set correct timezone
RUN apt-get update \
    && apt-get -y install --no-install-recommends tzdata \
    && echo "Europe/Paris" | tee /etc/timezone \
    && dpkg-reconfigure --frontend noninteractive tzdata

# Configure apt and install packages for dev
RUN apt-get update \
    && apt-get -y install --no-install-recommends apt-utils apt-file dialog 2>&1 \
    && apt-file update \
    # \
    # Verify git, process tools, lsb-release (useful for CLI installs) installed\
    && apt-get -y install git git-lfs iproute2 procps lsb-release wget curl \
    && apt-get -y install emacs-nox vim-nox  \
    && apt-get -y install mesa-utils \
    #\
    # Install C++ tools\
    && apt-get -y install build-essential \
    && apt-get -y install g++ \ 
    #\
    # Install python tools\
    && apt-get -y install python3-minimal libpython3-dev swig \
    && apt-get -y install python-is-python3 python3-pip python3-jinja2 \
    && apt-get -y install python3-autopep8 black yapf3 python3-bandit flake8 pydocstyle pylint python3-pytest mypy

# Install NET runtime
RUN wget https://packages.microsoft.com/config/debian/11/packages-microsoft-prod.deb -O packages-microsoft-prod.deb \
    && dpkg -i packages-microsoft-prod.deb \
    && rm packages-microsoft-prod.deb

# Install pre-reqisites from Lncmi repository
RUN apt-get -y install debian-keyring lsb-release \
    && ln -s /usr/share/keyrings/debian-maintainers.gpg /etc/apt/trusted.gpg.d \
    && echo "deb http://euler.lncmig.local/~christophe.trophime@LNCMIG.local/debian/ $(lsb_release -cs) main" > /etc/apt/sources.list.d/lncmi.list \
    && apt-get update \
    && apt-get -y install magnettools libmagnettools-dev python3-magnettools

# Update to cmake >= 3.21 - specific for bullseye \
# RUN echo "deb http://deb.debian.org/debian bullseye-backports main contrib non-free" > /etc/apt/sources.list \
#     && apt-get update \
#     && apt-get -y install -t bullseye-backports cmake

# Eventually add USERNAME to VGLUSERS group (to be created if not present)
# for dev:
RUN mkdir -p ~${USERNAME}/.ssh/ \
    && ssh-keyscan github.com >> ~${USERNAME}/.ssh/known_hosts \
    && chown -R ${USERNAME}.$USER_GID ~${USERNAME}/.ssh

# Switch back to dialog for any ad-hoc use of apt-get
ENV DEBIAN_FRONTEND=dialog

# Switch to USERNAME
USER ${USERNAME}
WORKDIR /home/${USERNAME}
