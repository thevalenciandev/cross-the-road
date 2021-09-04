# cross-the-road
c++ console-based (n-curses) implementation of the popular "Cross the road" game.

![image](https://user-images.githubusercontent.com/45404603/131543666-23d8b0b6-22f5-472d-b87c-08cf22281b8b.png)

Heavily based on NVitanovic's one, with some key differences/improvements:
1) No flickering
2) No input lag
3) Usage of ncurses library (instead of conio).

To build, do (replace /mnt/c/Dev with your own dir):

`/usr/bin/g++ -g /mnt/c/Dev/cross-the-road/main.cpp -o /mnt/c/Dev/cross-the-road/main -lncurses`

2) Execute

`./main`


