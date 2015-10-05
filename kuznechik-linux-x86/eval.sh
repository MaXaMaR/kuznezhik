#!/bin/sh

if [ -f results.txt ]; then rm results.txt; fi
for ex in `ls xtest*`; do ./$ex >> results.txt; echo "------------------------------------------" >> results.txt; done
