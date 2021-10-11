DigiPen Animation - Project 1
Dylan Washburne

# Running
Open the .vcxproj file in Visual Studio 2019, run in x64 mode

# Controls

## Numpad:
0/. - Rotation
4/6 - Translation along x
2/8 - Translation along y
5 - Reset model

## ImGui
Click checkboxes, drag sliders

## Keyboard
Right-Click - Hold and drag to change camera angle
WASD - Move according to camera angle (FPS controls)
Shift - Increase move speed while held

# Models
Recieved from example course files.  Steve and Yodan are both *.x files which were imported through assimp
*.x files are a DirectX 2.0 format that is technically deprecated, but still contains all necessary information for rendering.  Thus, it can still be read and used.

# Animations
Also loaded in from the model files, through assimp

# Interpolation
Time between keyframes is calculated via `total time - next-lowest key time`, divided by `next-higher key time - next-lowest key time`
Rotation is calculated via Slerp, to use its cyclical pathing for the most true rotation angles/speeds/positions.
The quaternions are converted into a 4x4 matrix and multiplied with its parent to move the transform from local space to global space.

sin(Æ - Æt) / sin(Æ) * p0 + sin(Æt) / sin(Æ) * p1
