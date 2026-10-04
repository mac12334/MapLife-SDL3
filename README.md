## MapLife
MapLife is a program that is designed to make levels from tilemaps easy. MapLife will be able to help you edit levels in a fashion similarly to pixel art, then save the files into pure binary for efficient use.

## Why -SDL3?
I originally made the project for SDL2 but when I got to filepicking I realized it would be easier to use SDL3's ```SDL_ShowOpenFileDialougeOption()``` which is cross-compatible and easier than calling system specific functions for the same purpose.

## Code Updates -
- october 3rd, 2026 - added screen class and LinkedList.
    - Linked list - I know I could've used vectors but I like making some things from scratch
    - Screen - The screen class holds Linked lists of each component on the screen and updates them every tick (iterate function in main)
    - The need for linked lists was so that I could have any amount of buttons and textboxes and needed for the list also textures in the future.
    - This code change was added to make the screens on the to-do list possible.
    - One thing to add to this is the ability for each button to do a different functionality.

## To Do List -
1. Make editing screen - **completed**
   - The editing screen edits the current project.
   - Requires Map and file loading and saving
2. Make menu screen - **not completed**
   - The menu gives you options to load previous project
   - make new projects
3. Make transition screen from menu to edit - **not completed**
   - The transition screen allows you to pick which tilemap you want to use
   - input the size of each tile
4. Develope project structure - **not completed**
   - as in how the program will save, load, and refrence when editing and making projects.
   - also will save the path to the tilemap image used in the project
5. Make exporting screen - **not completed**
   - Exporting will need to include single level and multilevel projects
   - There is a possibility in the future of using multiple tilemaps in a single project and will have to account for that when the time comes.

## Remarks
This is a passion project I want to make it as good as it can be. I also will be drawing a lot of inspiration from Aseprite because they handle GUI impecibly.