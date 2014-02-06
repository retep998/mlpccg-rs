Here is where all the assets go.

Folder structure
* backgrounds/ - Client randomly picks an image to use from this folder as a background. Image will be scaled to fit, but the aspect ratio will be maintained.
* font.ttf - This font is used to do any text rendering
* chars6x8.png - Used by ascii to determine coverage values for properly generating ascii art. Should be a single row of characters with no padding or extra gaps. Make sure this image has a completely black background and the font itself is a monochrome bitmap. Also ensure the size of each character matches the specified dimensions (in this case 6 by 8, but if you decide to use a different size, make sure you update the code accordingly and use the correct file name).
