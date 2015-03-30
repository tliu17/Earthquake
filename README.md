README.Earthquake

Files Edited:

App.cpp
- Rotates the earth when the user clicks and drags the mouse
- Draws earthquakes data extracted from input file

App.h
- Added global variable (CoordinateFrame rotation_general) that keeps track of the overall rotation of earth by mouse click-and-drag

Earth.cpp
- Converts latitude and longitude (in radians) to x,y,z coordinate
- Draws a sphere using given number of STACKS and SLICES. First tessellates a sphere, then texture-maps it. 

Description:

Equations in getPosition fucntion convert latitudes and longitudes (in radians) to x,y,z coordinates.
SetupGeometry function has one nested for-loop that increments latitude (outer loop) from -PI/2 to PI/2, and longitude (inner loop) from from -PI to PI.
Inside of the nested for-loop, two vertices, normals, and tex coordinates are set for each loop.
Slices of the sphere is rendered by TRIANGLE primitive type, similar to the soup can activity.
Local variables i, j, and k to keep track of total number of vertices, current number of stacks, and current number of vertices in each stack, respectively.
Draw::sphere fucntion draws earthquakes based on data from the input file.
Earthquakes are displayed in RED if the magnitude is greater than 7.0, ORANGE if the magnitude is between 6.0 and 7.0, GREEN if the magnitude is less than 6.0.
The radius of the earthquake is based on its magnitude.
When user clicks and drags the mouse, the earth rotates by following the same direction of movement as that of the mouse (for example if the mouse is dragged to the right, the earth also rotates to the right).
Specifically, the rotation axis is calculated as the vector normal to the plane between the z axis and the vector described by the mouse movement (cross product). 

