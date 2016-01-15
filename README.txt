This C++11 program generates the following images in ppm format:

1) Standard wareya RGB test plane
2) Six interesting YCgCo planes (at 1/8ths step brightness from 1/8 to 7/8)

In the YCgCo planes, out-of-gamut colors are coded black, white, or gray depending on how they exceed the gamut.

White: Overbright
Black: Negative light
Gray: Both, in different channels

In sRGB, Linear, and Gamma 3 spaces.

sRGB is labeled "naiive".
Linear is labeled "linear".
Gamma 3 is labeled "visual".

sRGB is generated naiively, directly on individual sRGB channels linear with normal sRGB brightness.

Linear is corrected for even brightness of mixed colors.

Gamma 3 is generated differently from the other two: It's generated at a lower reference gamma, then corrected for even brightness.

"visual" should look be darker than the center of "naiive" in brightness, but have the same correct mixed color evenness as "linear".

Because Gamma 3 is generated differently, it has different out-of-bounds color areas, and they are organized in a less interesting way.

This demonstrates the following:

1) Interpolating chroma should be done in linear light
2) Interpolating luma should be done in perceptual light (approx gamma 3)
