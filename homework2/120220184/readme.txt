driver name : hw_driver.ko
driver file name : /dev/dev_driver
major number : 242

execution flow 
1. make module and execution file in each folder(app, module)[host]
> make

return hw_driver.ko(device module) & hw_driver_test(execution file)
    
2. transfer files to target board [host]
> adb push ./module/hw_driver.ko /data/local/tmp/
> adb push ./app/hw_driver_test /data/local/tmp

3. insert module in kernel [target board]
> insmod hw_driver.ko

4. make device file  [target board]
> mknod /dev/dev_driver c 242 0

5. test application [target board]
> hw_driver_test [INTERVAL] [CNT] [TIMER_INIT]


If you want to remove device module
1. check module is inserted [target board]
> lsmod 

2. remove device moduel [target board]
> rmmod hw_driver

3. remove device file [target board]
> rm /dev/dev_driver