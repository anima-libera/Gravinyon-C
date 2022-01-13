
# Gravinyon

A small indie action video game where your ship is attracted by your cursor
like a planet is attracted by its sun.

Programmed by Anima Libera, see github.com/anima-libera uwu.

## Note

This project is an _old_ poject of me and I sometimes start a new instance of it in the hope of getting closer to the ultimate goal of a finished game than the previous iterations. Obviously, it never happens. Thus, this time, instead of making a new project, I'm reviving this one (something I oddly never did before, as if an old project belongs to someone else or something). This revival consists in "updating" the code with my current C practices.

The OLD TODO list below has not been through the revival process yet and is still old (maybe inacurate). The NEW TODO list hs higher relevancy and priority.

## NEW TODO list

- Make the core loop filled with conditionnals to be able to "enable" and "disable" lots of features and aspects and debugging visuals at run time.
- Remove `if`s such as in `if (enable_bullets) { for (i = 0; i < bullet_number; i++) { ... }}` (as bullet_number should be `0` anyway), maybe even prefer something like `enable_shooting_bullets` instead of `enable_bullets`, or maybe even better: start some primitive weapon system with two weapons (the classic one and a debugging one that doesn't shoot).
- Make the code and the build system able to support testing of very different features and aspects of the game. To do this, have somewhere (in the C code) a list of functions, each of which will be called at game initialization to load and run one specific case to be tested/developed.
- Have some more general shaders for models, and use instancing to draw enemies and stuff.
- Use some _temporary library_ for text rendering (or not, text rendering is probably not important for now).
- Experiment with adding new types of enemies, contol mechanics and weapons, and make the process of adding these more easy along the way.

## OLD TODO list

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
