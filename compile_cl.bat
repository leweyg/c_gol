
# remove previous:
del gol.exe

# compile:
cl -std=c99 -o gol.exe main.c -lm

# test:
gol.exe BlinkEr
gol.exe beacon
gol.exe

