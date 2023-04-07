# to quit, press ctrl + A + K, then y
#screen $(ls /dev/cu.usbserial*) 115200,onlcr

# use ctrl+s to lock, ctrl+q to unlock
(sleep 0.5 && stty -f $(ls /dev/cu.wchusbserial*) 115200) & cat $(ls /dev/cu.wchusbserial*)