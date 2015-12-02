// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

/// This file contains the raw image data for images used by the standard library.
#include <GTGE/GUI/GUIStandardLibrary.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4125)   // decimal digit terminates octal escape sequence
#elif defined(__GNUC__)
#endif

/* GIMP RGBA C-Source image dump (tick-12x12.c) */

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[12 * 12 * 4 + 1];
} tick_12x12 = {
  12, 12, 4,
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\377\377\377p\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\222\377\377\377\377\377\377\377"
  "p\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377"
  "\377M\377\377\377\350\377\377\377\353\377\377\377<\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\27\377\377\377\276\377\377"
  "\377\374\377\377\377\203\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\377\377\377]\377\377\377\367\377\377\377\332\377\377\377"
  "*\0\0\0\0\0\0\0\0\377\377\377m\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377"
  "\27\377\377\377\304\377\377\377\374\377\377\377x\0\0\0\0\0\0\0\0\377\377"
  "\377m\377\377\377\377\377\377\377\237\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377"
  "\203\377\377\377\373\377\377\377\304\377\377\377\27\0\0\0\0\0\0\0\0\377\377"
  "\377<\377\377\377\355\377\377\377\377\377\377\377\222\0\0\0\0\377\377\377"
  "<\377\377\377\353\377\377\377\350\377\377\377M\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\377\377\377T\377\377\377\320\377\377\377\377\377\377\377\233\377\377"
  "\377\222\377\377\377\377\377\377\377\222\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\377\377\377<\377\377\377\324\377\377\377\377\377\377\377"
  "\367\377\377\377\353\377\377\377<\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\377\377\3774\377\377\377\340\377\377\377\377\377\377\377"
  "\271\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\377\377\377#\377\377\377\272\377\377\377\14\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0"
};

/* GIMP RGBA C-Source image dump (arrow-down-small.c) */

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[7 * 4 * 4 + 1];
} arrow_down_small = {
  7, 4, 4,
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377Z\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "G\0\0\0\0\377\377\377W\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377W\0\0\0\0\0\0\0\0\0\0\0\0\377\377\3771\377\377\377\377\377\377\377"
  "+\0\0\0\0\0\0\0\0",
};

/* GIMP RGBA C-Source image dump (arrow-up-small.c) */

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[7 * 4 * 4 + 1];
} arrow_up_small = {
  7, 4, 4,
  "\0\0\0\0\0\0\0\0\377\377\3771\377\377\377\377\377\377\377+\0\0\0\0\0\0\0"
  "\0\0\0\0\0\377\377\377W\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377W\0\0\0\0\377\377\377Z\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377G\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377",
};

/* GIMP RGBA C-Source image dump (arrow-left-small.c) */

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[4 * 7 * 4 + 1];
} arrow_left_small = {
  4, 7, 4,
  "\0\0\0\0\0\0\0\0\377\377\377G\377\377\377\377\0\0\0\0\377\377\377W\377\377"
  "\377\377\377\377\377\377\377\377\377+\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\3771\377\377\377\377\377\377\377\377\377\377\377\377\0\0\0\0"
  "\377\377\377W\377\377\377\377\377\377\377\377\0\0\0\0\0\0\0\0\377\377\377"
  "Z\377\377\377\377",
};

/* GIMP RGBA C-Source image dump (arrow-right-small.c) */

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[4 * 7 * 4 + 1];
} arrow_right_small = {
  4, 7, 4,
  "\377\377\377\377\377\377\377G\0\0\0\0\0\0\0\0\377\377\377\377\377\377\377"
  "\377\377\377\377W\0\0\0\0\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377+\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\3771\377\377"
  "\377\377\377\377\377\377\377\377\377W\0\0\0\0\377\377\377\377\377\377\377"
  "Z\0\0\0\0\0\0\0\0",
};

/* GIMP RGBA C-Source image dump (arrow-right-medium.c) */

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[6 * 13 * 4 + 1];
} arrow_right_medium = {
  6, 13, 4,
  "\377\377\377\361\377\377\377/\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377"
  "\377\377\377\377\342\377\377\377\35\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\377"
  "\377\377\377\377\377\377\377\333\377\377\377\25\0\0\0\0\0\0\0\0\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\302\377\377\377\10\0\0\0"
  "\0\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\252\377\377\377\2\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\212\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\373\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\233\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\252\377\377\377\2\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\272\377\377\377\6\0\0\0\0\377\377\377\377\377"
  "\377\377\377\377\377\377\316\377\377\377\16\0\0\0\0\0\0\0\0\377\377\377\356"
  "\377\377\377\341\377\377\377\35\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\356\377"
  "\377\377+\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
};


/* GIMP RGBA C-Source image dump (arrow-right-medium.c) */

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[8 * 15 * 4 + 1];
} arrow_right_large = {
  8, 15, 4,
  "\377\377\377\361\377\377\377>\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\377\377\377\374\377\377\377\347\377\377\377=\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\377\377\377\374\377\377\377\377\377\377\377\344\377\377"
  "\377D\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\374\377\377\377\377\377"
  "\377\377\377\377\377\377\344\377\377\377C\0\0\0\0\0\0\0\0\0\0\0\0\377\377"
  "\377\374\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\344"
  "\377\377\377C\0\0\0\0\0\0\0\0\377\377\377\374\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\344\377\377\377B\0\0\0\0"
  "\377\377\377\374\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\347\377\377\377B\377\377\377\374\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\345\377\377\377\374\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\347\377\377\3771\377\377\377\374\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\350\377\377\3771\0\0\0\0\377\377\377"
  "\374\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\350\377"
  "\377\3771\0\0\0\0\0\0\0\0\377\377\377\374\377\377\377\377\377\377\377\377"
  "\377\377\377\350\377\377\3772\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\374\377"
  "\377\377\377\377\377\377\350\377\377\3772\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\377\377\377\374\377\377\377\353\377\377\377=\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\377\377\377\360\377\377\377=\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0",
};





/* GIMP RGBA C-Source image dump (search-16.c) */

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[16 * 16 * 4 + 1];
} search_16x16 = {
  16, 16, 4,
  "\0\0\0\0\0\0\0\0\377\377\377\25\377\377\377a\377\377\377\311\377\377\377"
  "\362\377\377\377\342\377\377\377\220\377\377\377(\377\377\377\3\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377>\377\377\377\336"
  "\377\377\377\315\377\377\377h\377\377\377D\377\377\377S\377\377\377\224\377"
  "\377\377\371\377\377\377\203\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\377\377\377\25\377\377\377\336\377\377\377\221\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\377\377\377\1\377\377\377)\377\377\377\357\377\377\377g\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377a\377\377\377\315\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377q\377\377\377\360"
  "\377\377\377\27\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\311\377\377\377"
  "h\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\26"
  "\377\377\377\375\377\377\377b\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377"
  "\362\377\377\377D\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\377\377\377\5\377\377\377\352\377\377\377{\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\377\377\377\342\377\377\377T\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\377\377\377\10\377\377\377\365\377\377\377p\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\377\377\377\220\377\377\377\224\377\377\377\1\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377D\377\377\377\371\377"
  "\377\377D\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377(\377\377\377\371\377"
  "\377\377)\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\17\377\377"
  "\377\271\377\377\377\263\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377"
  "\377\3\377\377\377\203\377\377\377\357\377\377\377q\377\377\377\26\377\377"
  "\377\5\377\377\377\10\377\377\377D\377\377\377\271\377\377\377\377\377\377"
  "\377[\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377"
  "g\377\377\377\360\377\377\377\375\377\377\377\352\377\377\377\365\377\377"
  "\377\371\377\377\377\270\377\377\377\224\377\377\377\375\377\377\377U\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\27\377\377"
  "\377b\377\377\377{\377\377\377p\377\377\377D\0\0\0\0\0\0\0\0\377\377\377"
  "\224\377\377\377\377\377\377\377\246\377\377\377\37\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\377\377\377\26\377\377\377\341\377\377\377\377\377\377\377\322\377\377"
  "\377%\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377N\377\377\377\356\377\377\377\377"
  "\377\377\377\315\377\377\377\23\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\\\377"
  "\377\377\367\377\377\377\377\377\377\377\277\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\377\377\3776\377\377\377\360\377\377\377\324",
};


/* GIMP RGBA C-Source image dump (square-8x8.c) */

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[8 * 8 * 4 + 1];
} square_8x8 = {
  8, 8, 4,
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377",
};

/* GIMP RGBA C-Source image dump (square-12x12.c) */

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[12 * 12 * 4 + 1];
} square_12x12 = {
  12, 12, 4,
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377",
};






/* GIMP RGBA C-Source image dump (folder-12x8.c) */

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[12 * 8 * 4 + 1];
} folder_12x8 = {
  12, 8, 4,
  "\304\257\0\377\304\257\0\377\304\257\0\377\304\257\0\377\304\257\0\377\304"
  "\257\0@\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\304\257\0\377\0\0"
  "\0\0\0\0\0\0\0\0\0\0\304\257\0@\304\257\0\377\304\257\0@\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\304\257\0\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\304"
  "\257\0@\304\257\0\377\304\257\0\377\304\257\0\377\304\257\0\377\304\257\0"
  "\377\304\257\0\377\304\257\0\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\304\257\0\377\304\257\0\377\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\304\257\0\377\304\257\0\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\304\257\0\377\304\257\0\377\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\304\257\0\377\304\257\0\377\304\257\0\377\304\257\0\377\304\257\0\377"
  "\304\257\0\377\304\257\0\377\304\257\0\377\304\257\0\377\304\257\0\377\304"
  "\257\0\377\304\257\0\377\304\257\0\377",
};

/* GIMP RGBA C-Source image dump (file-8x12.c) */

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[8 * 12 * 4 + 1];
} file_8x12 = {
  8, 12, 4,
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377g\0\0\0\0\0\0\0\0\377\377\377\377\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\377\377\377\377\377\377\377\200\0\0\0\0\377\377\377\377\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\377\377\377\377\200\377"
  "\377\377\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377"
  "\377\377\377\377\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377"
  "\377\377\377\377\377\377\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\377\377\377\377\377\377\377\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\377\377\377\377\377\377\377\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\377\377\377\377\377\377\377\377\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\377\377\377\377\377\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\377\377\377\377\377\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377",
};



/* GIMP RGBA C-Source image dump (cross-8x8.c) */

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[8 * 8 * 4 + 1];
} cross_8x8 = {
  8, 8, 4,
  "\377\377\377\377\377\377\377\214\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\1\377"
  "\377\377\220\377\377\377\377\377\377\377\221\377\377\377\377\377\377\377"
  "\214\0\0\0\0\377\377\377\1\377\377\377\221\377\377\377\377\377\377\377\200"
  "\377\377\377\1\377\377\377\221\377\377\377\377\377\377\377\214\377\377\377"
  "\221\377\377\377\377\377\377\377\202\0\0\0\0\0\0\0\0\377\377\377\1\377\377"
  "\377\221\377\377\377\377\377\377\377\377\377\377\377\203\0\0\0\0\0\0\0\0"
  "\0\0\0\0\377\377\377\1\377\377\377\222\377\377\377\377\377\377\377\377\377"
  "\377\377\214\0\0\0\0\0\0\0\0\377\377\377\1\377\377\377\223\377\377\377\377"
  "\377\377\377\206\377\377\377\221\377\377\377\377\377\377\377\214\0\0\0\0"
  "\377\377\377\224\377\377\377\377\377\377\377\210\0\0\0\0\377\377\377\1\377"
  "\377\377\221\377\377\377\377\377\377\377\215\377\377\377\377\377\377\377"
  "\211\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\1\377\377\377\221\377\377\377\376",
};


/* GIMP RGBA C-Source image dump (plus.c) */

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[8 * 8 * 4 + 1];
} plus_8x8 = {
  8, 8, 4,
  "\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\377\377\377\377\377\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\377\377\377\377\377\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\377\377\377\377\377"
  "\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\0\0\0\0"
  "\0\0\0\0\0\0\0\0\377\377\377\377\377\377\377\377\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\377\377\377\377\377\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\377\377\377\377\377\0\0\0\0"
  "\0\0\0\0\0\0\0\0",
};


/* GIMP RGBA C-Source image dump (minus.c) */

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[8 * 8 * 4 + 1];
} minus_8x8 = {
  8, 8, 4,
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
};


/* GIMP RGBA C-Source image dump (pause-8x8.c) */

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[8 * 8 * 4 + 1];
} pause_8x8 = {
  8, 8, 4,
  "\377\377\377\377\377\377\377\377\377\377\377\377\0\0\0\0\0\0\0\0\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\0\0\0\0\0\0\0\0\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\0\0\0\0\0\0\0\0"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\0\0\0\0\0\0\0\0\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\0\0\0\0"
  "\0\0\0\0\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\0\0\0\0\0\0\0\0\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\0\0\0\0\0\0\0\0\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\0\0\0\0\0\0\0\0\377\377\377\377"
  "\377\377\377\377\377\377\377\377",
};

/* GIMP RGBA C-Source image dump (pause-12x12.c) */

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[12 * 12 * 4 + 1];
} pause_12x12 = {
  12, 12, 4,
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\0\0\0\0\0\0\0\0\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\0\0\0\0\0\0\0\0\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\0\0\0\0\0\0\0\0\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\0\0\0\0\0\0\0\0\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\0\0\0\0"
  "\0\0\0\0\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\0\0\0\0\0\0\0\0\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\0\0\0\0\0\0\0\0"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\0\0\0\0\0\0\0\0\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\0\0\0\0\0\0\0\0\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\0\0\0\0\0\0\0\0\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\0\0\0\0\0\0\0\0\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\0\0\0\0\0\0\0\0\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
  "\377\377\377\377\377\377",
};

/* GIMP RGBA C-Source image dump (direction-arrow-up-8x8.c) */

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[8 * 8 * 4 + 1];
} direction_arrow_up_8x8 = {
  8, 8, 4,
  "\0\0\0\0\0\0\0\0\377\377\377\23\377\377\377\303\377\377\377\304\377\377\377"
  "\23\0\0\0\0\0\0\0\0\0\0\0\0\377\377\377\"\377\377\377\330\377\377\377\371"
  "\377\377\377\370\377\377\377\332\377\377\377$\0\0\0\0\377\377\377>\377\377"
  "\377\356\377\377\377\351\377\377\377\346\377\377\377\346\377\377\377\350"
  "\377\377\377\356\377\377\377A\377\377\377\332\377\377\377\335\377\377\377"
  "(\377\377\377\343\377\377\377\331\377\377\377&\377\377\377\333\377\377\377"
  "\314\377\377\377*\377\377\377\26\377\377\377\14\377\377\377\335\377\377\377"
  "\331\0\0\0\0\377\377\377\23\377\377\377\35\0\0\0\0\0\0\0\0\0\0\0\0\377\377"
  "\377\340\377\377\377\340\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\377\377\377\335\377\377\377\335\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\377\377\377\331\377\377\377\343\0\0\0\0\0\0\0\0\0\0\0\0",
};










namespace GT
{
    bool GUIStandardLibrary::LoadImages(GUIServer &server)
    {
        server.RegisterImage("std-tick-12x12", tick_12x12.width, tick_12x12.height, tick_12x12.bytes_per_pixel, tick_12x12.pixel_data);

        server.RegisterImage("std-arrow-down",  arrow_down_small.width,  arrow_down_small.height,  arrow_down_small.bytes_per_pixel,  arrow_down_small.pixel_data);
        server.RegisterImage("std-arrow-up",    arrow_up_small.width,    arrow_up_small.height,    arrow_up_small.bytes_per_pixel,    arrow_up_small.pixel_data);
        server.RegisterImage("std-arrow-left",  arrow_left_small.width,  arrow_left_small.height,  arrow_left_small.bytes_per_pixel,  arrow_left_small.pixel_data);
        server.RegisterImage("std-arrow-right", arrow_right_small.width, arrow_right_small.height, arrow_right_small.bytes_per_pixel, arrow_right_small.pixel_data);

        server.RegisterImage("std-arrow-right-medium", arrow_right_medium.width, arrow_right_medium.height, arrow_right_medium.bytes_per_pixel, arrow_right_medium.pixel_data);
        server.RegisterImage("std-arrow-right-large",  arrow_right_large.width,  arrow_right_large.height,  arrow_right_large.bytes_per_pixel,  arrow_right_large.pixel_data);

        server.RegisterImage("std-search-16x16", search_16x16.width, search_16x16.height, search_16x16.bytes_per_pixel, search_16x16.pixel_data);

        server.RegisterImage("std-square-8x8",   square_8x8.width,   square_8x8.height,   square_8x8.bytes_per_pixel,   square_8x8.pixel_data);
        server.RegisterImage("std-square-12x12", square_12x12.width, square_12x12.height, square_12x12.bytes_per_pixel, square_12x12.pixel_data);

        server.RegisterImage("folder-icon", folder_12x8.width, folder_12x8.height, folder_12x8.bytes_per_pixel, folder_12x8.pixel_data);
        server.RegisterImage("file-icon",   file_8x12.width,   file_8x12.height,   file_8x12.bytes_per_pixel,   file_8x12.pixel_data);

        server.RegisterImage("std-cross",   cross_8x8.width,   cross_8x8.height,   cross_8x8.bytes_per_pixel,   cross_8x8.pixel_data);
        server.RegisterImage("std-plus",    plus_8x8.width,    plus_8x8.height,    plus_8x8.bytes_per_pixel,    plus_8x8.pixel_data);
        server.RegisterImage("std-minus",   minus_8x8.width,   minus_8x8.height,   minus_8x8.bytes_per_pixel,   minus_8x8.pixel_data);

        server.RegisterImage("std-pause-8x8",   pause_8x8.width,   pause_8x8.height,   pause_8x8.bytes_per_pixel,   pause_8x8.pixel_data);
        server.RegisterImage("std-pause-12x12", pause_12x12.width, pause_12x12.height, pause_12x12.bytes_per_pixel, pause_12x12.pixel_data);

        server.RegisterImage("std-direction-arrow-up-8x8", direction_arrow_up_8x8.width, direction_arrow_up_8x8.height, direction_arrow_up_8x8.bytes_per_pixel, direction_arrow_up_8x8.pixel_data);

        return true;
    }
}


#if defined(_MSC_VER)
    #pragma warning(pop)
#elif defined(__GNUC__)
#endif
