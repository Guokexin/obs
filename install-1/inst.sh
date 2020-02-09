#!/bin/sh
## protocol
more <<"EOF"
	License Agreement
	......
EOF

##
agreed=
while [ x$agreed = x ]
do
	echo "Do you agree to the above license terms?[ yes or no]"
	read reply leftover
	case $reply in
		y* | Y*)
			agreed=1
			;;
		n* | N*)
			echo "you cann't install this software"
			exit 1
			;;
	esac
done	
echo "Unpacking..."
tail -n +49 $0 > data.tar

tar xvf data.tar > /dev/null
if [ $? != 0 ]
then
	echo "There is error when unpacking files"
	rm -rf data.tar product
	exit 1
fi

echo "Done."
##
rm -f data.tar

##
cd product/work

sh product-inst.sh

cd ../..

##
rm -rf product

exit 0
