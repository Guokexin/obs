#!/bin/sh

##
echo "set installing path:"

INST_PATH=/opt/xjobs


##
#cat ../bin/start.tmpl | sed "s/@INSTALLPATH@/'${INST_PATH}'/" > ../bin/start.sh
chmod +x ../bin/start.sh

#
mkdir -p "${INST_PATH}"
mkdir -p "${INST_PATH}/bin"
cp -R ../lib "${INST_PATH}"
cp ../bin/start.sh "${INST_PATH}/bin"
${INST_PATH}/bin/start.sh

