g++ src/main.cpp include/libs/SimplexNoise.cpp src/glad.c -Iinclude -lglfw -ldl -lGL -o triangle && echo "compiled" || echo && ./triangle
