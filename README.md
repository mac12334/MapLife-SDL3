## MapLife
MapLife is a program that is designed to make levels from tilemaps easy. MapLife will be able to help you edit levels in a fashion similarly to pixel art, then save the files into pure binary for efficient use.

## Why -SDL3?
I originally made the project for SDL2 but when I got to filepicking I realized it would be easier to use SDL3's ```SDL_ShowOpenFileDialougeOption()``` which is cross-compatible and easier than calling system specific functions for the same purpose.

## To Do List -
1. Make editing screen
   - The editing screen edits the current project.
   - Requires Map and file loading and saving
2. Make menu screen
   - The menu gives you options to load previous project
   - make new projects
3. Make transition screen from menu to edit
   - The transition screen allows you to pick which tilemap you want to use
   - input the size of each tile
4. Develope project structure
   - as in how the program will save, load, and refrence when editing and making projects.
   - also will save the path to the tilemap image used in the project
5. Make exporting screen
   - Exporting will need to include single level and multilevel projects
   - There is a possibility in the future of using multiple tilemaps in a single project and will have to account for that when the time comes.

## Remarks
This is a passion project I want to make it as good as it can be. I also will be drawing a lot of inspiration from Aseprite because they handle GUI impecibly.