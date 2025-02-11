#!/bin/bash
make run && dot out -Tpng -o out.png && imv out.png
