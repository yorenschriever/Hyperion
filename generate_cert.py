from os.path import join, isfile
from os import getcwd, mkdir
from shutil import copyfile

Import("env")

PROJECT_DIR = env['PROJECT_DIR']
SRC_DIR = env['PROJECTSRC_DIR']
CERT_DIR = join(env['PROJECT_DIR'],"certificate")

#Generate a certificate and put in it the src folder
#this runs the generate_cert.sh script that comes with the esp_32_https_server library
if not isfile(join(SRC_DIR, "private_key.h")) or not isfile(join(SRC_DIR, "cert.h")):

    #it is likely that the esp_32_https_server library is not installed yet, because this script runs
    #before the script that checks the libraries.
    #so call pio lib install install all libraries
    env.Execute("pio lib install")

    print("Generating certificate. ",CERT_DIR)

    env.Execute(join(PROJECT_DIR,"generate_cert.sh"))