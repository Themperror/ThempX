ThempX
======

DirectX 9.0 Engine

======

This is a basic DirectX Engine which works as following:

1. main.cpp  (Program entry point) (nothing here happens except for the instantiation of my engine and the creation of a window)

2. ThempX.cpp  ( The graphics device here gets claimed and linked, all my subclasses get instantiated and the game loop starts)

2a. Resource manager starts (ResourceHandler.cpp)

2b. Sound manager starts    (SoundHandler.cpp)

2c. Input manager starts    (InputHandler.cpp)

2d. All data gets assigned and DataStruct is created


3. game loop starts (ThempX.cpp)

Game loop goes as following
 
 -windows message pipeline (don't use it at all, but needed for the window)

 -FixedUpdate (runs every iteration of the loop thus very intensive on the CPU)

 -Update(), Input and everything is handled here (only handled each frame (currently 120 times per second))

 -Draw(),  all models and sprites get drawn here (only handled each frame (currently 120 times per second))

 
 repeat
 

4. when data.loop == false the cleanup starts, everything gets released and disposed of.


5. exit program

