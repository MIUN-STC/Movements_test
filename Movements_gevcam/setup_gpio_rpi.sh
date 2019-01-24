#Setup GPIO 21 (PIN 40) of RPI2 to output.
echo 21 > /sys/class/gpio/export
echo "out" > /sys/class/gpio/gpio21/direction
chmod +w /sys/class/gpio/gpio21/value
