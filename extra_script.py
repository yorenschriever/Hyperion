Import("env")

#use custom OTA upload command
env.Replace(
    UPLOADCMD="curl -F \"image=@$SOURCE\" minimini.local:81/update --progress-bar --connect-timeout 10 "
)
