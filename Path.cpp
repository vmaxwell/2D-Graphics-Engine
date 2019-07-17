#include <math.h>
#include "GMath.h"
#include "GMatrix.h"
#include "GPath.h"

GPath& GPath::addRect(const GRect& rect, Direction direction) {
    if(direction == Direction::kCW_Direction) {
        this->moveTo(rect.left(), rect.top());
        this->lineTo(rect.right(), rect.top());
        this->lineTo(rect.right(), rect.bottom());
        this->lineTo(rect.left(), rect.bottom());
    } else {
        this->moveTo(rect.left(), rect.top());
        this->lineTo(rect.left(), rect.bottom());
        this->lineTo(rect.right(), rect.bottom());
        this->lineTo(rect.right(), rect.top());
    }
    return *this;
}

GPath& GPath::addPolygon(const GPoint pts[], int count) {
    for(int i = 0; i < count; i++) {
        if(i == 0) {
            this->moveTo(pts[0]);
        } else {
            this->lineTo(pts[i]);
        }
    }
    return *this;
}

GPath& GPath::addCircle(GPoint center, float radius, Direction direction) {
    GMatrix mat = GMatrix();
    mat.setScale(radius, radius);
    mat.postTranslate(center.fX, center.fY);

    GPoint a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p;

    if(direction == kCW_Direction) {
        a = mat.mapXY(1, 0);
        b = mat.mapXY(1, -1*tan(M_PI/8));
        c = mat.mapXY(cos(M_PI/4), -1*sin(M_PI/4));
        d = mat.mapXY(1/tan(3*M_PI/8), -1);
        e = mat.mapXY(0, -1);
        f = mat.mapXY(-1/tan(3*M_PI/8), -1);
        g = mat.mapXY(-1*cos(M_PI/4), -1*sin(M_PI/4));
        h = mat.mapXY(-1, -1*tan(M_PI/8));
        i = mat.mapXY(-1, 0);
        j = mat.mapXY(-1, tan(M_PI/8));
        k = mat.mapXY(-1*cos(M_PI/4), sin(M_PI/4));
        l = mat.mapXY(-1/tan(3*M_PI/8), 1);
        m = mat.mapXY(0, 1);
        n = mat.mapXY(1/tan(3*M_PI/8), 1);
        o = mat.mapXY(cos(M_PI/4), sin(M_PI/4));
        p = mat.mapXY(1, tan(M_PI/8));
    } else {
        a = mat.mapXY(1, 0);
        b = mat.mapXY(1, tan(M_PI/8));
        c = mat.mapXY(cos(M_PI/4), sin(M_PI/4));
        d = mat.mapXY(1/tan(3*M_PI/8), 1);
        e = mat.mapXY(0, 1);
        f = mat.mapXY(-1/tan(3*M_PI/8), 1);
        g = mat.mapXY(-1*cos(M_PI/4), sin(M_PI/4));
        h = mat.mapXY(-1, tan(M_PI/8));
        i = mat.mapXY(-1, 0);
        j = mat.mapXY(-1, -1*tan(M_PI/8));
        k = mat.mapXY(-1*cos(M_PI/4), -1*sin(M_PI/4));
        l = mat.mapXY(-1/tan(3*M_PI/8), -1);
        m = mat.mapXY(0, -1);
        n = mat.mapXY(1/tan(3*M_PI/8), -1);
        o = mat.mapXY(cos(M_PI/4), -1*sin(M_PI/4));
        p = mat.mapXY(1, -1*tan(M_PI/8));
    }
    
    this->moveTo(a);
    this->quadTo(b, c);
    this->quadTo(d, e);

    this->quadTo(f, g);
    this->quadTo(h, i);

    this->quadTo(j, k);
    this->quadTo(l, m);

    this->quadTo(n, o);
    this->quadTo(p, a);

    return *this;
}

GRect GPath::bounds() const {
    if(fPts.size() == 0) {
        return GRect::MakeWH(0, 0);
    }
    float right_x = fPts[0].x();
    float left_x = fPts[0].x();
    float bot_y = fPts[0].y();
    float top_y = fPts[0].y();

    for(int i = 0; i < fPts.size(); i++) {
        if(right_x < fPts[i].x()) {
            right_x = fPts[i].x();
        }
        if(left_x > fPts[i].x()) {
            left_x = fPts[i].x();
        }
        if(bot_y < fPts[i].y()) {
            bot_y = fPts[i].y();
        }
        if(top_y > fPts[i].y()) {
            top_y = fPts[i].y();
        }
    }
    return GRect::MakeLTRB(left_x, top_y, right_x, bot_y);
}

void GPath::transform(const GMatrix& matrix) {
    for(int i = 0; i < fPts.size(); i++) {
        fPts[i] = matrix.mapPt(fPts[i]);
    }
}

void GPath::ChopQuadAt(const GPoint src[3], GPoint dst[5], float t) {
    float p1_x = (1-t)*src[0].fX + t*src[1].fX;
    float p1_y = (1-t)*src[0].fY + t*src[1].fY;
    float p2_x = (1-t)*(1-t)*src[0].fX + 2*t*(1-t)*src[1].fX + t*t*src[2].fX;
    float p2_y = (1-t)*(1-t)*src[0].fY + 2*t*(1-t)*src[1].fY + t*t*src[2].fY;
    float p3_x = (1-t)*src[1].fX + t*src[2].fX;
    float p3_y = (1-t)*src[1].fY + t*src[2].fY;

    dst[0] = {src[0].fX, src[0].fY};
    dst[1] = {p1_x, p1_y};
    dst[2] = {p2_x, p2_y};
    dst[3] = {p3_x, p3_y};
    dst[4] = {src[2].fX, src[2].fY};
}

void GPath::ChopCubicAt(const GPoint src[4], GPoint dst[7], float t) {
    float p1_x = (1-t)*src[0].fX + t*src[1].fX;
    float p1_y = (1-t)*src[0].fY + t*src[1].fY;
    float e_x = (1-t)*src[1].fX + t*src[2].fX;
    float e_y = (1-t)*src[1].fY + t*src[2].fY;
    float p5_x = (1-t)*src[2].fX + t*src[3].fX;
    float p5_y = (1-t)*src[2].fY + t*src[3].fY;
    float p2_x = (1-t)*p1_x + t*e_x;
    float p2_y = (1-t)*p1_y + t*e_y;
    float p4_x = (1-t)*e_x + t*p5_x;
    float p4_y = (1-t)*e_y + t*p5_y;
    float p3_x = (1-t)*(1-t)*(1-t)*src[0].fX + 3*t*(1-t)*(1-t)*src[1].fX + 3*t*t*(1-t)*src[2].fX + t*t*t*src[3].fX;
    float p3_y = (1-t)*(1-t)*(1-t)*src[0].fY + 3*t*(1-t)*(1-t)*src[1].fY + 3*t*t*(1-t)*src[2].fY + t*t*t*src[3].fY;

    dst[0] = {src[0].fX, src[0].fY};
    dst[1] = {p1_x, p1_y};
    dst[2] = {p2_x, p2_y};
    dst[3] = {p3_x, p3_y};
    dst[4] = {p4_x, p4_y};
    dst[5] = {p5_x, p5_y};
    dst[6] = {src[3].fX, src[3].fY};
}
