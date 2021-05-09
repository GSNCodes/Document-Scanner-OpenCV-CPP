## Document - Scanner

This repository contains code for a basica scanning application.  
When a path to a document image is provided, the following processes take place :-  
  * The biggest contours is assumed to be the document and hence it is first obtained.
  * The corner points are re-ordered according to the warp function's needs.
  * The cropped out portion from the original image is then warped to produce the final output.


## Notes

 * The source can be found within the "Project1" folder.
 * The code is no way perfect. You need to tweak the size of the image to get accurate outputs.
 * This work serves as a guide to perform image manipulations in C++.
