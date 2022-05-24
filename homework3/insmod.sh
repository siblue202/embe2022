mv ./../stopwatch_application ./
echo "mv stopwatch_application"
mv ./../stopwatch_driver.ko ./
echo "mv stopwatch_driver.ko"
insmod stopwatch_driver.ko
echo "insmod"
mknod "/dev/stopwatch" c 242 0
echo "mknod"
echo "done"
