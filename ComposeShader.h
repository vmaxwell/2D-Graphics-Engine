#ifndef ComposeShader_DEFINED
#define ComposeShader_DEFINED

#include "GPixel.h"
#include "GColor.h"
#include "GMatrix.h"
#include "GBitmap.h"
#include "GShader.h"

class ComposeShader : public GShader {
    public:
        ComposeShader(GShader* shader1, GShader* shader2) {
            fShader1 = shader1;
            fShader2 = shader2;
        }

        bool isOpaque() override {
            return fShader1->isOpaque() && fShader2->isOpaque();
        }

        bool setContext(const GMatrix& ctm) override {
            return fShader1->setContext(ctm) && fShader2->setContext(ctm);
        }

        void shadeRow(int x, int y, int count, GPixel row[]) override {
            int a, r, g, b;
           GPixel row1[count];
           fShader1->shadeRow(x, y, count, row1);

           GPixel row2[count];
           fShader2->shadeRow(x, y, count, row2);

           for(int i = 0; i < count; i++) {
               a = GRoundToInt((GPixel_GetA(row1[i]) * GPixel_GetA(row2[i])) / 255.0);
               r = GRoundToInt((GPixel_GetR(row1[i]) * GPixel_GetR(row2[i])) / 255.0);
               g = GRoundToInt((GPixel_GetG(row1[i]) * GPixel_GetG(row2[i])) / 255.0);
               b = GRoundToInt((GPixel_GetB(row1[i]) * GPixel_GetB(row2[i])) / 255.0);

               row[i] = GPixel_PackARGB(a, r, g, b);
           }
        }



    private:
        GShader* fShader1;
        GShader* fShader2;
};

#endif