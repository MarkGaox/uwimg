#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    double sum = 0;

    for (int j = 0; j < im.h; j++) {
        for (int i = 0; i < im.w; i++) {
            sum += get_pixel(im, i, j, 0);
        }
    }

    for (int j = 0; j < im.h; j++) {
        for (int i = 0; i < im.w; i++) {
            double cur = get_pixel(im, i, j, 0);
            set_pixel(im, i, j, 0, cur / sum);
        }
    }
    
}

image make_box_filter(int w)
{
    image img = make_image(w, w, 1);

    for (int j = 0; j < w; j++) {
        for (int i = 0; i < w; i++) {
            set_pixel(img, i, j, 0, 1.0f);
        }
    }

    l1_normalize(img);

    return img;
}

image convolve_image(image im, image filter, int preserve)
{
    assert(im.c == filter.c || filter.c == 1);
    int sumChan = preserve != 1;
    int filterOneChan = filter.c == 1;

    image img = make_image(im.w, im.h, sumChan ? 1 : im.c);
    
    int offx = filter.w / 2;
    int offy = filter.h / 2;
    for (int c = 0; c < im.c; c++) {
        int filterC = filterOneChan ? 0 : c;
        int targetC = sumChan ? 0 : c;
        for (int imj = 0; imj < im.h; imj++) {
            for (int imi = 0; imi < im.w; imi++) {
                float sum = 0;
                for (int j = 0; j < filter.h; j++) {
                    for (int i = 0; i < filter.w; i++) {
                        float weight = get_pixel(filter, i, j, filterC);
                        sum += weight * get_pixel(im, imi - offx + i, imj - offy + j, c);
                    }
                }
                if (c != 0) {
                    sum += get_pixel(img, imi, imj, targetC);
                }
                set_pixel(img, imi, imj, targetC, sum);
            }
        }
    }

    return img;
}

image make_highpass_filter()
{
    image img = make_box_filter(3);
    set_pixel(img, 0, 0, 0, 0.0f);
    set_pixel(img, 2, 0, 0, 0.0f);
    set_pixel(img, 0, 2, 0, 0.0f);
    set_pixel(img, 2, 2, 0, 0.0f);
    set_pixel(img, 1, 0, 0, -1.0f);
    set_pixel(img, 0, 1, 0, -1.0f);
    set_pixel(img, 2, 1, 0, -1.0f);
    set_pixel(img, 1, 2, 0, -1.0f);
    set_pixel(img, 1, 1, 0, 4.0f);
    return img;
}

image make_sharpen_filter()
{
    image img = make_box_filter(3);
    set_pixel(img, 0, 0, 0, 0.0f);
    set_pixel(img, 2, 0, 0, 0.0f);
    set_pixel(img, 0, 2, 0, 0.0f);
    set_pixel(img, 2, 2, 0, 0.0f);
    set_pixel(img, 1, 0, 0, -1.0f);
    set_pixel(img, 0, 1, 0, -1.0f);
    set_pixel(img, 2, 1, 0, -1.0f);
    set_pixel(img, 1, 2, 0, -1.0f);
    set_pixel(img, 1, 1, 0, 5.0f);
    return img;
}

image make_emboss_filter()
{
    image img = make_box_filter(3);
    set_pixel(img, 0, 0, 0, -2.0f);
    set_pixel(img, 2, 0, 0, 0.0f);
    set_pixel(img, 0, 2, 0, 0.0f);
    set_pixel(img, 2, 2, 0, 2.0f);
    set_pixel(img, 1, 0, 0, -1.0f);
    set_pixel(img, 0, 1, 0, -1.0f);
    set_pixel(img, 2, 1, 0, 1.0f);
    set_pixel(img, 1, 2, 0, 1.0f);
    set_pixel(img, 1, 1, 0, 1.0f);
    return img;;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: Blur, sharpen, and gaussian. Because we want to keep the color of the image when using the image.

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: When the sum of the filter is between -1 and 1 we don't have to do any post-processing for the filters.

image make_gaussian_filter(float sigma)
{
    int size = sigma * 6;
    size = size - (size % 2) + 1;
    int half = size / 2;

    image result = make_image(size, size, 1);

    for (int j = -half; j <= half; j++) {
        for (int i = -half; i <= half; i++) {
            float x = - ((float)(i * i + j * j))/(2 * sigma * sigma);
            set_pixel(result, i + half, j + half, 0, exp(x));
        }
    }

    l1_normalize(result);
    
    return result;
}

image add_image(image a, image b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c);
    image result = make_image(a.w, a.h, a.c);
    for (int i = 0; i < a.c; i++) {
        for (int j = 0; j < a.w; j++) {
            for (int k = 0; k < a.h; k++) {
                set_pixel(result, j, k, i, get_pixel(a, j, k, i) + get_pixel(b, j, k, i));
            }
        }
    }
    return result;
}

image sub_image(image a, image b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c);
    image result = make_image(a.w, a.h, a.c);
    for (int i = 0; i < a.c; i++) {
        for (int j = 0; j < a.w; j++) {
            for (int k = 0; k < a.h; k++) {
                set_pixel(result, j, k, i, get_pixel(a, j, k, i) - get_pixel(b, j, k, i));
            }
        }
    }
    return result;
}

image make_gx_filter()
{
    image result = make_box_filter(3);
    set_pixel(result, 0, 0, 0, -1);
    set_pixel(result, 1, 0, 0, 0);
    set_pixel(result, 2, 0, 0, 1);
    set_pixel(result, 0, 1, 0, -2);
    set_pixel(result, 1, 1, 0, 0);
    set_pixel(result, 2, 1, 0, 2);
    set_pixel(result, 0, 2, 0, -1);
    set_pixel(result, 1, 2, 0, 0);
    set_pixel(result, 2, 2, 0, 1);
    return result;
}

image make_gy_filter()
{
    image result = make_box_filter(3);
    set_pixel(result, 0, 0, 0, -1);
    set_pixel(result, 1, 0, 0, -2);
    set_pixel(result, 2, 0, 0, -1);
    set_pixel(result, 0, 1, 0, 0);
    set_pixel(result, 1, 1, 0, 0);
    set_pixel(result, 2, 1, 0, 0);
    set_pixel(result, 0, 2, 0, 1);
    set_pixel(result, 1, 2, 0, 2);
    set_pixel(result, 2, 2, 0, 1);
    return result;
}

void feature_normalize(image im)
{
    // find range
    float max, min = im.data[0];
    for (int i = 0; i < im.c; i++) {
        for (int j = 0; j < im.w; j++) {
            for (int k = 0; k < im.h; k++) {
               max = MAX(max, get_pixel(im, j, k, i));
               min = MIN(min, get_pixel(im, j, k, i));
            }
        }
    }

    float range = max - min;
    for (int i = 0; i < im.c; i++) {
        for (int j = 0; j < im.w; j++) {
            for (int k = 0; k < im.h; k++) {
                if (range == 0) {
                    set_pixel(im, j, k, i, 0.0);
                } else {
                    set_pixel(im, j, k, i, (get_pixel(im, j, k, i) - min) / range);
                }
            }
        }
    }
}

image *sobel_image(image im)
{
    image *result = calloc(2, sizeof(image));
    // gradient magnitude. G = sqrt(G_x^2 + G_y^2)
    image gx = convolve_image(im, make_gx_filter(), 0);
    image gy = convolve_image(im, make_gy_filter(), 0);
    image magnitude = make_image(im.w, im.h, 1);
    image direction = make_image(im.w, im.h, 1);

    for (int j = 0; j < im.h; j++) {
        for (int k = 0; k < im.w; k++) {
            float x = get_pixel(gx, k, j, 0);
            float y = get_pixel(gy, k, j, 0);
            set_pixel(magnitude, k, j, 0, sqrtf(x * x + y * y));
            set_pixel(direction, k, j, 0, atan2f(y, x));
        }
    }

    result[0] = magnitude;
    result[1] = direction;
    free_image(gx);
    free_image(gy);
    return result;
}

image colorize_sobel(image im)
{
    image *sobel = sobel_image(im);
    image mag = sobel[0];
    image dir = sobel[1];
    feature_normalize(mag);
    feature_normalize(dir);
    image result = make_image(im.w, im.h, im.c);

    for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
            set_pixel(result, i, j, 0, get_pixel(dir, i, j, 0));
        }
    }

    for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
            set_pixel(result, i, j, 1, get_pixel(mag, i, j, 0));
            set_pixel(result, i, j, 2, get_pixel(mag, i, j, 0));
        }
    }

    return result;
}
