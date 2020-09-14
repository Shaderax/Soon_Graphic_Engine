# ShaderCompiler

The main goal of this project is to stop recompiling shader files by hand and to recompile them as soon as they are modified.  
The ShaderCompiler executable can be launched on a folder and will recursively look for all modifications on shader files (.vert|.frag...) to compile them with glslc.  
The program will not run if there is already an instance that observes the same folder. For the moment it is better to use the Full Path of a folder.  
The executable uses a named pipe to transmit glslc program return errors and so this pipe can be opened from another program and read it.

## Disclaimer
Not Complete for Windows  
Ok for Linux  
This project use [efsw](https://github.com/SpartanJ/efsw) to observe a folder.