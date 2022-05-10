#!/bin/bash

##startupScript for jack and wavemoprhSynth
##this script needs to be in the /home/pi folder

## Stop the ntp service
sudo service ntp stop

## Stop the triggerhappy service
sudo service triggerhappy stop

## Remount /dev/shm to prevent memory allocation errors
sudo mount -o remount,size=128M /dev/shm

## Uncomment if you'd like to disable the network adapter completely
sudo systemctl disable NetworkManager.service

## Set the CPU scaling governor to performance
echo -n performance | sudo tee /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor

##start synth
/home/pi/Desktop/WaveMorphSynth/main
