#include "GPoint.h"
#include "GMatrix.h"
#include "GBitmap.h"
#include "GShader.h"

class BitmapShader : public GShader {
    public:
        BitmapShader(const GBitmap device, const GMatrix mtx, GShader::TileMode tileMode) : fDevice(device), fMtx(mtx), fTileMode(tileMode)  {
            fInverse = GMatrix();
        }

        bool isOpaque() override {
            GPixel* pixelPtr;
            for(int y = 0; y < fDevice.height(); y++) {
                for(int x = 0; x < fDevice.width(); x++) {
                    pixelPtr = fDevice.getAddr(x, y);
                    if (GPixel_GetA(*pixelPtr) != 0xFF) {
                        return false;
                    }
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
           GPoint local = fInverse.mapXY(x + 0.5, y + 0.5);
           float tempX;
           float tempY;

           for(int i = 0; i < count; ++i) {
               switch(fTileMode) {
                case GShader::kClamp:
                    tempX = local.fX;
                    tempY = local.fY;

                    if (local.fX < 0) {
                        tempX = 0;
                    } else if (local.fX > fDevice.width() - 1) {
                        tempX = fDevice.width() - 1;
                    }

                    if (local.fY < 0) {
                        tempY = 0;
                    } else if (local.fY > fDevice.height() - 1) {
                        tempY = fDevice.height() - 1;
                    }

                    row[i] = *fDevice.getAddr(floorf(tempX), floorf(tempY));
                    local.fX += fInverse[GMatrix::SX];
                    local.fY += fInverse[GMatrix::KY];
                    break;
                case GShader::kRepeat:
                    tempX = local.fX / fDevice.width();
                    tempY = local.fY / fDevice.height();

                    if (tempX < 0 || tempX > 1) {
                        tempX = tempX - floorf(tempX);
                    }

                    if (tempY < 0 || tempY > 1) {
                        tempY = tempY - floorf(tempY);
                    }

                    tempX *= fDevice.width();
                    tempY *= fDevice.height();
                    row[i] = *fDevice.getAddr(floorf(tempX), floorf(tempY));
                    local.fX += fInverse[GMatrix::SX];
                    local.fY += fInverse[GMatrix::KY];
                    break;
                case GShader::kMirror:
                    tempX = local.fX / fDevice.width();
                    tempY = local.fY / fDevice.height();

                    if (tempX < 0 || tempX > 1) {
                        tempX *= 0.5;
                        tempX -= floorf(tempX);
                        if(tempX > 0.5) {
                            tempX = 1 - tempX;
                        }
                        tempX *= 2;
                    }

                    if (tempY < 0 || tempY > 1) {
                        tempY *= 0.5;
                        tempY -= floorf(tempY);
                        if(tempY > 0.5) {
                            tempY = 1 - tempY;
                        }
                        tempY *= 2;
                    }

                    tempX *= fDevice.width();
                    tempY *= fDevice.height();
                    row[i] = *fDevice.getAddr(floorf(tempX), floorf(tempY));
                    local.fX += fInverse[GMatrix::SX];
                    local.fY += fInverse[GMatrix::KY];
                    break;
               }
           }
        }

    private:
        GMatrix fInverse;
        const GBitmap fDevice;
        const GMatrix fMtx;
        GShader::TileMode fTileMode;
};

std::unique_ptr<GShader> GCreateBitmapShader(const GBitmap& device, const GMatrix& localMatrix, GShader::TileMode tileMode) {
    if (!device.pixels()) {
        return nullptr;
    }

    return std::unique_ptr<GShader>(new BitmapShader(device, localMatrix, tileMode));
}