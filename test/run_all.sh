#!/usr/bin/env bash

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
ROOT_DIR="$( cd "$SCRIPT_DIR/.." >/dev/null 2>&1 && pwd )"

function run_test_suite {
    local suite=$1
    local script=$2
    
    echo
    echo "======================================================"
    echo "$suite"
    echo "======================================================"
    echo
    
    if [ ! -x "$script" ]; then
        chmod +x "$script"
    fi
    
    "$script"
    local rc=$?
    
    if [ $rc -ne 0 ]; then
        echo "$suite tests FAIL with exit code $rc"
        return $rc
    fi
    
    echo "$suite OK"
    return 0
}

pushd "$ROOT_DIR" > /dev/null

# run cmake tests
run_test_suite "cmake" "$SCRIPT_DIR/cmake/run_tests.sh"
cmake_rc=$?

# run zig tests
run_test_suite "zig" "$SCRIPT_DIR/zig/run_tests.sh"
zig_rc=$?

popd > /dev/null

if [ $cmake_rc -ne 0 ] || [ $zig_rc -ne 0 ]; then
    echo
    echo "======================================================"
    echo "FAIL"
    echo "cmake tests: $([ $cmake_rc -eq 0 ] && echo "PASS" || echo "FAIL")"
    echo "zig tests: $([ $zig_rc -eq 0 ] && echo "PASS" || echo "FAIL")"
    echo "======================================================"
    exit 1
fi

echo
echo "======================================================"
echo "OK"
echo "======================================================"

exit 0
