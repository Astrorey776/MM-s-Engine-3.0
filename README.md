# MM-s-Engine-3.0

Welcome to MM's Engine, a video game engine under development by Adrian Martin Murillo, a CITM student. As the engine is updated, the readme will be updated to cover all the new features. Thank you for your attention.

# Creator

- Adrian Martin
   *Github Account: [Adrian Martin](https://github.com/Astrorey776)
***

## Libraries Used:

&emsp; -&emsp;*[SDL2](https://www.libsdl.org/)*

&emsp; -&emsp;*[OpenGL](https://www.opengl.org/)* 

&emsp; -&emsp;*[ImGui](https://github.com/ocornut/imgui)*

&emsp; -&emsp;*[GLEW](https://glew.sourceforge.net/)*

&emsp; -&emsp;*[DevIL](https://openil.sourceforge.net/)*

&emsp; -&emsp;*[MathGeoLib](https://github.com/juj/MathGeoLib)*

&emsp; -&emsp;*[GLMath](https://glmath.sourceforge.net/)*

&emsp; -&emsp;*[PhysFS](https://github.com/icculus/physfs/)*

&emsp; -&emsp;*[Bullet](https://github.com/bulletphysics/bullet3)*


***

## Guid to execute
 * Go to Release  
 * Download the Engine
 * Unpack .zip
 * Execute .exe

***

## Windows
* Drag and drop is enabled to drop fbx and import it to the project.

* Dock the windows were you want.

###  Main Bar
Here you can find multiple dropdowns: 
* *File*: there's a button which allows you to save or load the scene, and a button to close the engine.
* *GameObjects*: a list of basic shapes to use.
* *Edit*: A window to configure some engine parameters where you can find some headers with some propertis of windows or configuration:
* *FPS*: Show the name of engine and the organisation that help us, shos too the fps (you can limitate), miliseconds and it shows u the with and height of the windows.
* *Visual*: Buttons to active some aditional widgets of the windows: VSync, brightness, lights, dephts test, face culling, color material, textures.
* *Hardware*: Show the CPU, GPU, the GB of RAM, the Caps that your PC are using and the versions of some librarys that we use.
.
* *More*: A window showing information about the engine and its creator with all the libraries linked to its repository.

###  Inspector
Show the components of the selected GameObject and a cube to Disable/Enable the GameObject.
* Transform with position, rotarion and scale of the object.
* Mesh of the object.

### Camera
* Added frustum camera
* Added the camera collisions and the possibility to shoot balls with the camera.

### Camera Controls
- Mouse right click: Rotate camera.
- Mouse wheel: to move forward and backwards the camera.
- Shift + left click: Focus the camera on the selected Game Object and rotate on it's orbit.
- WASD buttons: move camera postion, forward, bacward and sideways.
- Shift + (WASD): move camera postion faster. 
- F: Focus the camera on the slected Game Object.
- Press the "1" key to shoot spheres in the direction of the camera.

### Car controls 
UP: Accelerate the car.
DOWN: brake/reverse acceleration.
LEFT: Turn wheels right direction.
RIGHT: Turn wheels left direction..

###  Hierarchy
* Show the list of the GameObjects in the scene and their parents/chillds.

###  Console

* A simple console

###  Scene

* A button to activate play mode.
* Draw the render of the scene.
* Render the car
* Renders 4 types of joint to different objects 
* All buildings in city environment have the new component Collider.