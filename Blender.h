#ifndef Blender_DEFINED
#define Blender_DEFINED

class Blender {
    public:
        GPixel blend(const GPixel& src, const GPixel& dst, const GBlendMode& mode);
        void blendModePrint(const GBlendMode& mode);
};

#endif