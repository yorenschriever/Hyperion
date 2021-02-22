#!/bin/bash
set -e

mkdir -p certificate
cd certificate
#------------------------------------------------------------------------------
# cleanup any previously created files
rm -f hyperionca.* hyperion.* cert.h private_key.h

#------------------------------------------------------------------------------
# create a CA called "myca"

# create a private key
openssl genrsa -out hyperionca.key 1024

# create certificate
cat > hyperionca.conf << EOF  
[ req ]
distinguished_name     = req_distinguished_name
prompt                 = no
[ req_distinguished_name ]
C = DE
ST = BE
L = Berlin
O = MyCompany
CN = hyperionca.local
EOF
openssl req -new -x509 -days 3650 -key hyperionca.key -out hyperionca.crt -config hyperionca.conf
# create serial number file
echo "01" > hyperionca.srl

#------------------------------------------------------------------------------
# create a certificate for the ESP (hostname: "myesp")

# create a private key
openssl genrsa -out hyperion.key 1024
# create certificate signing request
cat > hyperion.conf << EOF  
[ req ]
distinguished_name     = req_distinguished_name
prompt                 = no
[ req_distinguished_name ]
C = DE
ST = BE
L = Berlin
O = Hyperion
CN = hyperion.local
EOF
openssl req -new -key hyperion.key -out hyperion.csr -config hyperion.conf

# have myca sign the certificate
openssl x509 -days 3650 -CA hyperionca.crt -CAkey hyperionca.key -in hyperion.csr -req -out hyperion.crt

# verify
openssl verify -CAfile hyperionca.crt hyperion.crt

# convert private key and certificate into DER format
openssl rsa -in hyperion.key -outform DER -out hyperion.key.DER
openssl x509 -in hyperion.crt -outform DER -out hyperion.crt.DER

# create header files
echo "#ifndef CERT_H_" > ./cert.h
echo "#define CERT_H_" >> ./cert.h
xxd -i hyperion.crt.DER >> ./cert.h
echo "#endif" >> ./cert.h

echo "#ifndef PRIVATE_KEY_H_" > ./private_key.h
echo "#define PRIVATE_KEY_H_" >> ./private_key.h
xxd -i hyperion.key.DER >> ./private_key.h
echo "#endif" >> ./private_key.h

# # Copy files to every example
# for D in ../examples/*; do
#   if [ -d "${D}" ] && [ -f "${D}/$(basename $D).ino" ]; then
#     echo "Adding certificate to example $(basename $D)"
#     cp ./cert.h ./private_key.h "${D}/"
#   fi
# done

cp ./cert.h ./private_key.h "../src/"


echo ""
echo "Certificates created!"
echo "---------------------"
echo ""
echo "  Private key:      private_key.h"
echo "  Certificate data: cert.h"
echo ""
echo "Make sure to have both files available for inclusion when running the examples."
echo "The files have been copied to all example directories, so if you open an example"
echo " sketch, you should be fine."