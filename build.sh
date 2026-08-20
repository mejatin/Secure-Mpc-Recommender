#!/usr/bin/env bash
# Builds all C++ binaries for the mpc-protocol backend.
# Safe to run from anywhere -- paths are resolved relative to this script,
# not the current working directory.
set -e

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$ROOT_DIR/mpc-protocol"
CXX_STD="-std=c++17 -O2"

echo "Building run1 (Gen_queries: gen_queries.cpp)..."
g++ $CXX_STD -o "$PROJECT_DIR/Gen_queries/run1" \
    "$PROJECT_DIR/Gen_queries/gen_queries.cpp" \
    "$PROJECT_DIR/Gen_queries/includes/"*.cpp

echo "Building run2 (Gen_queries: dpf_user.cpp)..."
g++ $CXX_STD -o "$PROJECT_DIR/Gen_queries/run2" \
    "$PROJECT_DIR/Gen_queries/dpf_user.cpp" \
    "$PROJECT_DIR/Gen_queries/includes/"*.cpp

echo "Building run3 (Communication: Preprocessing.cpp)..."
g++ $CXX_STD -o "$PROJECT_DIR/Communication/run3" \
    "$PROJECT_DIR/Communication/Preprocessing.cpp" \
    "$PROJECT_DIR/Communication/includes/"*.cpp

echo "Building run4 (Communication: secondpreprocessing.cpp)..."
g++ $CXX_STD -o "$PROJECT_DIR/Communication/run4" \
    "$PROJECT_DIR/Communication/secondpreprocessing.cpp" \
    "$PROJECT_DIR/Communication/includes/"*.cpp

echo "Building run5 (Party 0 server)..."
g++ $CXX_STD -pthread -o "$PROJECT_DIR/files/party0/run5" \
    "$PROJECT_DIR/files/party0/party0.cpp" \
    "$PROJECT_DIR/files/party0/includes/"*.cpp

echo "Building run6 (Party 1 server)..."
g++ $CXX_STD -pthread -o "$PROJECT_DIR/files/party1/run6" \
    "$PROJECT_DIR/files/party1/party1.cpp" \
    "$PROJECT_DIR/files/party1/includes/"*.cpp

echo "Building run7 (Updater)..."
g++ $CXX_STD -o "$PROJECT_DIR/Updater/run7" \
    "$PROJECT_DIR/Updater/FinalUiUpdater.cpp" \
    "$PROJECT_DIR/Updater/includes/"*.cpp

echo "Building main (standalone CLI orchestrator, optional)..."
g++ $CXX_STD -pthread -o "$PROJECT_DIR/Main/main" \
    "$PROJECT_DIR/Main/main.cpp" \
    "$PROJECT_DIR/Main/includes/"*.cpp

echo ""
echo "All binaries built successfully:"
echo "  mpc-protocol/Gen_queries/run1, run2"
echo "  mpc-protocol/Communication/run3, run4"
echo "  mpc-protocol/files/party0/run5"
echo "  mpc-protocol/files/party1/run6"
echo "  mpc-protocol/Updater/run7"
echo "  mpc-protocol/Main/main"
