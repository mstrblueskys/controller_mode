#!/bin/sh

FLAG=/boot/controller_mode

# If the controller-mode flag exists, remove it
if [ -f $FLAG ]; then
    rm -f $FLAG
fi