#!/bin/sh
id=$RANDOM

echo $id env: $(env | grep test)
echo $id args: $@
read -t 0.1 a
echo $id read: $a
