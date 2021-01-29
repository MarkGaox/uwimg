from uwimg import *

im = load_image("data/dog.jpg")
f = make_box_filter(7)
blur = convolve_image(im, f, 1)
save_image(blur, "dog-box7")

fun = colorize_sobel(im)
save_image(fun, "dog-fun")

bear = load_image("data/bear.jpg")
tuzi = load_image("data/tuzi.jpg")
f = make_gaussian_filter(2)
lfreq_bear = convolve_image(bear, f, 1)
lfreq_tuzi = convolve_image(tuzi, f, 1)
hfreq_bear = bear - lfreq_bear
hfreq_tuzi = tuzi - lfreq_tuzi

tb = hfreq_bear + hfreq_tuzi
bt = lfreq_tuzi + lfreq_bear
save_image(tb, "tb")
save_image(bt, "bt")
