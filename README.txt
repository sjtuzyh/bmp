This C++11 program generates the following images in ppm format:

1) Standard wareya RGB test plane
2) Six interesting YCgCo planes (at 1/8ths step brightness from 1/8 to 7/8)

In sRGB, Linear, and Gamma 3 spaces.

The YCgCo planes are generated using SRGB reference brightness levels, then converted to the other two gammaspaces. Every plane would appear slightly different if they were generated in Gamma 3 or Linear space instead, due to how YCgCo works. *YCgCo is an encoding of arbitrary RGB colorspaces, not a well-defined colorspace in and of itself.*