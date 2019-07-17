#ifndef Edge_DEFINED
#define Edge_DEFINED


class Edge {
    public:
        int fTop, fBot;
        float fCurr_x;
        float fM;
        float fTop_x;
        int fWinding;

        Edge() { 
            fTop = 0;
            fBot = 0;
            fCurr_x = 0;
            fM = 0;
            fTop_x = 0;
            fWinding = 1;
        }
        Edge(GPoint& p0, GPoint& p1);
        bool init(GPoint& p0, GPoint& p1, int winding = 1);
        Edge(GPoint p0, GPoint p1, int winding);
        float calculateCurrX(GPoint& p);
        float calculateSlope(GPoint& p0, GPoint& p1);
        int top() const { return fTop; }
        int bot() const { return fBot; }
        void setTop(int newTop) { fTop = newTop; }
        void setBot(int newBot) { fBot = newBot; }
        float slope() const { return fM; }
        float next_x();
        Edge copyEdge() {
            Edge e = Edge();
            e.fTop = fTop;
            e.fBot = fBot;
            e.fCurr_x = fCurr_x;
            e.fM = fM;
            e.fTop_x = fTop_x;
            e.fWinding = fWinding;
            return e;
        }
};


#endif