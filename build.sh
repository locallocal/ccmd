#/bin/bash
function build_ccmd {
    if [ -d ./build ];then
	echo "remove old build directory."
        rm -rf ./build
    fi

    cmake_command="$CMAKE_COMMAND -B build"
    ${cmake_command}
    build_command="cmake --build build"
    ${build_command}
    if [ $? -eq 0 ];then
	echo "build sucessfully"
    else
	echo "build failed"
    fi
}

function run_test {
    if [ ! -f "./build/bin/ccmd-test" ];then
	echo "please run build with --debug firstly."
	exit 1
    fi
    ./build/bin/ccmd-test
}

function run_coverage {
    type lcov > /dev/null 2>&1
    if [ $? -ne 0 ];then
        echo "lcov command not found, please install it."
	exit 1
    fi

    if [ ! -f "./build/CMakeFiles/ccmd.dir/src/ccmd.cc.gcda" ];then
        echo "please build with debug and runnint unit test firstly."
	exit 1
    fi
    lcov -c -o ccmd.info -d ./build/CMakeFiles/ccmd.dir/src
    genhtml ccmd.info -o cov 
    rm -rf ccmd.info
}

function clean {
    echo "remove \"./build\" directory."
    rm -rf ./build
    echo "remove \"./cov\" directory."
    rm -rf ./cov
}

BUILD_TYPE="--release"
if [ $# -eq 1 ];then
    BUILD_TYPE=$1
fi

if [ "${BUILD_TYPE}" == "--help" ];then
    echo "Usage: build.sh [options]"
    echo ""
    echo " --debug    build with debug options."
    echo " --test     run unit tests after debug build."
    echo " --cov      run source code coverage test after runing tests."
    echo " --clean    remove build and cov directory."
    exit 0
fi

type cmake > /dev/null 2>&1
if [ $? -ne 0 ];then
    echo "cmake command not found. please install it."
    exit 1
fi

CMAKE_COMMAND="cmake"
if [ "${BUILD_TYPE}" == "--debug" ];then
    CMAKE_COMMAND="${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=debug"
    
    if [ !$(type lcov) ];then
        echo "lcov command not found. please install it."
	exit 1
    fi
fi

case "${BUILD_TYPE}" in
    --release)
	echo "start to build ccmd release."
	build_ccmd "${CMAKE_COMMAND}"
	;;
    --debug)
        echo "start to build ccmd debug."
	build_ccmd "${CMAKE_COMMAND}"
	;;
    --test)
	echo "start to run unit tests."
	run_test
	;;
    --cov)
	echo "start to run source coverage test."
	run_coverage
	;;
    --clean)
	echo "start to clean."
	clean
	;;
    *)
        echo "invalid options."
	exit 1
	;;
esac


