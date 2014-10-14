#ifndef _CLASSLIST_H_
#define _CLASSLIST_H_

class BaseCamera;  //base type for any sort of camera
class Billboard;
class Bitmap;  //2d images
class Camera;  //basic camera for displaying a scene
class Collidable;   //parent class for anything ship/weapons can hit (except walls)
class ColorShader;  //displays all vertices as a single color
class D3Dgraphics;  //low level graphics
class Debris;  //collidables that can smack into the ship or be shot at
class Font;
class FontShader;
class Fps;
class GameState;  //interface for different states of the game
class GameStateManager;  //keeps track and runs current state
class Gapp;  //main app
class Graphics;  //uses d3d to do graphics
class Gui;
class Input;  //handles input from mouse/keyboard
class Level;  //contains info about ship and track for next level
class Light;
class LightShader;
class MainMenuState;  //opening menu of game
class Model;  //3d object loaded from a text file
class ModelManager;
class ProgressBar;  //for gui
class Quaternion;
class Renderable;   //any object thats rendered in 3d world
class Ring;   //a ring for the ship to fly through
class SelectionState;  //state for preparing ship and level
class Shield;  //handles graphics for shields of ships
class ShieldShader;  //shader used for creating shield effects
class Ship;  //base object for a ship.  may split off into classes for enemy/other...
class Shot;  //parent class for any type of weapon the ship shoots
class SpringCamera;
class TestingState;  //gamestate used to test different graphics
class Text;  
class Texture;  //holds single texture
class TextureManager;  //keeps track of all textures
class TextureShader;  //renders from a texture
class Timer;
class TrackLine;  //draws a line to represent path of a track segment
class TrackManager;  //creates several track segements to make a full track for a level
class TrackSegment;  //keeps data on single piece of game track
class TrackState;  //currently main game state
class TrackTube;  //creates and draws the tube that surrounds the track
class TubeShader;  //shader used for rendering the tube
class Vector;
class Window;  //for gui

#endif;