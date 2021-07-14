#include "prelude.hpp"

// NOTE: See `https://opengameart.org/content/ascii-bitmap-font-futuristic`.
// NOTE: See `https://opengameart.org/content/16x12-terminal-bitmap-font`.

#define STBI_ASSERT(condition) EXIT_IF(!(condition))

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG

#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wcast-align"
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wdouble-promotion"
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#pragma GCC diagnostic ignored "-Wimplicit-int-conversion"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"

#include "stb_image.h"

#pragma GCC diagnostic pop

#undef STB_IMAGE_IMPLEMENTATION
#undef STBI_ONLY_PNG

i32 main(i32 n, const char** args) {
    EXIT_IF(n < 2);
    i32 width;
    i32 height;
    i32 channels = STBI_rgb_alpha;
    u8* pixels = stbi_load(args[1], &width, &height, null, channels);
    i32 m = width * height * channels;
    {
        // NOTE: Set black pixels to fully transparent.
        for (i32 i = 0; i < m; i += channels) {
            if ((pixels[i] + pixels[i + 1] + pixels[i + 2]) == 0) {
                pixels[i + 3] = 0;
            }
        }
    }
    printf("#ifndef __SCENE_ASSETS_CODEGEN_H__\n"
           "#define __SCENE_ASSETS_CODEGEN_H__\n"
           "#include \"prelude.hpp\"\n");
    {
        printf("#define PIXEL_WIDTH %d\n"
               "#define PIXEL_HEIGHT %d\n"
               "#define PIXEL_CHANNELS %d\n"
               "static const u8 PIXEL_DATA"
               "[PIXEL_WIDTH * PIXEL_HEIGHT * PIXEL_CHANNELS] = {",
               width,
               height,
               channels);
        for (i32 i = 0; i < m; ++i) {
            printf("%hhu,", pixels[i]);
        }
        printf("};\n");
    }
    printf("#endif\n");
    stbi_image_free(pixels);
    return EXIT_SUCCESS;
}
