QSnake
======

a very simple game written in Qt5 based on these rules:

* Snake grows every 10 steps by one unit
* every step a brick is spawn
* bricks tend to colonize
* every 100 steps a bomb appears
* bombs can be collected and used with the spacebar
* bombs have a radius of 10 units

![](screenshots/2020-02-19_001.png)

# Video Demo
[![QSnake Gameplay Demo](https://img.youtube.com/vi/6lUHXIsVJDg/0.jpg)](https://www.youtube.com/watch?v=6lUHXIsVJDg "QSnake Gameplay Demo")


# Controls
* arrow keys
* spacebar (resets game and triggers bombs)
* P for pausing the game

# Requirements
* Monitor with full HD resolution
* some CPU power, it is really now written efficiently

# Build on Linux

```
qmake
make
```

# License
public domain


