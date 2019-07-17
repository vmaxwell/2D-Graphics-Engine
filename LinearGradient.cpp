#include "GPixel.h"
#include "GColor.h"
#include "GMatrix.h"
#include "GBitmap.h"
#include "GShader.h"

class LinearGradient : public GShader {
    public:
        LinearGradient(GPoint p0, GPoint p1, const GColor color[], int count, GShader::TileMode tileMode) : fP0(p0), fP1(p1), fColors(color), fCount(count), fTileMode(tileMode) {
            fInverse = GMatrix();
            fMtx = GMatrix();
            
            float dx = std::abs(fP1.x() - fP0.x());
            float dy = std::abs(fP1.y() - fP0.y());
            fMtx.set6(dx, -dy, fP0.x(), dy, dx, fP0.y());
        }

        bool isOpaque() override {
            for(int i = 0; i < fCount; i++) {
                if (fColors[i].fA != 0xFF) {
                    return false;
                }
            }
            return true;
        }

        bool setContext(const GMatrix& ctm) override {
            GMatrix temp = GMatrix();
            temp.setConcat(ctm, fMtx);
            
            if(!temp.invert(&fInverse)) {
                return false;
            }
            return true;
        }

        void shadeRow(int x, int y, int count, GPixel row[]) override {
           GPoint localPt = fInverse.mapXY(x + 0.5, y + 0.5);
           float tempX, w;
           int orig_leftIndex;
           GColor leftColor, rightColor, src_color;
           float a, r, g, b;
           int ia, premulR, premulG, premulB;


           for(int i = 0; i < count; ++i) {
               switch(fTileMode) {
                case GShader::kClamp:
                    localPt = fInverse.mapXY(x + i + 0.5, y + 0.5);
                    tempX = localPt.fX;

                    if (tempX < 0) {
                        tempX = 0;
                    } else if (tempX > 1) {
                        tempX = 1;
                    }

                    orig_leftIndex = floorf(tempX * (fCount - 1));
                    w = tempX * (fCount-1) - orig_leftIndex;

                    if(w == 0 || w == 1) {
                        leftColor = fColors[orig_leftIndex];

                        a = leftColor.fA;
                        r = leftColor.fR;
                        g = leftColor.fG;
                        b = leftColor.fB;
                    } else {
                        leftColor = fColors[orig_leftIndex];
                        rightColor = fColors[orig_leftIndex + 1];

                        a = leftColor.fA + w * (rightColor.fA - leftColor.fA);
                        r = leftColor.fR + w * (rightColor.fR - leftColor.fR);
                        g = leftColor.fG + w * (rightColor.fG - leftColor.fG);
                        b = leftColor.fB + w * (rightColor.fB - leftColor.fB);
                    }

                    src_color = GColor::MakeARGB(GPinToUnit(a), GPinToUnit(r), GPinToUnit(g), GPinToUnit(b));
                    ia = GRoundToInt(src_color.fA * 255);
                    premulR = GRoundToInt((src_color.fA * src_color.fR) * 255);
                    premulG = GRoundToInt((src_color.fA * src_color.fG) * 255);
                    premulB = GRoundToInt((src_color.fA * src_color.fB) * 255);

                    row[i] = GPixel_PackARGB(ia, premulR, premulG, premulB);
                    //localPt.fX += fInverse[GMatrix::SX];
                    break;
                case GShader::kRepeat:
                    localPt = fInverse.mapXY(x + i + 0.5, y + 0.5);
                    tempX = localPt.fX;

                    if (tempX < 0 || tempX > 1) {
                        tempX = tempX - floorf(tempX);
                    }

                    orig_leftIndex = floorf(tempX * (fCount - 1));
                     w = tempX * (fCount-1) - orig_leftIndex;
                
                    if(w == 0 || w == 1) {
                        leftColor = fColors[orig_leftIndex];

                        a = leftColor.fA;
                        r = leftColor.fR;
                        g = leftColor.fG;
                        b = leftColor.fB;
                    } else {
                        leftColor = fColors[orig_leftIndex];
                        rightColor = fColors[orig_leftIndex + 1];

                        a = rightColor.fA + w * (leftColor.fA - rightColor.fA);
                        r = rightColor.fR + w * (leftColor.fR - rightColor.fR);
                        g = rightColor.fG + w * (leftColor.fG - rightColor.fG);
                        b = rightColor.fB + w * (leftColor.fB - rightColor.fB);
                    }

                    src_color = GColor::MakeARGB(GPinToUnit(a), GPinToUnit(r), GPinToUnit(g), GPinToUnit(b));
                    ia = GRoundToInt(src_color.fA * 255);
                    premulR = GRoundToInt((src_color.fA * src_color.fR) * 255);
                    premulG = GRoundToInt((src_color.fA * src_color.fG) * 255);
                    premulB = GRoundToInt((src_color.fA * src_color.fB) * 255);

                    row[i] = GPixel_PackARGB(ia, premulR, premulG, premulB);
                    //localPt.fX += fInverse[GMatrix::SX];
                    break;
                case GShader::kMirror:
                    localPt = fInverse.mapXY(x + i + 0.5, y + 0.5);
                    tempX = localPt.fX;

                    if (tempX < 0 || tempX > 1) {
                        tempX *= 0.5;
                        tempX -= floorf(tempX);
                        if(tempX > 0.5) {
                            tempX = 1 - tempX;
                        }
                        tempX *= 2;
                    }

                    orig_leftIndex = floorf(tempX * (fCount - 1));
                     w = tempX * (fCount-1) - orig_leftIndex;

                    if(w == 0 || w == 1) {
                        leftColor = fColors[orig_leftIndex];

                        a = leftColor.fA;
                        r = leftColor.fR;
                        g = leftColor.fG;
                        b = leftColor.fB;
                    } else {
                        leftColor = fColors[orig_leftIndex];
                        rightColor = fColors[orig_leftIndex + 1];

                        a = rightColor.fA + w * (leftColor.fA - rightColor.fA);
                        r = rightColor.fR + w * (leftColor.fR - rightColor.fR);
                        g = rightColor.fG + w * (leftColor.fG - rightColor.fG);
                        b = rightColor.fB + w * (leftColor.fB - rightColor.fB);
                    }

                    src_color = GColor::MakeARGB(GPinToUnit(a), GPinToUnit(r), GPinToUnit(g), GPinToUnit(b));
                    ia = GRoundToInt(src_color.fA * 255);
                    premulR = GRoundToInt((src_color.fA * src_color.fR) * 255);
                    premulG = GRoundToInt((src_color.fA * src_color.fG) * 255);
                    premulB = GRoundToInt((src_color.fA * src_color.fB) * 255);

                    row[i] = GPixel_PackARGB(ia, premulR, premulG, premulB);
                    //localPt.fX += fInverse[GMatrix::SX];
                    break;
               }
           }
        }



    private:
        GPoint fP0;
        GPoint fP1;
        const GColor *fColors;
        int fCount;

        GMatrix fInverse;
        GMatrix fMtx;
        GShader::TileMode fTileMode;
};

std::unique_ptr<GShader> GCreateLinearGradient(GPoint p0, GPoint p1, const GColor color[], int count, GShader::TileMode tileMode) {
    if(count < 1) {
        return nullptr;
    }
    return std::unique_ptr<GShader>(new LinearGradient(p0, p1, color, count, tileMode));
}