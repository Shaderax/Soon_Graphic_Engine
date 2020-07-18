#rm -rf Build
mkdir Build
cd Build
cmake -DCMAKE_BUILD_TYPE=Release ..
#cmake -DCMAKE_BUILD_TYPE=Debug ..


os=$(uname)
numCore=$(grep -c ^processor /proc/cpuinfo)

make -j $numCore
