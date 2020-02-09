#!/bin/sh
tar -cvf a.tar product

cat inst.sh a.tar > obs-setup.sh
chmod +x obs-setup.sh

echo "install success!"

exit 0
