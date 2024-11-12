# Image Compression Using Quadtrees

### Theme Description

In this project, a **.ppm** image file will be provided, along with a compression level factor 
indicating the desired compression level and an **.out** file where the required information will be written. 
The compression is based on a quadtree that contains the average RGB values from a 
portion of pixels in the image. The requirements are as follows:

- **-c1** - The quadtree will be created, and information about the generated tree will be written in the **.out** file (including the tree's height, the number of leaf nodes, and the largest uncompressed block).
- **-c2** - The **.out** file will contain binary data from each level of the tree and information about the compressed image (its size).
- **-d** - The **.in** file will be the compressed file from **-c2**, and the quadtree will be reconstructed based on it. Using this tree, the original **.ppm** image will be recreated.

### Implementation

For **-c1** and **-c2**, the program will read image data and construct the quadtree 
based on the provided data (using a structure that tracks the indices of the submatrix 
that the node in the tree represents). The node is split into 4 equal nodes representing 
a submatrix of size n x n, and the split occurs only if the average color of the matrix/submatrix 
is greater than the imposed factor. The information about the tree will then be written to the **.out** 
file based on the requested options. For **-d**, the program will read from the **.in** file, reconstruct 
the quadtree from the file, and then rebuild the image based on the tree (placing the RGB 
values from the tree into the matrix based on the position indices). The output image will be in **.ppm** format.

### How to Run

```bash
./quadtree [-c1 factor | -c2 factor | -d] [input_file] [output_file]
Where factor is the compression level.
