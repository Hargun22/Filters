# Image Convolution

This C program performs a convolution on a given image file, based on a provided convolution kernel.

To run the program, execute the script "filter" like so: ./filter input.ppm kernel output.ppm

Running the script with the specified parameters will use the kernel provided in the file "kernel" to perform an image convolution on the provided PPM file, and output the result in another PPM file.

Kernels can be found at: https://en.wikipedia.org/wiki/Kernel_(image_processing)

The text file "kernel" is formatted like so:

```
n
scale
n rows of n numbers each
```
For example:

```
3
9
1 1 1
1 1 1
1 1 1
```
