// stb_perlin.h - v0.3 - perlin noise
// public domain single-file C implementation by Sean Barrett
//
// LICENSE
//
//   See end of file.
//
//
// to create the implementation,
//     #define STB_PERLIN_IMPLEMENTATION
// in *one* C/CPP file that includes this file.
//
//
// Documentation:
//
// f32  stb_perlin_noise3( f32 x,
//                           f32 y,
//                           f32 z,
//                           int   x_wrap=0,
//                           int   y_wrap=0,
//                           int   z_wrap=0)
//
// This function computes a random value at the coordinate (x,y,z).
// Adjacent random values are continuous but the noise fluctuates
// its randomness with period 1, i.e. takes on wholly unrelated values
// at integer points. Specifically, this implements Ken Perlin's
// revised noise function from 2002.
//
// The "wrap" parameters can be used to create wraparound noise that
// wraps at powers of two. The numbers MUST be powers of two. Specify
// 0 to mean "don't care". (The noise always wraps every 256 due
// details of the implementation, even if you ask for larger or no
// wrapping.)
//
// Fractal Noise:
//
// Three common fractal noise functions are included, which produce 
// a wide variety of nice effects depending on the parameters 
// provided. Note that each function will call stb_perlin_noise3 
// 'octaves' times, so this parameter will affect runtime.
//
// f32 stb_perlin_ridge_noise3(f32 x, f32 y, f32 z,
//                               f32 lacunarity, f32 gain, f32 offset, int octaves,
//                               int x_wrap, int y_wrap, int z_wrap);
//
// f32 stb_perlin_fbm_noise3(f32 x, f32 y, f32 z,
//                             f32 lacunarity, f32 gain, int octaves,
//                             int x_wrap, int y_wrap, int z_wrap);
//
// f32 stb_perlin_turbulence_noise3(f32 x, f32 y, f32 z,
//                                    f32 lacunarity, f32 gain,int octaves,
//                                    int x_wrap, int y_wrap, int z_wrap);
//
// Typical values to start playing with:
//     octaves    =   6     -- number of "octaves" of noise3() to sum
//     lacunarity = ~ 2.0   -- spacing between successive octaves (use exactly 2.0 for wrapping output)
//     gain       =   0.5   -- relative weighting applied to each successive octave
//     offset     =   1.0?  -- used to invert the ridges, may need to be larger, not sure
//    
//
// Contributors:
//    Jack Mott - additional noise functions
//

#ifdef __cplusplus
extern "C" {
#endif
extern f32 stb_perlin_noise3(f32 x, f32 y, f32 z, i32 x_wrap, i32 y_wrap, i32 z_wrap);
extern f32 stb_perlin_ridge_noise3(f32 x, f32 y, f32 z,f32 lacunarity, f32 gain, f32 offset, i32 octaves, i32 x_wrap, i32 y_wrap, i32 z_wrap);
extern f32 stb_perlin_fbm_noise3(f32 x, f32 y, f32 z, f32 lacunarity, f32 gain, i32 octaves, i32 x_wrap, i32 y_wrap, i32 z_wrap);
extern f32 stb_perlin_turbulence_noise3(f32 x, f32 y, f32 z, f32 lacunarity, f32 gain, i32 octaves, i32 x_wrap, i32 y_wrap, i32 z_wrap);
#ifdef __cplusplus
}
#endif

#ifdef STB_PERLIN_IMPLEMENTATION

// not same permutation table as Perlin's reference to avoid copyright issues;
// Perlin's table can be found at http://mrl.nyu.edu/~perlin/noise/
static u8 stb__perlin_randtab[512] =
{
    23, 125, 161, 52, 103, 117, 70, 37, 247, 101, 203, 169, 124, 126, 44, 123, 
    152, 238, 145, 45, 171, 114, 253, 10, 192, 136, 4, 157, 249, 30, 35, 72, 
    175, 63, 77, 90, 181, 16, 96, 111, 133, 104, 75, 162, 93, 56, 66, 240, 
    8, 50, 84, 229, 49, 210, 173, 239, 141, 1, 87, 18, 2, 198, 143, 57, 
    225, 160, 58, 217, 168, 206, 245, 204, 199, 6, 73, 60, 20, 230, 211, 233, 
    94, 200, 88, 9, 74, 155, 33, 15, 219, 130, 226, 202, 83, 236, 42, 172, 
    165, 218, 55, 222, 46, 107, 98, 154, 109, 67, 196, 178, 127, 158, 13, 243, 
    65, 79, 166, 248, 25, 224, 115, 80, 68, 51, 184, 128, 232, 208, 151, 122, 
    26, 212, 105, 43, 179, 213, 235, 148, 146, 89, 14, 195, 28, 78, 112, 76, 
    250, 47, 24, 251, 140, 108, 186, 190, 228, 170, 183, 139, 39, 188, 244, 246, 
    132, 48, 119, 144, 180, 138, 134, 193, 82, 182, 120, 121, 86, 220, 209, 3, 
    91, 241, 149, 85, 205, 150, 113, 216, 31, 100, 41, 164, 177, 214, 153, 231, 
    38, 71, 185, 174, 97, 201, 29, 95, 7, 92, 54, 254, 191, 118, 34, 221, 
    131, 11, 163, 99, 234, 81, 227, 147, 156, 176, 17, 142, 69, 12, 110, 62, 
    27, 255, 0, 194, 59, 116, 242, 252, 19, 21, 187, 53, 207, 129, 64, 135, 
    61, 40, 167, 237, 102, 223, 106, 159, 197, 189, 215, 137, 36, 32, 22, 5,  

    // and a second copy so we don't need an extra mask or static initializer
    23, 125, 161, 52, 103, 117, 70, 37, 247, 101, 203, 169, 124, 126, 44, 123, 
    152, 238, 145, 45, 171, 114, 253, 10, 192, 136, 4, 157, 249, 30, 35, 72, 
    175, 63, 77, 90, 181, 16, 96, 111, 133, 104, 75, 162, 93, 56, 66, 240, 
    8, 50, 84, 229, 49, 210, 173, 239, 141, 1, 87, 18, 2, 198, 143, 57, 
    225, 160, 58, 217, 168, 206, 245, 204, 199, 6, 73, 60, 20, 230, 211, 233, 
    94, 200, 88, 9, 74, 155, 33, 15, 219, 130, 226, 202, 83, 236, 42, 172, 
    165, 218, 55, 222, 46, 107, 98, 154, 109, 67, 196, 178, 127, 158, 13, 243, 
    65, 79, 166, 248, 25, 224, 115, 80, 68, 51, 184, 128, 232, 208, 151, 122, 
    26, 212, 105, 43, 179, 213, 235, 148, 146, 89, 14, 195, 28, 78, 112, 76, 
    250, 47, 24, 251, 140, 108, 186, 190, 228, 170, 183, 139, 39, 188, 244, 246, 
    132, 48, 119, 144, 180, 138, 134, 193, 82, 182, 120, 121, 86, 220, 209, 3, 
    91, 241, 149, 85, 205, 150, 113, 216, 31, 100, 41, 164, 177, 214, 153, 231, 
    38, 71, 185, 174, 97, 201, 29, 95, 7, 92, 54, 254, 191, 118, 34, 221, 
    131, 11, 163, 99, 234, 81, 227, 147, 156, 176, 17, 142, 69, 12, 110, 62, 
    27, 255, 0, 194, 59, 116, 242, 252, 19, 21, 187, 53, 207, 129, 64, 135, 
    61, 40, 167, 237, 102, 223, 106, 159, 197, 189, 215, 137, 36, 32, 22, 5,  
};

static f32 stb__perlin_lerp(f32 a, f32 b, f32 t)
{
    return a + (b-a) * t;
}

static i32 stb__perlin_fastfloor(f32 a)
{
    i32 ai = (i32) a;
    return (a < ai) ? ai-1 : ai;
}

// different grad function from Perlin's, but easy to modify to match reference
static f32 stb__perlin_grad(i32 hash, f32 x, f32 y, f32 z)
{
    static f32 basis[12][4] =
    {
        {  1, 1, 0 },
        { -1, 1, 0 },
        {  1,-1, 0 },
        { -1,-1, 0 },
        {  1, 0, 1 },
        { -1, 0, 1 },
        {  1, 0,-1 },
        { -1, 0,-1 },
        {  0, 1, 1 },
        {  0,-1, 1 },
        {  0, 1,-1 },
        {  0,-1,-1 },
    };

    // perlin's gradient has 12 cases so some get used 1/16th of the time
    // and some 2/16ths. We reduce bias by changing those fractions
    // to 5/64ths and 6/64ths, and the same 4 cases get the extra weight.
    static u8 indices[64] =
    {
        0,1,2,3,4,5,6,7,8,9,10,11,
        0,9,1,11,
        0,1,2,3,4,5,6,7,8,9,10,11,
        0,1,2,3,4,5,6,7,8,9,10,11,
        0,1,2,3,4,5,6,7,8,9,10,11,
        0,1,2,3,4,5,6,7,8,9,10,11,
    };

    // if you use reference permutation table, change 63 below to 15 to match reference
    // (this is why the ordering of the table above is funky)
    f32 *grad = basis[indices[hash & 63]];
    return grad[0]*x + grad[1]*y + grad[2]*z;
}

f32 stb_perlin_noise3(f32 x, f32 y, f32 z, i32 x_wrap, i32 y_wrap, i32 z_wrap)
{
    f32 u,v,w;
    f32 n000,n001,n010,n011,n100,n101,n110,n111;
    f32 n00,n01,n10,n11;
    f32 n0,n1;

    u32 x_mask = (x_wrap-1) & 255;
    u32 y_mask = (y_wrap-1) & 255;
    u32 z_mask = (z_wrap-1) & 255;
    i32 px = stb__perlin_fastfloor(x);
    i32 py = stb__perlin_fastfloor(y);
    i32 pz = stb__perlin_fastfloor(z);
    i32 x0 = px & x_mask, x1 = (px+1) & x_mask;
    i32 y0 = py & y_mask, y1 = (py+1) & y_mask;
    i32 z0 = pz & z_mask, z1 = (pz+1) & z_mask;
    i32 r0,r1, r00,r01,r10,r11;

#define stb__perlin_ease(a)   (((a*6-15)*a + 10) * a * a * a)

    x -= px; u = stb__perlin_ease(x);
    y -= py; v = stb__perlin_ease(y);
    z -= pz; w = stb__perlin_ease(z);

    r0 = stb__perlin_randtab[x0];
    r1 = stb__perlin_randtab[x1];

    r00 = stb__perlin_randtab[r0+y0];
    r01 = stb__perlin_randtab[r0+y1];
    r10 = stb__perlin_randtab[r1+y0];
    r11 = stb__perlin_randtab[r1+y1];

    n000 = stb__perlin_grad(stb__perlin_randtab[r00+z0], x  , y  , z   );
    n001 = stb__perlin_grad(stb__perlin_randtab[r00+z1], x  , y  , z-1 );
    n010 = stb__perlin_grad(stb__perlin_randtab[r01+z0], x  , y-1, z   );
    n011 = stb__perlin_grad(stb__perlin_randtab[r01+z1], x  , y-1, z-1 );
    n100 = stb__perlin_grad(stb__perlin_randtab[r10+z0], x-1, y  , z   );
    n101 = stb__perlin_grad(stb__perlin_randtab[r10+z1], x-1, y  , z-1 );
    n110 = stb__perlin_grad(stb__perlin_randtab[r11+z0], x-1, y-1, z   );
    n111 = stb__perlin_grad(stb__perlin_randtab[r11+z1], x-1, y-1, z-1 );

    n00 = stb__perlin_lerp(n000,n001,w);
    n01 = stb__perlin_lerp(n010,n011,w);
    n10 = stb__perlin_lerp(n100,n101,w);
    n11 = stb__perlin_lerp(n110,n111,w);

    n0 = stb__perlin_lerp(n00,n01,v);
    n1 = stb__perlin_lerp(n10,n11,v);

    return stb__perlin_lerp(n0,n1,u);
}

f32 stb_perlin_ridge_noise3(f32 x, f32 y, f32 z,f32 lacunarity, f32 gain, f32 offset, i32 octaves, i32 x_wrap, i32 y_wrap, i32 z_wrap)
{
    i32 i;
    f32 frequency = 1.0f;
    f32 prev = 1.0f;
    f32 amplitude = 0.5f;
    f32 sum = 0.0f;

    for (i = 0; i < octaves; i++) {
        f32 r = (f32)(stb_perlin_noise3(x*frequency,y*frequency,z*frequency,x_wrap,y_wrap,z_wrap));
        r = r<0 ? -r : r; // fabs()
        r = offset - r;
        r = r*r;
        sum += r*amplitude*prev;
        prev = r;
        frequency *= lacunarity;
        amplitude *= gain;
    }
    return sum;
}

f32 stb_perlin_fbm_noise3(f32 x, f32 y, f32 z, f32 lacunarity, f32 gain, i32 octaves, i32 x_wrap, i32 y_wrap, i32 z_wrap)
{
    i32 i;
    f32 frequency = 1.0f;
    f32 amplitude = 1.0f;
    f32 sum = 0.0f;

    for (i = 0; i < octaves; i++) {
        sum += stb_perlin_noise3(x*frequency,y*frequency,z*frequency,x_wrap,y_wrap,z_wrap)*amplitude;
        frequency *= lacunarity;
        amplitude *= gain;
    }
    return sum;
}

f32 stb_perlin_turbulence_noise3(f32 x, f32 y, f32 z, f32 lacunarity, f32 gain, i32 octaves, i32 x_wrap, i32 y_wrap, i32 z_wrap)
{
    i32 i;
    f32 frequency = 1.0f;
    f32 amplitude = 1.0f;
    f32 sum = 0.0f;

    for (i = 0; i < octaves; i++) {
        f32 r = stb_perlin_noise3(x*frequency,y*frequency,z*frequency,x_wrap,y_wrap,z_wrap)*amplitude;
        r = r<0 ? -r : r; // fabs()
        sum += r;
        frequency *= lacunarity;
        amplitude *= gain;
    }
    return sum;
}

#endif  // STB_PERLIN_IMPLEMENTATION

/*
   ------------------------------------------------------------------------------
   This software is available under 2 licenses -- choose whichever you prefer.
   ------------------------------------------------------------------------------
   ALTERNATIVE A - MIT License
   Copyright (c) 2017 Sean Barrett
   Permission is hereby granted, free of charge, to any person obtaining a copy of 
   this software and associated documentation files (the "Software"), to deal in 
   the Software without restriction, including without limitation the rights to 
   use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
   of the Software, and to permit persons to whom the Software is furnished to do 
   so, subject to the following conditions:
   The above copyright notice and this permission notice shall be included in all 
   copies or substantial portions of the Software.
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
   SOFTWARE.
   ------------------------------------------------------------------------------
   ALTERNATIVE B - Public Domain (www.unlicense.org)
   This is free and unencumbered software released into the public domain.
   Anyone is free to copy, modify, publish, use, compile, sell, or distribute this 
   software, either in source code form or as a compiled binary, for any purpose, 
   commercial or non-commercial, and by any means.
   In jurisdictions that recognize copyright laws, the author or authors of this 
   software dedicate any and all copyright interest in the software to the public 
   domain. We make this dedication for the benefit of the public at large and to 
   the detriment of our heirs and successors. We intend this dedication to be an 
   overt act of relinquishment in perpetuity of all present and future rights to 
   this software under copyright law.
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
   AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
   WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
   ------------------------------------------------------------------------------
   */
