#include "GPoint.h"
//#include "GMath.h"
#include "Edge.h"

Edge::Edge(GPoint& p0, GPoint& p1) {
    init(p0, p1);
}

Edge::Edge(GPoint p0, GPoint p1, int winding) {
    init(p0, p1, winding);
}

bool Edge::init(GPoint& p0, GPoint& p1, int winding) {
    GPoint topPt = (p0.y() < p1.y()) ? p0 : p1;
    GPoint botPt = (p0.y() > p1.y()) ? p0 : p1;
    fTop = GRoundToInt(topPt.y());
    fBot = GRoundToInt(botPt.y());
    //printf("%i\n", fBot);
    fTop_x = topPt.x();
    if(fTop == fBot) {
        return false;
    }
    //printf("%i, %i\n", fTop, fBot);
    fM = (botPt.x() - topPt.x()) / (botPt.y() - topPt.y());
    fCurr_x = calculateCurrX(topPt);
    fWinding = winding;
    return true;
}

float Edge::calculateCurrX(GPoint& p) {
    float h = GRoundToInt(p.y()) - p.y() + 0.5;
    float d;
    if(std::isfinite(slope()) == 0) {
        d = 0;
    } else {
        d = slope() * h;
    }
    float x = p.x() + d;
    return x;
}

float Edge::next_x() {
    float x = fCurr_x;
    fCurr_x += slope();
    if(fCurr_x < 0) {
        fCurr_x = 0;
    }
    return x;
}

float Edge::calculateSlope(GPoint& p0, GPoint& p1) {
    float m = (p1.x() - p0.x()) / (p1.y() - p0.y());
    return m;
}