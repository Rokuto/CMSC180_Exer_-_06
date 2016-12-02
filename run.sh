#!/bin/bash

rm output.txt

mpicc -o exer.exe Pascua_Exer_06.c

sbatch submit.sh ./exer.exe

