#include "GPixel.h"
#include "GPaint.h"
#include "Blender.h"

GPixel Blender::blend(const GPixel& src, const GPixel& dst, const GBlendMode& mode) {
        int src_a = GPixel_GetA(src);
        int src_r = GPixel_GetR(src);
        int src_g = GPixel_GetG(src);
        int src_b = GPixel_GetB(src);

        int dst_a = GPixel_GetA(dst);
        int dst_r = GPixel_GetR(dst);
        int dst_g = GPixel_GetG(dst);
        int dst_b = GPixel_GetB(dst);

        int result_a;
        int result_r;
        int result_g;
        int result_b;
        switch(mode) {
            case GBlendMode::kClear: {
                result_a = 0;
                result_r = 0;
                result_g = 0;
                result_b = 0;
                break;
            }
            case GBlendMode::kSrc: {
                result_a = src_a;
                result_r = src_r;
                result_g = src_g;
                result_b = src_b;
                break;
            }
            case GBlendMode::kDst: {
                result_a = dst_a;
                result_r = dst_r;
                result_g = dst_g;
                result_b = dst_b;
                break;
            }
            case GBlendMode::kSrcOver: {
                result_a = src_a + ((255 - src_a) * dst_a + 127) / 255;
                result_r = src_r + ((255 - src_a) * dst_r + 127) / 255;
                result_g = src_g + ((255 - src_a) * dst_g + 127) / 255;
                result_b = src_b + ((255 - src_a) * dst_b + 127) / 255;
                break;
            }
            case GBlendMode::kDstOver: {
                result_a = dst_a + ((255 - dst_a) * src_a + 127) / 255;
                result_r = dst_r + ((255 - dst_a) * src_r + 127) / 255;
                result_g = dst_g + ((255 - dst_a) * src_g + 127) / 255;
                result_b = dst_b + ((255 - dst_a) * src_b + 127) / 255;
                break;
            }
            case GBlendMode::kSrcIn: {
                result_a = (dst_a * src_a) / 255;
                result_r = (dst_a * src_r) / 255;
                result_g = (dst_a * src_g) / 255;
                result_b = (dst_a * src_b) / 255;
                break;
            }
            case GBlendMode::kDstIn: {
                result_a = (src_a * dst_a) / 255;
                result_r = (src_a * dst_r) / 255;
                result_g = (src_a * dst_g) / 255;
                result_b = (src_a * dst_b) / 255;
                break;
            }
            case GBlendMode::kSrcOut: {
                result_a = ((255 - dst_a) * src_a + 127) / 255;
                result_r = ((255 - dst_a) * src_r + 127) / 255;
                result_g = ((255 - dst_a) * src_g + 127) / 255;
                result_b = ((255 - dst_a) * src_b + 127) / 255;
                break;
            }
            case GBlendMode::kDstOut: {
                result_a = ((255 - src_a) * dst_a + 127) / 255;
                result_r = ((255 - src_a) * dst_r + 127) / 255;
                result_g = ((255 - src_a) * dst_g + 127) / 255;
                result_b = ((255 - src_a) * dst_b + 127) / 255;
                break;
            }
            case GBlendMode::kSrcATop: {
                result_a = ((dst_a * src_a) / 255) + (((255 - src_a) * dst_a + 127) / 255);
                result_r = ((dst_a * src_r) / 255) + (((255 - src_a) * dst_r + 127) / 255);
                result_g = ((dst_a * src_g) / 255) + (((255 - src_a) * dst_g + 127) / 255);
                result_b = ((dst_a * src_b) / 255) + (((255 - src_a) * dst_b + 127) / 255);
                break;
            }
            case GBlendMode::kDstATop: {
                result_a = ((src_a * dst_a) / 255) + (((255 - dst_a) * src_a + 127) / 255);
                result_r = ((src_a * dst_r) / 255) + (((255 - dst_a) * src_r + 127) / 255);
                result_g = ((src_a * dst_g) / 255) + (((255 - dst_a) * src_g + 127) / 255);
                result_b = ((src_a * dst_b) / 255) + (((255 - dst_a) * src_b + 127) / 255);
                break;
            }
            case GBlendMode::kXor: {
                result_a = (((255 - src_a) * dst_a + 127) / 255) + (((255 - dst_a) * src_a + 127) / 255);
                result_r = (((255 - src_a) * dst_r + 127) / 255) + (((255 - dst_a) * src_r + 127) / 255);
                result_g = (((255 - src_a) * dst_g + 127) / 255) + (((255 - dst_a) * src_g + 127) / 255);
                result_b = (((255 - src_a) * dst_b + 127) / 255) + (((255 - dst_a) * src_b + 127) / 255);
                break;
            }
        }

        return GPixel_PackARGB(result_a, result_r, result_g, result_b);
    }

void Blender::blendModePrint(const GBlendMode& mode) {
    switch(mode) {
            case GBlendMode::kClear: {
                printf("kClear");
                break;
            }
            case GBlendMode::kSrc: {
                printf("kSrc");
                break;
            }
            case GBlendMode::kDst: {
                printf("kDst");
                break;
            }
            case GBlendMode::kSrcOver: {
                printf("kSrcOver");
                break;
            }
            case GBlendMode::kDstOver: {
                printf("kDstOver");
                break;
            }
            case GBlendMode::kSrcIn: {
                printf("kSrcIn");
                break;
            }
            case GBlendMode::kDstIn: {
                printf("kDstIn");
                break;
            }
            case GBlendMode::kSrcOut: {
                printf("kSrcOut");
                break;
            }
            case GBlendMode::kDstOut: {
                printf("kDstOut");
                break;
            }
            case GBlendMode::kSrcATop: {
                printf("kSrcATop");
                break;
            }
            case GBlendMode::kDstATop: {
                printf("kDstATop");
                break;
            }
            case GBlendMode::kXor: {
                printf("kXor");
                break;
            }
        }
}