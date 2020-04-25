Import("env")


#use custom OTA upload script, with fallback to original (usb) upload
env.Replace(
    UPLOADCMD="curl -F \"image=@$SOURCE\" Hyperion.local:81/update --progress-bar --connect-timeout 10 "
)
