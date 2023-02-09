# CSC8502-Graphics
Graphics project containing the individual challenges and final submission, initial codebase provided by university. 
Tutoirials contain further work from me and implementation of the final solution is my own work. Some tutorials have been removed for not being relevant/being to simple for me to make any of my own adjustements.

##Summary of Work
|Feature|Description|
|-------|-----------|
|Real-Time Lighting|Lights in the final submitted scene are rendered at real-time using OpenGl shaders, as well multiple lights can exist in a scene by passing the light data in Universal Buffer Objects|
|Skeletal Animations|A mesh with a skeleton can be loaded along with an animation, and over time haver the mesh perform this animation in a scene - Skeletal-Animation|
|Cube Mapping|In the submitted work project there exists a skybox that has used cube mapping to display a texture over the world where nothing exists|
|Grey scale|Applied in the fragment shader for the submmitedd work exists a function that provides a  grey scale effect to makes fragments in the distance grey, to add a fog effect and to avoid rendering unnecesary fragments|
|Post Processing|A gaussian blur effect has been added, which uses the depth buffer and smapling of texture matrix|

##Controls  
F5: reload shaders
WASD: forward, left, backward and right strafe
P: loop through camera motions and lock camera
O: cancel camera loop and unlock camera
ESCP: close project window
