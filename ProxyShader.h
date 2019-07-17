#ifndef ProxyShader_DEFINED
#define ProxyShader_DEFINED

#include "GPixel.h"
#include "GColor.h"
#include "GMatrix.h"
#include "GBitmap.h"
#include "GShader.h"

class ProxyShader : public GShader {
    public:
        ProxyShader(GShader* shader, const GMatrix& extraTransform) : fRealShader(shader), fExtraTransform(extraTransform) {}

        bool isOpaque() override {
            return fRealShader->isOpaque();
        }

        bool setContext(const GMatrix& ctm) override {
            GMatrix temp = GMatrix();
            temp.setConcat(ctm, fExtraTransform);
            return fRealShader->setContext(temp);
        }

        void shadeRow(int x, int y, int count, GPixel row[]) override {
           fRealShader->shadeRow(x, y, count, row);
        }



    private:
        GShader* fRealShader;
        GMatrix fExtraTransform;
};

#endif