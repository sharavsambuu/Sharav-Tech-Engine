Sharav-Tech-Engine
==================

it's my private project, which is accumulation of my raw OpenGL knowledge. Of course it's not exactly an engine or a framework. it's just a bunch of experiments

used library list and tools :

    - GLM
    - Assimp
    - SOIL
    - GLFW
    - Mingw + Netbeans 7.4 + Windows 7
    
lib linking flags on windows

    -lSOIL -lassimp.dll -lglew32 -lglu32 -lglfw3 -lopengl32 -lglu32 -lgdi32 -luser32 -lkernel32
    

on ubuntu 18.04, installing dependencies and compiling

    sudo apt install libassimp-dev
    sudo apt install libglm-dev
    sudo apt install libsoil-dev
    sudo apt install libglfw3-dev
    sudo apt install libglew-dev

    g++ -o main *.cpp -lglfw -lGLEW -lGLU -lGL -lSOIL -lassimp

    ./compile.sh && ./main


![alt tag](http://2.bp.blogspot.com/-7rERDEBI7t8/UqRflZhDDCI/AAAAAAAAC34/vL0Gz37jiRE/s1600/fixed-material-manager-objects.jpg)

![alt tag](http://2.bp.blogspot.com/-vhVXksW-vYc/UqcmCN3p7kI/AAAAAAAAC4g/sIZ7G8wdkHs/s1600/DOF-5-iteration.jpg)
