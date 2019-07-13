#! /bin/bash
mpiexec ${1} -n 1 python3 scriptYade.py : -n 2 icoFoamYade -parallel

