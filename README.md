# CS 143B Demos

## How to use it

`make && ./shared_var`

## Environment
Tested under Ubuntu 18.04, g++ 8.2.0. <br/>
If you are on UCI ICS Openlab, do `module load gcc/8.2.0` before running `make`

## Measurement
Use `time sudo ionice -c 1 -n 0 <program>` to measure the performance to elimate the effect of the scheduling policy on our program.
