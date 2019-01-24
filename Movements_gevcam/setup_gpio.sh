#This script can not be run
#Run each command seperatly
sudo -s
echo 255 > /sys/class/gpio/export
echo "out" > /sys/class/gpio/gpio255/direction
chmod +w /sys/class/gpio/gpio255/value
exit
