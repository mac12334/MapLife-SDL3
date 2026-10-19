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
- october 4th, 2026 - added button callback functions and project struct.
   - Button callback functions - every button has it's own functionality intended so each button holds its own callback function to complete said functionality.
   - Button parameters - just like with the component parameters the button parameters allow you to pass in as many parameters as you need with void**.
   - project struct - one thing is that the end goal of MapLife is to allow a user to create projects that require level editing or maps. The project struct allows me to have fluid projects with mutliple different tilemaps or maps in the project. map is another term for level.
- October 11th, 2026 - added project loading, writing, and closing functions along with setting the maps to be within the project as a linked list and setting the tilemap dirs as a linked list.
   - project functions - the project is stored within a .MapLife file that in reality is just a .zip file with the changed prefix. This makes it easier to load in old projects because you just have to pick one file and then load it. The file also somewhat preserves directories which allows the application to pick which files to read and write only the map binary files.
   - Linked Lists - for both the tilemap directories and the maps you need to be able to have 'unlimited' ability to make as many maps or tilemaps for one project.
   - something to add in the future - The ability to make and pick maps while in the project editor. The project struct linked lists build up to that but it isn't yet realized. Also the ability to have .json file hold the tilemap directories in a list. Also modify map binaries slightly to allow for the first item to tell the project which tilemap is being used.
- October 11th, 2026 - changed the way buttons were defined. instead of defined using the 'new' keyword they are dynamically allocated using malloc making modifying and changing values easier.
   - old way - I originally used the new keyword for ease of use, but as the project got larger it soon was seen harder to use the new keyword when defining the buttons and textboxes in a one line addition to the linked list. I did use the delete keyword on all components after using new, I'm not a memory monster after all.
   - the new way - Now I will be using malloc to get a pointer to the gui component this allows me to use multiple lines to modify the component until putting it within the screen. then finally freed by the destroyLists method.
   - Why? - the main reason was because of working on the project loading and writing, I subconsciously used malloc without thinking about it, so I thought that it would probably be good to be used on screen components for better control over the components. I also knwo I could've used new outside of the add method for the screen but I think since I used malloc most often it makes my code easier to develop since everything uses the same memory allocation method.
- October 18th, 2026 - added the ability to pick which tilemap your using, and stored that in the '.MapLife' file format.
   - How the tilemap is stored - the tilemap is stored as a directory towards the tilemap image. The tilemap is located in the zip file ('.MapLife' file), in properties.json. This allows for the ability to store multiple tilemap directories in the future and store other properties like tile size and the number of columns in the tilemap.
   - What needs to be added - the ability to change the number of columns used and to change the number of tiles used in the tilemap. essentially anything used to grab the tiles from the image needs to be a property of the project. possibly the property of the project could be another linked list that stores a tilemap property struct, this struct could also hold the tilemap directory.
   - another addition - we still need the ability to have multiple maps in the project but we are building towards that by using linked lists for now.

## To Do List -
1. Make editing screen - **completed**
   - The editing screen edits the current project.
   - Requires Map and file loading and saving
2. Make menu screen - **complete**
   - The menu gives you options to load previous project
   - make new projects
3. Make transition screen from menu to edit - **in progress**
   - The transition screen allows you to pick which tilemap you want to use
   - input the size of each tile
4. Develope project structure - **in progress** // kinda working on this as the project progresses
   - as in how the program will save, load, and refrence when editing and making projects.
   - also will save the path to the tilemap image used in the project
5. Make exporting screen - **not completed**
   - Exporting will need to include single map and multi-map projects
   - There is a possibility in the future of using multiple tilemaps in a single project and will have to account for that when the time comes.

## Remarks
This is a passion project I want to make it as good as it can be. I also will be drawing a lot of inspiration from Aseprite because they handle GUI impecibly.