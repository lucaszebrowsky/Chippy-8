# Chippy-8
A fast Chip 8 Emulator written in C.

## Building

#### 1. Install the dependencies

* Ubuntu/Debian:

**NOTICE:** As of writing, neither Ubuntu nor Debian provide SDL3 packages. Please use the v1.0 tag, which relies on SDL2 instead.

* Fedora
```shell
$ sudo dnf install make gcc SDL3-devel git
```

* Arch
```shell
$ sudo pacman -S base-devel sdl3 git
```

#### 2. Clone the repo and cd into it:

```
$ git clone https://github.com/Lu-Die-Milchkuh/Chippy-8.git && cd Chippy-8
```

#### 3. Compilation

```
$ make
```

Then execute it like the following:
```
$ ./Chippy-8 <path_to_rom>
```
## Controls

The Original Chip-8 has the following Keyboard Layout:
```
1   2   3   C
4   5   6   D
7   8   9   E
A   0   B   F
```
which are mapped like the following on your keyboard
```
1   2   3   4
Q   W   E   R
A   S   D   F
Y   X   C   V
```

## Screenshots
![testrom_sucess](screenshots/testrom_success.png)
![space_invaders_titlescreen](screenshots/space_invaders_titlescreen.png)
![pong_gameplay](screenshots/pong_gameplay.png)
