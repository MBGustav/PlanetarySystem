

mkdir -p build/
cd build/
cmake ..
make

echo "-- Build completed --"
echo "available executables:"
ls -l ./ | grep -E '^-..x'