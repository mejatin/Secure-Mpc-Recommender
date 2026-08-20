# Use a C++ build environment
FROM ubuntu:22.04

# Install required packages
RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    gdb \
    cmake \
    && rm -rf /var/lib/apt/lists/*

# Set base working directory
WORKDIR /app

# Copy all source code and include directories into container
COPY . /app

# ----------------------------
# Compile executables next to their sources
# ----------------------------

# Compile run1 → Gen_queries/gen_queries.cpp
RUN g++ -o ./Gen_queries/run1 Gen_queries/gen_queries.cpp Gen_queries/includes/*.cpp

# Compile run2 → Gen_queries/dpf_user.cpp
RUN g++ -o ./Gen_queries/run2 Gen_queries/dpf_user.cpp Gen_queries/includes/*.cpp

# Compile run3 → Communication/Preprocessing.cpp
RUN g++ -o ./Communication/run3 Communication/Preprocessing.cpp Communication/includes/*.cpp

# Compile run4 → Communication/seconndPreprocessing.cpp
RUN g++ -o ./Communication/run4 Communication/secondpreprocessing.cpp Communication/includes/*.cpp

# Compile run5 → files/party0/party0.cpp
RUN g++ -o ./files/party0/run5 files/party0/party0.cpp files/party0/includes/*.cpp

# Compile run6 → files/party1/party1.cpp
RUN g++ -o ./files/party1/run6 files/party1/party1.cpp files/party1/includes/*.cpp

# Compile run7 → Updater/FinalUiUpdater.cpp
RUN g++ -o ./Updater/run7 Updater/FinalUiUpdater.cpp Updater/includes/*.cpp

# Compile Main/main.cpp → main
RUN g++ -pthread -o ./Main/main Main/main.cpp Main/includes/*.cpp

# Set default working directory
WORKDIR /app/Main
