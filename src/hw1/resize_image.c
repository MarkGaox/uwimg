#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    int ix = (int)(x + 0.5f);
    int iy = (int)(y + 0.5f);

    return get_pixel(im, ix, iy, c);
}

image nn_resize(image im, int w, int h)
{
    image rim = make_image(w, h, 3);
    
    float ax = (float)im.w / (float)w;
    float bx = ax * 0.5f - 0.5f;
    float ay = (float)im.h / (float)h;
    float by = ay * 0.5f - 0.5f;
    for (int c = 0; c < 3; c++) {
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                float x = ax * i + bx;
                float y = ay * j + by;
                set_pixel(rim, i, j, c, nn_interpolate(im, x, y, c));
            }
        }
    }
    return rim;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    float V1 = get_pixel(im, (int)x, (int)y, c);
    float V2 = get_pixel(im, (int)(x + 1), (int)y, c);
    float V3 = get_pixel(im, (int)x, (int)(y + 1), c);
    float V4 = get_pixel(im, (int)(x + 1), (int)(y + 1), c);

    float d1 = x - (float)((int)x);
    float d2 = 1 - d1;
    float d3 = y - (float)((int)y);
    float d4 = 1 - d3;

    float q1 = V1 * d2 + V2 * d1;
    float q2 = V3 * d2 + V4 * d1;
    float q = q1 * d4 +  q2 * d3;

    return q;
}

image bilinear_resize(image im, int w, int h)
{
    image rim = make_image(w, h, 3);
    
    float ax = (float)im.w / (float)w;
    float bx = ax * 0.5f - 0.5f;
    float ay = (float)im.h / (float)h;
    float by = ay * 0.5f - 0.5f;
    for (int c = 0; c < 3; c++) {
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                float x = ax * i + bx;
                float y = ay * j + by;
                set_pixel(rim, i, j, c, bilinear_interpolate(im, x, y, c));
            }
        }
    }
    return rim;
}

