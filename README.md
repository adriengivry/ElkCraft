![alt text](header.jpg?raw=true "Class Diagram")

# What is ElkCraft ?
Here is ElkCraft, a Minecraft clone made with C++/OpenGL during 5 weeks in a group of 4 programmers.
The actual version allows you to play in an infinite world, generated at runtime, where you can build, destroy and mining or crafting things.
We implemented a health and a hunger system. There is also a day and night cycle.
Monsters aren't implemented.

# What is ElkEngine ?
ElkEngine is a voxel engine that we made in order to create this game.

# What this repo contains ?
- Release for 64-bits systems
- Source code
- Documentation (Mainly in french)

# Technical informations
In order to have fun with our work, you need to have :
- Windows
- Visual Studio 2017
- OpenGL 4.6 or higher
- At least 500MB of RAM to play

If you don't want to compile our project, you can launch the release in the "Bin/" folder.

# How to compile ?
In order to compile this project you need to follows these steps :
- Open the Visual Studio solution for ElkEngine ("Sources/ElkEngine/ElkEngine.sln")
- Build every projects in order (ElkTools --> ElkRendering --> ElkPhysics --> ElkGameEngine)
- Open the Visual Studio solution for ElkCraft ("Sources/ElkCraft/ElkCraft.sln")
- Build the project in the same configuration as you did for ElkEngine
- Hit play

# Recommandation
- You can build the project for every configuration (x64/x86 | Debug/Release)

# ElkEngine package diagram
![alt text](Doc/ElkEngine.jpg?raw=true "Class Diagram")

# ElkCraft class diagram
![alt text](Doc/ElkCraft.jpg?raw=true "Class Diagram")

# Credits
- Adrien GIVRY
- Charly JEAUC
- Saad RAOUF
- Hanseul SHIN
