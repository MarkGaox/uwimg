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
    // TODO
    return make_image(1,1,1);
}

image sub_image(image a, image b)
{
    // TODO
    return make_image(1,1,1);
}

image make_gx_filter()
{
    // TODO
    return make_image(1,1,1);
}

image make_gy_filter()
{
    // TODO
    return make_image(1,1,1);
}

void feature_normalize(image im)
{
    // TODO
}

image *sobel_image(image im)
{
    // TODO
    return calloc(2, sizeof(image));
}

image colorize_sobel(image im)
{
    // TODO
    return make_image(1,1,1);
}
