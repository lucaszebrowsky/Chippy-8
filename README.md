# Chippy-8
A fast Chip 8 Emulator written in C.


## Building

### 1. Clone the repo:
```
git clone https://github.com/Lu-Die-Milchkuh/Chippy-8.git
```

### 2. Install the dependencies

* Ubuntu/Debian:
```    
sudo apt install build-essential libsdl2-dev
```

* Arch
```
sudo pacman -S base-devel sdl2
```

### 3. Compilation
```
gcc -std=c11 -Wall -Werror -flto -march=native -O2 -lSDL2 Chippy-8.c -o Chippy8
```