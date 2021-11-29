
# Gravinyon

A small indie action video game where your ship is attracted by your cursor
like a planet is attracted by its sun.

Programmed by Anima Libera, see github.com/anima-libera uwu.

## TODO

- Style
  - Maybe (maybe) consider using `struct part` instead of `part_t` etc (maybe)
- Code quality
  - Check for the success of every SDL function call
  - Free everything
  - Do the core loop and state thing uwu
- Less dependecies
  - Remove the GLEW dependency by manually grabbing the function pointers
  - Remove the SDL2 dependency by doing all of what it does manually
    - This one will take time
- Text rendering
  - Make a tilemap of ascii chars
  - Render text with it
  - Make the tilemap font cooler
    - By using the green channel to represent a threshold
    - Use this threshold to render clouds
    - Clouds seem to be emmited by the characters and fade away
- Audio
  - Play some music
- Ship
  - Make the ship face the direction it is going
  - Add a small floating diamond that looks at the cursor and shoots
  - Add trails behind the ship
  - Add a shooting animation
- 3D
  - Make some objects look like 3D by adding shadows according to the angle
  - Maybe
- Cursor
  - Make the cursor look neat (little diamonds)
  - Make it rotate
  - Add a little animation for shooting and stuff
  - Make the cursor appear and disappear during some moments
    - By making the diamonds further and further appart until out of the screen
- Bullets
  - Show it with like the bullets' color but smooth
- Portability
  - Make sure it works on other Linux distros
  - Port to Windows
  - Port to MacOS
