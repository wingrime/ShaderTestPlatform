# ShaderTestPlatform
My Shader playground, Little project for playing GLSL and modern OpenGL, and ***HARDCORE realtime rendering***
![](https://www.dropbox.com/s/85a514zs9cjf9mq/test_model48_blum_fix.JPG?raw=1)



NOT buildable untill I config suprepos and depedences


Require CMake and MinGW for build

External depedences should be added later as Git Subrepo's
# What was done
 * Normal mapping
 * Possion sampling soft shadows
 * SH-Based Atomspheric lightning
 * SH-Basis RGB values are generating using compute shader from cubemap, generated from scene
 * PBR-based BRDF
 * HDR Pipeline with Uncharted tonemapping function
 * Sky-model based on perez all weather model
 * Bloom

# TODOs
* Cascaded shadows
* Some GI tech (voxel,lpv, light-maps)
* Better piplenine chaing
* Some multilight tech (forward+,deffered)


