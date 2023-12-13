#!/bin/bash

target="192.168.0.158"
gateway="192.168.0.1"

ping -c 1 -W 1 $gateway
if [ $? -eq 0 ]; then
  ping -c 30 -W 1 $target
  if [ $? -ne 0 ]; then
    poweroff
  fi
fi