#!/bin/bash
make run && dot out -Tpng -o out.png && xdg-open out.png
