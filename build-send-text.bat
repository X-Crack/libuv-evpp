@echo off 

for /l %%n in (1,1,4096) do (

type 1.txt >> 2.txt

)