driver name : stopwatch_driver.ko
driver file name : /dev/stopwatch
major number : 242

execution flow 
1. make module and execution file in each folder(app, module)[host]
> make

return stopwatch_driver.ko(device module) & stopwatch_application(execution file)
    
2. transfer files to target board [host]
> adb push ./module/stopwatch_driver.ko /data/local/tmp/
> adb push ./app/stopwatch_application /data/local/tmp

3. insert module in kernel [target board]
> insmod stopwatch_driver.ko

4. make device file  [target board]
> mknod /dev/stopwatch c 242 0

5. test application [target board]
> ./stopwatch_application

If you want to remove device module
1. check module is inserted [target board]
> lsmod 

2. remove device moduel [target board]
> rmmod stopwatch_driver

3. remove device file [target board]
> rm /dev/stopwatch