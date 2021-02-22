Import("env")

#use custom OTA upload command
env.Replace(
    UPLOADCMD="curl -F \"image=@$SOURCE\" Hyperion.local:81/update --progress-bar --connect-timeout 10 "
)
