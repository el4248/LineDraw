Author: Eric La

Dependencies: FLTK, GLUT
Complilation and Execution:
Compile with 'make'
Run as 'test.out'
Clean with 'make clean'

Notes: This program is an old project so there are times when things might break, if that is the case just follow the instructions on the terminal. In addition, intersections will sometimes return fewer/more points than it should. Also, don't use more than 10 curves at a time unless you don't mind the strange looking interface.

IMPORTANT:
-if program crashes just restart, there seems to be some odd errors floating around
-Pay attention to the terminal as well since it has some extra information on the operation of the program

Instructions For Use:
-Use the right hand menu to change setings
-The 4 options on the top right of the menu deal with the cursor settings (more on that below)
-Panel on left is mapped such that the center is the origin (0,0)
-Bezier and Castel both draw all curves (active or not) and returns the time it took
)
-If there are too few intersections try increasing E and try again. Doe the opposite if there are too many
_______
UI info|_________________________________
Black Points are currently selected.
Red Points are ones that have not been set.
Blue Points are part of a curve.
Anything else is generally a different color for easier viewing
Change the check boxes to toggle options
Use the "T" slider to change the "T" value of all curves
Use the "E" slider to change the error value for intersections
The check boxes on the bottom right to toggle visible curves and options
___________
Basic Usage|
--------------------------------------------------------------------------------
Graph: Set currently selected selected points as a curve
Undo: Remove the last unset point, if there are none undo graph
DeletePT: Delete currently selected point
InsertPt: Insert new control point into curve, mode is switced to move
Intrsct: (Intersect) looks for the intersection points of the first 2 active curves (starts from c0 and moves up)
Clear: Remove everything on the canvas
Exit: exactly what it says on the tin
_________
Modes:   |
--------------------------------------------------------------------------------
Select: click to pick points to move, selected points are highlighted in black. (limited to number within range of cursor)

Set: set individual points, Graph makes them into a curve, non-permanent points are in red.

Link:--------------Does nothing-------------------

Move: move selected Point(s) to mouse cursor. Note that it is a click to pick up and a click to place. DO NOT CLICK AND DRAG


_________
Toggles: |
--------------------------------------------------------------------------------
Control Points: Control Points
Curves: Curves
Lines: Intermediate Lines in finding "T"
Polygons: Control Polygon
Subdivides: subDivision of graphs at "T"
t-Point: current point mapped to by "T"
