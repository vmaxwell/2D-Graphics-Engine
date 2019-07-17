#include "GPoint.h"
#include "GBitmap.h"
#include "Clipper.h"
#include <stdio.h>
/*
void Clipper::chop_Y(GPoint& topPt, GPoint& botPt, const GBitmap& device) {
    float m = (botPt.fX - topPt.fX) / (botPt.fY - topPt.fY);
    //printf("in chop_y\n");
    if(topPt.fY < 0) {
        float x = topPt.fX + m * (0 - topPt.fY);
        this->setClipped(GPoint::Make(x, 0));
    } else {
        if(topPt.fX < 0) {
            chop_left(topPt, botPt, device);
        } else if(topPt.fX > device.width()) {
            chop_right(topPt, botPt, device);
        } else {
            this->setClipped(GPoint::Make(topPt.fX, topPt.fY));
        }
    }

    if(botPt.fY > device.height()) {
        float x = botPt.fX - m * (botPt.fY - device.height());
        float y = device.height();
        this->setClipped(GPoint::Make(x, y));
    } else {
        if(botPt.fX < 0) {
            chop_left(botPt, topPt, device);
        } else if(botPt.fX > device.width()) {
            chop_right(botPt, topPt, device);
        } else {
            this->setClipped(GPoint::Make(botPt.fX, botPt.fY));
        }
    }
}
*/
void Clipper::chop_Y(GPoint& p0, float& m, const GBitmap& device) {
    //printf("%i\n", device.height());

    if(p0.fY < 0) {
        float x = p0.fX + m * (0 - p0.fY);
        //if(x >= 0 && x <= device.width()) {
            if(x < 0) {
                this->setClipped(GPoint::Make(0, 0));
            } else if(x > device.width()) {
                this->setClipped(GPoint::Make(device.width(), 0));
            } else {
                this->setClipped(GPoint::Make(x, 0));
            }
        //}
    } else if(p0.fY > device.height()) {
        float x = p0.fX - m * (p0.fY - device.height());
        //float y = device.height();
        //if(x >= 0 && x <= device.width()) {
            if(x < 0) {
                this->setClipped(GPoint::Make(0, device.height()));
            } else if(x > device.width()) {
                this->setClipped(GPoint::Make(device.width(), device.height()));
            } else {
                this->setClipped(GPoint::Make(x, device.height()));
            }
            //this->setClipped(GPoint::Make(x, y));
        //}
    } else if(p0.fX < 0) {
         chop_left(p0, m, device);
    } else if(p0.fX > device.width()) {
        chop_right(p0, m, device);
    } else {
        this->setClipped(GPoint::Make(p0.fX, p0.fY));
    }
}

void Clipper::chop_left(GPoint& p0, float& m, const GBitmap& device) {
    if(std::isfinite(m) == 0) {
        return;
    }
    float y = p0.fY + ((0 - p0.fX) / m);
    //if(y >= 0 && y <= device.height()) {
        GPoint p1;
        if(y < 0) {
            p1 = GPoint::Make(0, 0);
        } else if(y > device.height()) {
            p1 = GPoint::Make(0, device.height());
        } else {
            p1 = GPoint::Make(0, y);
        }
        //GPoint p1 = GPoint::Make(0, y);
        GPoint p2 = GPoint::Make(p1.fX, p0.fY);
        if(p1.fY < p2.fY) {
            this->setClipped(p2);
            this->setClipped(p1);
        } else {
            this->setClipped(p1);
            this->setClipped(p2);
        }
    //}
}

void Clipper::chop_right(GPoint& p0, float& m, const GBitmap& device) {
    if(std::isfinite(m) == 0) {
        return;
    }
    float y = p0.fY - ((p0.fX - device.width()) / m);
    float x = device.width();
    //if(y >= 0 && y <= device.height()) {
        GPoint p1;
        if(y < 0) {
            p1 = GPoint::Make(x, 0);
        } else if(y > device.height()) {
            p1 = GPoint::Make(x, device.height());
        } else {
            p1 = GPoint::Make(x, y);
        }
        //GPoint p1 = GPoint::Make(x, y);
        GPoint p2 = GPoint::Make(p1.fX, p0.fY);
        if(p1.fY < p2.fY) {
            this->setClipped(p1);
            this->setClipped(p2);
        } else {
            this->setClipped(p2);
            this->setClipped(p1);
        }
    //}
}
/*
void Clipper::chop_left(GPoint& leftPt, GPoint& rightPt, const GBitmap& device) {
    float m = (leftPt.fX - rightPt.fX) / (leftPt.fY - rightPt.fY);
    float y = leftPt.fY + ((0 - leftPt.fX) / m);
    GPoint p0 = GPoint::Make(0, y);
    this->setClipped(p0);
    this->setClipped(GPoint::Make(p0.fX, leftPt.fY));
}

void Clipper::chop_right(GPoint& leftPt, GPoint& rightPt, const GBitmap& device) {
    float m = (leftPt.fX - rightPt.fX) / (leftPt.fY - rightPt.fY);
    float y = rightPt.fY - ((rightPt.fX - device.width()) / m);
    float x = device.width();
    GPoint p1 = GPoint::Make(x, y);
    this->setClipped(p1);
    this->setClipped(GPoint::Make(p1.fX, rightPt.fY));
}
*/

/*
void Clipper::chop_X(GPoint& leftPt, GPoint& rightPt, const GBitmap& device) {
    float m = (leftPt.fX - rightPt.fX) / (leftPt.fY - rightPt.fY);
    //printf("in chop_x\n");
    GPoint p0, p1;
    if(leftPt.fX < 0) {
        float y = leftPt.fY + ((0 - leftPt.fX) / m);
        p0 = GPoint::Make(0, y);
        this->setClipped(p0);
        this->setClipped(GPoint::Make(p0.fX, leftPt.fY));
    } else {
        this->setClipped(GPoint::Make(leftPt.fX, leftPt.fY));
    }
    
    if(rightPt.fX > device.width()) {
        float y = rightPt.fY - ((rightPt.fX - device.width()) / m);
        float x = device.width();
        p1 = GPoint::Make(x, y);
        this->setClipped(p1);
        this->setClipped(GPoint::Make(p1.fX, rightPt.fY));
    } else {
        this->setClipped(GPoint::Make(rightPt.fX, rightPt.fY));
    }
}
*/
/*
Clipper::Clipper(int length) {
    Clipper::fClipped[length*3] = {};
}
*/
void Clipper::clip(const GPoint *points, int length, const GBitmap& device) {
    GPoint p0, p1;
    for(int i = 0; i < length; i++) {
        p0 = points[i];
        p1 = (i == length - 1) ? points[0] : points[i+1];
        clipPts(p0, p1, device);
    }
}

void Clipper::clipPts(GPoint& p0, GPoint& p1, const GBitmap& device) {
    /*
    if(p0.fY == p1.fY) {
        return;
    }
    */

    GPoint topPt = (p0.fY < p1.fY) ? p0 : p1;
    GPoint botPt = (p0.fY > p1.fY) ? p0 : p1;
    GPoint leftPt = (p0.fX < p1.fX) ? p0 : p1;
    GPoint rightPt = (p0.fX > p1.fX) ? p0 : p1;

    if(p0.fY == p1.fY) {
        if(p0.fY >= 0 && p0.fY <= device.height()) {
            if(rightPt.fX < 0 || leftPt.fX > device.width()) {
                return;
            } else if(leftPt.fX < 0) {
                this->setClipped(GPoint::Make(0, leftPt.fY));
            } else if(rightPt.fX > device.width()) {
                this->setClipped(GPoint::Make(device.width(), rightPt.fY));
            }
        } else if(rightPt.fX >=0 && rightPt.fX <= device.width()) {
            if(leftPt.fX < 0 && leftPt.fY < 0) {
                this->setClipped(GPoint::Make(0, 0));
            } else if(leftPt.fX < 0 && leftPt.fY > device.height()) {
                this->setClipped(GPoint::Make(0, device.height()));
            }
        } else if(leftPt.fX >=0 && leftPt.fX <= device.width()) {
            if(rightPt.fX > device.width() && rightPt.fY < 0) {
                this->setClipped(GPoint::Make(device.width(), 0));
            } else if(rightPt.fX > device.width() && rightPt.fY > device.height()) {
                this->setClipped(GPoint::Make(device.width(), device.height()));
            }
        }
        return;
    }

    float m = (botPt.fX - topPt.fX) / (botPt.fY - topPt.fY);

    if(botPt.fY < 0 || topPt.fY > device.height()) {
        return;
    } else if(rightPt.fX < 0) {
        /*
        if(p0.fX == p1.fX) {
            if(topPt.fY >= 0 && topPt.fY <= device.height()) {
                this->setClipped(GPoint::Make(0, topPt.fY));
            } else {
                this->setClipped(GPoint::Make(0, 0));
            }
            if(botPt.fY >= 0 && botPt.fY <= device.height()) {
                this->setClipped(GPoint::Make(0, botPt.fY));
            } else {
                this->setClipped(GPoint::Make(0, device.height()));
            }
        }
        */
        return;
    } else if(leftPt.fX > device.width()) {
        /*
        if(p0.fX == p1.fX) {
            if(topPt.fY >= 0 && topPt.fY <= device.height()) {
                this->setClipped(GPoint::Make(device.width(), topPt.fY));
            } else {
                this->setClipped(GPoint::Make(device.width(), 0));
            }
            if(botPt.fY >= 0 && botPt.fY <= device.height()) {
                this->setClipped(GPoint::Make(device.width(), botPt.fY));
            } else {
                this->setClipped(GPoint::Make(device.width(), device.height()));
            }
        }
        */
        return;
    } else {
        chop_Y(p0, m, device);
        chop_Y(p1, m, device);
    }
    /*
    GPoint leftPt = (p0.fX < p1.fX) ? p0 : p1;
    GPoint rightPt = (p0.fX > p1.fX) ? p0 : p1;

    if(rightPt.fX < 0 || leftPt.fX > device.width()) {
        return;
    } else {
        chop_X(leftPt, rightPt, device);
    }
    */
}