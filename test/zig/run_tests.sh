#!/usr/bin/env bash

function reset_build {
    for DIR in "_installed"
    do
        if [ -d "$SCRIPT_DIR/$DIR" ]; then
            rm -rf "$SCRIPT_DIR/$DIR"
        fi
        mkdir -p "$SCRIPT_DIR/$DIR"
    done
}

function run_volk_test {
    for FILE in "./zig-out/bin/volk_source_direct_test" "./zig-out/bin/volk_source_direct_test.exe" "./zig-out/bin/volk_static_lib_test" "./zig-out/bin/volk_static_lib_test.exe" "./zig-out/bin/volk_module_test" "./zig-out/bin/volk_module_test.exe" "./zig-out/bin/volk_installed_test" "./zig-out/bin/volk_installed_test.exe"
    do
        if [ -f $FILE ]; then
            echo "Running test: $FILE"
            $FILE
            RC=$?
            echo "Test return code: $RC"
            if [ $RC -ne 0 ]; then
                exit $RC
            fi
            break
        fi
    done
}

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
pushd $SCRIPT_DIR/../.. > /dev/null

reset_build

zig build -Dinstall=true --prefix "$SCRIPT_DIR/_installed"

if [ -z "$VULKAN_SDK" ]; then
    echo "VULKAN_SDK environment variable not set, some tests may fail"
    VULKAN_HEADERS_OPT=""
else
    VULKAN_HEADERS_OPT="-Dvulkan_headers_dir="$VULKAN_SDK/Include""
fi

echo
echo "zig_source_directly =======================================>"
echo 

pushd test/zig/source_directly > /dev/null
zig build $VULKAN_HEADERS_OPT
run_volk_test
popd > /dev/null

echo
echo "zig_static_lib =======================================>"
echo 

pushd test/zig/static_lib > /dev/null
zig build $VULKAN_HEADERS_OPT
run_volk_test
popd > /dev/null

echo
echo "zig_module =======================================>"
echo 

pushd test/zig/module > /dev/null
zig build $VULKAN_HEADERS_OPT
run_volk_test
popd > /dev/null

echo
echo "zig_installed =======================================>"
echo 

pushd test/zig/installed > /dev/null
zig build -Dvolk-path="$SCRIPT_DIR/_installed/include" $VULKAN_HEADERS_OPT
run_volk_test
popd > /dev/null

popd > /dev/null 