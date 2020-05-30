#rm -rf Build
mkdir Build
cd Build
cmake ..

os=$(uname)
numCore=$(grep -c ^processor /proc/cpuinfo)

make -j $numCore
