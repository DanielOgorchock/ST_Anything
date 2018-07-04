#!/usr/bin/env bash
#
# A simple script to automate building BH1750 examples.
#
# Example (MacOSX):
#     $ ARDUINO_IDE_PATH=/Applications/Arduino.app/Contents/Java ./build-examples.bash
#

# Path to script directory.
SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"
LIBNAME="$(basename "$SCRIPTPATH")"

if [[ -z "${ARDUINO_IDE_PATH}" ]]; then
  echo "ARDUINO_IDE_PATH env var is not set"
  exit 1
fi

# Link BH1750 library into Arduino libraries directory
ln -s $SCRIPTPATH $ARDUINO_IDE_PATH/libraries/

cd $ARDUINO_IDE_PATH

for sketch in `find $SCRIPTPATH/examples -name '*.ino'`
do
  echo "Compiling $sketch"
  ./arduino-builder -hardware ./hardware -tools ./hardware/tools/avr -tools ./tools-builder -libraries ./libraries -fqbn arduino:avr:uno $sketch
  # ./arduino-builder -hardware ./hardware -tools ./hardware/tools/avr -tools ./tools-builder -libraries ./libraries -fqbn esp8266:esp8266:generic $sketch
done

# Unlink BH1750 library from Arduino libraries directory
unlink $ARDUINO_IDE_PATH/libraries/$LIBNAME
