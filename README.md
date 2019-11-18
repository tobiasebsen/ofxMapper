# ofxMapper
2D Mapping / warping / blending / masking of multiple screens.

## class Mapper
The main class you add to your project. This class holds a collection of screen outputs - or just a single screen in case you are not using multiple screens. It also has an internal framebuffer (fbo) that can be used as source texture for mapping content.

Basic setup:  
```c++
ofxMapper::Mapper mapper; // Put this in your ofApp class
mapper.setup(800, 600); // In setup. Allocates internal frame buffer
```

## class Screen
The Screen class is where you add your slices (mapped rectangles) and masks. It is also has an internal frame buffer, which is where the slices and masks are drawn onto. In order to manage references and avoid data-copy, the mapper keeps a screen as a shared pointer called `ScreenPtr`.

Basic setup:
```c++
ofxMapper::ScreenPtr screen = mapper.addScreen(1920, 1080); // Add a screen to your mapper
```

## class Slice
A slice is a rectangular sub-section of an input texture that can be warped onto the output screen. Upon adding a slice you specify an input rectangle, which must be within the boundaries of your mapper's frame buffer size. The warping can be done by either a linear warper or a bezier warper within the slice.

Basic setup:
```c++
ofxMapper::SlicePtr slice = screen->addSlice(40, 65, 560, 400);
slice->bezierEnable = false;
````

## Handles
Warping the slice can be done using handles, which are points that control the shape of the slice. To see the handles, you basically call:
```c++
screen->drawHandles();
````
In order to manipulate the handles, you may want to add some mouse interaction:
```c++
void ofApp::mousePressed(int x, int y, int button) {
    screen->grabHandles(x, y);
}
void ofApp::mouseDragged(int x, int y, int button) {
    screen->dragHandles(x - ofGetPreviousMouseX(), y - ofGetPreviousMouseY());
}
void ofApp::mouseReleased(int x, int y, int button) {
  screen->releaseHandles();
}
```
