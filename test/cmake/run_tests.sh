#!/usr/bin/env bash

function reset_build {
    for DIR in "_build" "_installed"
    do
        if [ -d "$SCRIPT_DIR/$DIR" ]; then
            rm -rf "$SCRIPT_DIR/$DIR"
        fi
        mkdir -p "$SCRIPT_DIR/$DIR"
    done
}

function run_volk_test {
    for FILE in "./volk_test" "./volk_test.exe" "Debug/volk_test.exe" "Release/volk_test.exe"
    do
        if [ -f $FILE ]; then
            echo "Running test:"
            $FILE
            RC=$?
            break
        fi
    done
    echo "volk_test return code: $RC"
}

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
ROOT_DIR="$( cd "$SCRIPT_DIR/../.." >/dev/null 2>&1 && pwd )"
pushd $ROOT_DIR > /dev/null

reset_build

mkdir -p "$SCRIPT_DIR/_build"
pushd "$SCRIPT_DIR/_build" > /dev/null
cmake -DCMAKE_INSTALL_PREFIX="$SCRIPT_DIR/_installed" -DVOLK_INSTALL=ON "$ROOT_DIR" || exit 1
cmake --build . --target install || exit 1
popd > /dev/null

echo
echo "cmake_using_source_directly =======================================>"
echo 

pushd "$SCRIPT_DIR/source_directly" > /dev/null
mkdir -p _build
pushd _build > /dev/null
cmake .. || exit 1
cmake --build . || exit 1
run_volk_test
popd > /dev/null
popd > /dev/null

echo
echo "cmake_using_subdir_static =======================================>"
echo 

pushd "$SCRIPT_DIR/subdir_static" > /dev/null
mkdir -p _build
pushd _build > /dev/null
cmake .. || exit 1
cmake --build . || exit 1
run_volk_test
popd > /dev/null
popd > /dev/null

echo
echo "cmake_using_subdir_headers =======================================>"
echo 

pushd "$SCRIPT_DIR/subdir_headers" > /dev/null
mkdir -p _build
pushd _build > /dev/null
cmake .. || exit 1
cmake --build . || exit 1
run_volk_test
popd > /dev/null
popd > /dev/null

echo
echo "cmake_using_installed_headers =======================================>"
echo 

pushd "$SCRIPT_DIR/installed_headers" > /dev/null
mkdir -p _build
pushd _build > /dev/null
cmake -DCMAKE_PREFIX_PATH="$SCRIPT_DIR/_installed" .. || exit 1
cmake --build . || exit 1
run_volk_test
popd > /dev/null
popd > /dev/null

popd > /dev/null

