# Gem Puzzle Solver

## Linux

SFML can be installed using the following command
```sh
sudo apt-get install libsfml-dev
```

## Windows
Requires GCC 13.1.0 MinGW for building on Windows, which can be downloaded from [here](https://github.com/brechtsanders/winlibs_mingw/releases/download/13.1.0-16.0.5-11.0.0-msvcrt-r5/winlibs-x86_64-posix-seh-gcc-13.1.0-mingw-w64msvcrt-11.0.0-r5.7z) 


### To build an executable file with dynamic link libraries (`.dll` need to be in the same directory as the executable)

#### Compiling
```powershell
g++ -c main.cpp -I".\src\include"
```

#### Linking
```powershell
g++ main.o -o gem_puzzle -L".\src\lib" -lsfml-graphics -lsfml-audio -lsfml-window  -lsfml-system
```
#### Running
```powershell
.\gem_puzzle
```

### To build an executable file with static link libraries (no need for `.dll` files)

#### Compiling
```powershell
g++ -c main.cpp -I".\src\include" -DSFML_STATIC
```

#### Linking
```powershell
g++ main.o -o gem_puzzle -L".\src\lib" -lsfml-graphics-s -lsfml-audio-s -lsfml-window-s -lsfml-system-s -lopengl32 -lfreetype -lwinmm -lgdi32 -mwindows
```

#### Running
```powershell
.\gem_puzzle
```
