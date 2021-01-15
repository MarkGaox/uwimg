#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    // x col, y row, c channel

    if (x < 0) x = 0;
    if (x >= im.w) x = im.w - 1;
    if (y < 0) y = 0;
    if (y >= im.h) y = im.h - 1;
    if (c < 0) c = 0;
    if (c >= im.c) c = im.c - 1;

    int index = c * (im.w * im.h) + y * im.w + x;
    return im.data[index];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    if (x < 0 || x >= im.w) {
        return;
    }   
    if (y < 0 || y >= im.h) {
        return;
    }   
    if (c < 0 || c >= im.c) {
        return;
    }   
    int index = c * (im.w * im.h) + y * im.w + x;
    im.data[index] = v;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    for (int i = 0; i < im.c * im.h * im.w; i++) {
        copy.data[i] = im.data[i];
    }
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);

    int pixel_one_c = im.h * im.w;
    for (int i = 0; i < pixel_one_c; i++) {
        int rindex = i;
        int gindex = i + pixel_one_c;
        int bindex = i + 2 * pixel_one_c;

        gray.data[i] = 0.299 * im.data[rindex] + 0.587 * im.data[gindex] + 0.114 * im.data[bindex];
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    int pixel_one_c = im.h * im.w;
    for (int i = 0; i < pixel_one_c; i++) {
        int index = c * (im.h * im.w) + i;
        im.data[index] += v;
    }
}

void clamp_image(image im)
{
    for (int i = 0; i < im.c * im.h * im.w; i++) {
        if (im.data[i] < 0) {
            im.data[i] = 0;
        } else if (im.data[i] > 1) {
            im.data[i] = 1;
        }
    }
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

int max_RGB(float R, float G, float B, float C) {

    if (C < 0.001f) {
        return 0;
    } else if (R > G) {
        if (R > B) {
            return 1;
        } else {
            return 3;
        }
    } else {
        if (G > B) {
            return 2;
        } else {
            return 3;
        }
    }

}

/**
 * Matrix for converting rgb to hue:
 * | 1      0          0 |
 * |-0.5  sqrt(3)/2    0 |
 * |-0.5 -sqrt(3)/2    0 |
**/
float single_rgb_to_hsv(float *R, float *G, float *B) {
    float V = MAX(MAX(*R, *G), *B);
    float m = MIN(MIN(*R, *G), *B);
    float C = V - m;
    float S = 0;
    if (V > 0.001) {
        S = C / V;
    }

    int HMode = max_RGB(*R, *G, *B, C);
    float HPrime = 0.00f;
    switch (HMode) {
    case 0:
        HPrime = 0;
        break;
    case 1:
        HPrime = ((*G) - (*B)) / C;
        break;
    case 2:
        HPrime = ((*B) - (*R)) / C + 2.0f;
        break;
    case 3:
        HPrime = ((*R) - (*G)) / C + 4.0f;
        break;
    default:
        HPrime = 0;
    }

    float H = HPrime / 6.0f;
    if (HPrime < 0.0f) {
        H += 1.0f;
    }
    
    *R = H;
    *G = S;
    *B = V;
}

// Note that at 100% saturation, there is always a color at
// max and at 0
void single_hsv_to_rgb(float *H, float *S, float *V) {
    *H *= 360.0f;

    float maxCol = *V;
    float minCol = maxCol * (1 - *S);

    float midScale = fmod(*H, 120);
    if (midScale > 60.0) {
        midScale = 120.0f - midScale;
    }
    midScale /= 60.0f;
    float midCol = (maxCol - minCol) * midScale + minCol;
    
    // There are 6 choices of assigning the three value.
    if ( *H <= 120.0) {
        *V = minCol;
        if (*H < 60.0) {
            *H = maxCol;
            *S = midCol;
        } else {
            *S = maxCol;
            *H = midCol;
        }
    } else if (*H <= 240.0) {
        if (*H < 180.0) {
            *S = maxCol;
            *V = midCol;
        } else {
            *V = maxCol;
            *S = midCol;
        }
        *H = minCol;
    } else {
        *S = minCol;
        if (*H < 300.0) {
            *V = maxCol;
            *H = midCol;
        } else {
            *H = maxCol;
            *V = midCol;
        }
    }
}

void rgb_to_hsv(image im)
{
    int Mh = im.h;
    int Mw = im.w;

    for (int h = 0; h < Mh; h++) {
        for (int w = 0; w < Mw; w++) {
            float *R =  im.data + 0 * (im.w * im.h) + h * im.w + w;
            float *G =  im.data + 1 * (im.w * im.h) + h * im.w + w;
            float *B =  im.data + 2 * (im.w * im.h) + h * im.w + w;
            single_rgb_to_hsv(R, G, B);
        }
    }
}

void hsv_to_rgb(image im)
{
    int Mh = im.h;
    int Mw = im.w;

    for (int h = 0; h < Mh; h++) {
        for (int w = 0; w < Mw; w++) {
            float *H =  im.data + 0 * (im.w * im.h) + h * im.w + w;
            float *S =  im.data + 1 * (im.w * im.h) + h * im.w + w;
            float *V =  im.data + 2 * (im.w * im.h) + h * im.w + w;
            single_hsv_to_rgb(H, S, V);
        }
    }
}
