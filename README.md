# Chippy-8
A fast Chip 8 Emulator written in C.

## Building

#### 1. Install the dependencies

* Ubuntu/Debian:
```    
sudo apt install build-essential libsdl2-dev git
```

* Arch
```
sudo pacman -S base-devel sdl2
```

#### 2. Clone the repo and cd into it:

```
git clone https://github.com/Lu-Die-Milchkuh/Chippy-8.git
cd Chippy-8
```

#### 3. Compilation

```
gcc -std=c11 -Wall -Werror -flto -march=native -O2 -lSDL2 Chippy-8.c -o Chippy8
```