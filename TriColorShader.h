#ifndef TriColorShader_DEFINED
#define TriColorShader_DEFINED

#include "GPixel.h"
#include "GColor.h"
#include "GMatrix.h"
#include "GBitmap.h"
#include "GShader.h"

class TriColorShader : public GShader {
    public:
        TriColorShader(GPoint p0, GPoint p1, GPoint p2, const GColor color[3]) : fP0(p0), fP1(p1), fP2(p2), fColors(color) {
            fInverse = GMatrix();
            fMtx = GMatrix();
            
            float u_x = std::abs(fP1.x() - fP0.x());
            float u_y = std::abs(fP1.y() - fP0.y());
            float v_x = std::abs(fP2.x() - fP0.x());
            float v_y = std::abs(fP2.y() - fP0.y());
            fMtx.set6(u_x, v_x, fP0.x(), u_y, v_y, fP0.y());
        }

        bool isOpaque() override {
            for(int i = 0; i < 3; i++) {
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
            /*
           GPoint localPt = fInverse.mapXY(x + 0.5, y + 0.5);
           float tempX, tempY;
           GColor src_color;
           float a, r, g, b;
           int ia, premulR, premulG, premulB;


           for(int i = 0; i < count; ++i) {
               localPt = fInverse.mapXY(x + i + 0.5, y + 0.5);
               tempX = localPt.fX;
               tempY = localPt.fY;

//printf("%g %g\n", tempX, tempY);
               a = tempX * fColors[1].fA + tempY * fColors[2].fA - (1 - tempX - tempY) * fColors[0].fA;
               r = tempX * fColors[1].fR + tempY * fColors[2].fR - (1 - tempX - tempY) * fColors[0].fR;
               g = tempX * fColors[1].fG + tempY * fColors[2].fG - (1 - tempX - tempY) * fColors[0].fG;
               b = tempX * fColors[1].fB + tempY * fColors[2].fB - (1 - tempX - tempY) * fColors[0].fB;

               src_color = GColor::MakeARGB(GPinToUnit(a), GPinToUnit(r), GPinToUnit(g), GPinToUnit(b));
               ia = GRoundToInt(src_color.fA * 255);
               premulR = GRoundToInt((src_color.fA * src_color.fR) * 255);
               premulG = GRoundToInt((src_color.fA * src_color.fG) * 255);
               premulB = GRoundToInt((src_color.fA * src_color.fB) * 255);
               
               row[i] = GPixel_PackARGB(ia, premulR, premulG, premulB);
           }
           */


           GColor src_color;
           float a, r, g, b;
           int ia, premulR, premulG, premulB;

           GPoint localPt = fInverse.mapXY(x + 0.5, y + 0.5);
           GColor dC1 = GColor::MakeARGB(fColors[1].fA - fColors[0].fA, fColors[1].fR - fColors[0].fR, fColors[1].fG - fColors[0].fG, fColors[1].fB - fColors[0].fB);
           GColor dC2 = GColor::MakeARGB(fColors[2].fA - fColors[0].fA, fColors[2].fR - fColors[0].fR, fColors[2].fG - fColors[0].fG, fColors[2].fB - fColors[0].fB);

           float dA = fInverse[0] * dC1.fA + fInverse[3] * dC2.fA;
           float dR = fInverse[0] * dC1.fR + fInverse[3] * dC2.fR;
           float dG = fInverse[0] * dC1.fG + fInverse[3] * dC2.fG;
           float dB = fInverse[0] * dC1.fB + fInverse[3] * dC2.fB;

           a = localPt.fX * dC1.fA + localPt.fY * dC2.fA + fColors[0].fA;
           r = localPt.fX * dC1.fR + localPt.fY * dC2.fR + fColors[0].fR;
           g = localPt.fX * dC1.fG + localPt.fY * dC2.fG + fColors[0].fG;
           b = localPt.fX * dC1.fB + localPt.fY * dC2.fB + fColors[0].fB;
           
           src_color = GColor::MakeARGB(GPinToUnit(a), GPinToUnit(r), GPinToUnit(g), GPinToUnit(b));
           ia = GRoundToInt(src_color.fA * 255);
           premulR = GRoundToInt((src_color.fA * src_color.fR) * 255);
           premulG = GRoundToInt((src_color.fA * src_color.fG) * 255);
           premulB = GRoundToInt((src_color.fA * src_color.fB) * 255);
           
           row[0] = GPixel_PackARGB(ia, premulR, premulG, premulB);

           for(int i = 1; i < count; ++i) {
               a = a + dA; 
               r = r + dR;
               g = g + dG;
               b = b + dB;
               
               src_color = GColor::MakeARGB(GPinToUnit(a), GPinToUnit(r), GPinToUnit(g), GPinToUnit(b));
               ia = GRoundToInt(src_color.fA * 255);
               premulR = GRoundToInt((src_color.fA * src_color.fR) * 255);
               premulG = GRoundToInt((src_color.fA * src_color.fG) * 255);
               premulB = GRoundToInt((src_color.fA * src_color.fB) * 255);
               
               row[i] = GPixel_PackARGB(ia, premulR, premulG, premulB);
           }
           
           
        }

        private:
        GPoint fP0, fP1, fP2;
        const GColor* fColors;

        GMatrix fInverse;
        GMatrix fMtx;
};

#endif