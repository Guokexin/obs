#!/bin/sh
tar -cvf temp.tar bin

cat inst.sh temp.tar > obs-setup.sh
chmod +x obs-setup.sh

echo "install success!"

exit 0
