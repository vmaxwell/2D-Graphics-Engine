#include "GMatrix.h"
#include "math.h"

void GMatrix::setIdentity() {
    fMat[0] = 1;    fMat[1] = 0;    fMat[2] = 0;
    fMat[3] = 0;    fMat[4] = 1;    fMat[5] = 0;
}

void GMatrix::setTranslate(float tx, float ty) {
   fMat[0] = 1;     fMat[1] = 0;    fMat[2] = tx;
   fMat[3] = 0;     fMat[4] = 1;    fMat[5] = ty;
   
}

void GMatrix::setScale(float sx, float sy) {
   fMat[0] = sx;    fMat[1] = 0;    fMat[2] = 0;
   fMat[3] = 0;    fMat[4] = sy;    fMat[5] = 0;
   
}

void GMatrix::setRotate(float radians) {
    
    fMat[0] = cos(radians);    fMat[1] = -sin(radians);    fMat[2] = 0;
    fMat[3] = sin(radians);    fMat[4] = cos(radians);    fMat[5] = 0;
}

void GMatrix::setConcat(const GMatrix& secundo, const GMatrix& primo) {
    float temp[6];
    temp[0] = (secundo.fMat[0] * primo.fMat[0]) + (secundo.fMat[1] * primo.fMat[3]);    
    temp[1] = (secundo.fMat[0] * primo.fMat[1]) + (secundo.fMat[1] * primo.fMat[4]);    
    temp[2] = (secundo.fMat[0] * primo.fMat[2]) + (secundo.fMat[1] * primo.fMat[5]) + secundo.fMat[2];
    temp[3] = (secundo.fMat[3] * primo.fMat[0]) + (secundo.fMat[4] * primo.fMat[3]);    
    temp[4] = (secundo.fMat[3] * primo.fMat[1]) + (secundo.fMat[4] * primo.fMat[4]);    
    temp[5] = (secundo.fMat[3] * primo.fMat[2]) + (secundo.fMat[4] * primo.fMat[5]) + secundo.fMat[5];

    set6(temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);
}

bool GMatrix::invert(GMatrix* inverse) const {
    float det = (fMat[0] * fMat[4]) - (fMat[1] * fMat[3]);
    if (det == 0) {
        return false;
    }

    float a = fMat[4];
    float b = -1 * fMat[1];
    float c = (fMat[1] * fMat[5]) - (fMat[4] * fMat[2]);
    float d = -1 * fMat[3];
    float e = fMat[0];
    float f = (fMat[3] * fMat[2]) - (fMat[0] * fMat[5]);

    inverse->set6(a/det, b/det, c/det, d/det, e/det, f/det);
    return true;
}

void GMatrix::mapPoints(GPoint dst[], const GPoint src[], int count) const {
    float src_temp_x;
    float src_temp_y;
    for(int i = 0; i < count; i++) {
        src_temp_x = src[i].x();
        src_temp_y = src[i].y();
        dst[i].fX = (fMat[0] * src_temp_x) + (fMat[1] * src_temp_y) + fMat[2];
        dst[i].fY = (fMat[3] * src_temp_x) + (fMat[4] * src_temp_y) + fMat[5];
    }
}