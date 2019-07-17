#ifndef Clipper_DEFINED
#define Clipper_DEFINED

#include "GPoint.h"

class Clipper {
    
    public:
        GPoint *fClipped;
        int fNum_pts;

        Clipper(int length) {
            fClipped = new GPoint[length*3];
            fNum_pts = 0;
        }

        ~Clipper() {
            delete[] fClipped;
        }
        
        GPoint* getClipped() { return fClipped; }
        void setClipped(GPoint p0) { 
            //printf("in setClip\n");
            /*
            if(fNum_pts == 1) {
                int length = sizeof(fClipped) / sizeof(fClipped[0]);
                printf("%i\n", length);
            }
            printf("%i\n", fNum_pts);
            */
           
            if(!containsPt(p0)) {
                //printf("%f, %f\n", p0.fX, p0.fY);
                fClipped[fNum_pts] = p0;
                fNum_pts++;
            }
        }
        //void chop_Y(GPoint& topPt, GPoint& botPt, const GBitmap& device);
        void chop_Y(GPoint& topPt, float& m, const GBitmap& device);
        //void chop_X(GPoint& leftPt, GPoint& rightPt, const GBitmap& device);
        //void chop_left(GPoint& leftPt, GPoint& rightPt, const GBitmap& device);
        void chop_left(GPoint& leftPt, float& m, const GBitmap& device);
        //void chop_right(GPoint& leftPt, GPoint& rightPt, const GBitmap& device);
        void chop_right(GPoint& leftPt, float& m, const GBitmap& device);
        void clip(const GPoint[], int length, const GBitmap& device);
        void clipPts(GPoint& p0, GPoint& p1, const GBitmap& device);
        void clearPts() { fNum_pts = 0; }

    private:
        bool containsPt(GPoint& p0) {
            for(int i = 0; i < fNum_pts; i++) {
                if(fClipped[i].fX == p0.fX && fClipped[i].fY == p0.fY) {
                    return true;
                }
            }
            return false;
        }
};

#endif