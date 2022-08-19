# to quit, press ctrl + A + K, then y
#screen $(ls /dev/cu.usbserial*) 115200,onlcr

(sleep 0.5 && stty -f $(ls /dev/cu.usbserial*) 115200) & cat $(ls /dev/cu.usbserial*)