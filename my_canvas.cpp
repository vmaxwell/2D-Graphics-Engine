/*
#include "GMath.h"
#include "GColor.h"
#include "GPaint.h"
*/
#include "GCanvas.h"
#include "GBitmap.h"
#include "GPoint.h"
#include "GMatrix.h"
/*
#include "GColor.h"
#include "GPaint.h"
#include "GPixel.h"
*/

#include "GRect.h"
#include "Blender.h"
#include "Clipper.h"
#include "Edge.h"
//#include "EdgeBuilder.h"
#include "GShader.h"
#include "TriColorShader.h"
#include "ProxyShader.h"
#include "ComposeShader.h"
#include "RadialGradient.h"
#include "GPath.h"
#include <algorithm>
#include <stack>
#include <vector>
#include <stdlib.h>
/*
#include "GMath.h"
*/
bool sortE_YXM(Edge& e0, Edge& e1) {
    if(e0.top() == e1.top()) {
        if(e0.fTop_x == e1.fTop_x) {
            return e0.slope() < e1.slope();
        } else {
            return e0.fTop_x < e1.fTop_x;
        }
    } else {
        return e0.top() < e1.top();
    }
}

bool sortE_YX(Edge& e0, Edge& e1) {
    if(e0.top() == e1.top()) {
        return e0.fTop_x < e1.fTop_x;
    } else {
        return e0.top() < e1.top();
    }
}

bool sort_X(Edge& e0, Edge& e1) {
    return e0.fCurr_x < e1.fCurr_x;
}

class MyCanvas : public GCanvas {
public:
    MyCanvas(const GBitmap& device) : fDevice(device) {
        fCTM_Arr.push(GMatrix());
    }

    void save() override {
        fCTM_Arr.push(fCTM);
    }

    void restore() override {
        fCTM = fCTM_Arr.top();
        fCTM_Arr.pop();
    }

    void concat(const GMatrix& mtx) {
        fCTM.preConcat(mtx);
    }

    void drawPaint(const GPaint& paint) override { 
        auto *shader = paint.getShader();
        
        GPixel premulPixel = premulColor(paint.getColor());
        
        GPixel row[fDevice.width()];
        int n = fDevice.width();

        for(int y = 0; y < fDevice.height(); y++) {
            if(shader != nullptr) {
                if(!shader->setContext(fCTM)) {
                    return;
                }
                shader->shadeRow(0, y, n, row);
            }

            for(int x = 0; x < fDevice.width(); x++) {
                GPixel* pixelPtr = fDevice.getAddr(x, y);
                if(shader != nullptr) {
                    *pixelPtr = row[x];
                } else {
                    *pixelPtr = premulPixel;
                }
            }
        }

    }
    
    void drawRect(const GRect& rect, const GPaint& paint) override {
        GPoint points[4] = {GPoint::Make(rect.fLeft, rect.fTop), 
                            GPoint::Make(rect.fRight, rect.fTop), 
                            GPoint::Make(rect.fRight, rect.fBottom), 
                            GPoint::Make(rect.fLeft, rect.fBottom)};

        drawConvexPolygon(points, 4, paint);
    }

    void drawConvexPolygon(const GPoint *points, int count, const GPaint& paint) {
        //printf("Device height: %i, count: %i\n", fDevice.height(), count);
        if(count < 3) {
            return;
        }

        GPoint *mappedPoints = new GPoint[count];
        fCTM.mapPoints(mappedPoints, points, count);
        
        Clipper clipper = Clipper(count);
        clipper.clip(mappedPoints, count, fDevice);

        if(clipper.fNum_pts == 0) {
            return;
        }

        GPoint *clipped = clipper.getClipped();

        int num_edges = 0;
        Edge *edges = new Edge[count * 3];
        
        for(int i = 0; i < clipper.fNum_pts; i++) {

            //Edge e = (i == clipper.fNum_pts - 1) ? (Edge(clipped[i], clipped[0])) : (Edge(clipped[i], clipped[i+1]));
            Edge e = Edge();
            if(i == clipper.fNum_pts - 1) {
                if(e.init(clipped[i], clipped[0])) {
                    edges[num_edges] = e;
                    num_edges++;
                }
            } else {
                if(e.init(clipped[i], clipped[i+1])) {
                    edges[num_edges] = e;
                    num_edges++;
                }
            }
        }

        if(num_edges == 0) {
            return;
        }

        std::sort(edges, edges + num_edges, sortE_YXM);
        auto *shader = paint.getShader();
        
        GPixel premulPixel = premulColor(paint.getColor());
        Blender blender = Blender();

        int i = 0;
        //printf("about to start drawing pixels\n");
        Edge leftEdge = edges[i];
        Edge rightEdge = edges[i+1];
        i += 2;
        int leftX, rightX;
        int n;

        GPixel row[fDevice.width()];
        
        GPixel* pixelPtr;
        
        for(int y = leftEdge.top(); y < edges[num_edges - 1].bot(); y++) {
            
            if(leftEdge.bot() < y) {
                leftEdge = edges[i];
                i++;
            }
            if(rightEdge.bot() < y) {
                rightEdge = edges[i];
                i++;
            }
            
            leftX = GRoundToInt(leftEdge.next_x());
            rightX = GRoundToInt(rightEdge.next_x());
            if(rightX > fDevice.width()) {
                rightX = fDevice.width() - 1;
            }
            n = rightX - leftX;

            if(shader != nullptr) {
                if(!shader->setContext(fCTM)) {
                    return;
                }
                shader->shadeRow(leftX, y, n, row);
            }

            for(int x = 0; x < n; x++) {
                pixelPtr = fDevice.getAddr(leftX + x, y);
                if(shader != nullptr) {
                    *pixelPtr = blender.blend(row[x], *pixelPtr, paint.getBlendMode());
                } else {
                    *pixelPtr = blender.blend(premulPixel, *pixelPtr, paint.getBlendMode());
                }
            }

        }

        delete[] edges;
        delete[] mappedPoints;
    }

    void drawPath(const GPath& orig_path, const GPaint& paint) override {
        //printf("-----Begin drawPath-----\n");
        GPath path = orig_path;
        path.transform(fCTM);

        std::vector<Edge> edges = makeEdges(path);
        if(edges.size() == 0) {
            return;
        }

        Edge active[edges.size()];
        Edge* activePtrs[edges.size()];
        int num_active = 0;
        
        std::sort(edges.begin(), edges.end(), sortE_YXM);
        auto edge_iter = edges.begin();
        
        
        
        
        auto *shader = paint.getShader();
        GPixel row[fDevice.width()];
        int n;
        GPixel premulPixel = premulColor(paint.getColor());
        Blender blender = Blender();

        GRect bounds = path.bounds();
        int bounds_top = GRoundToInt(bounds.top());
        int bounds_bot = GRoundToInt(bounds.bottom());
        Edge edge, next;
        int winding = 0;
        int x0, x1;
        GPixel* pixelPtr;
        

        for(int i = 0; i < edges.size(); i++) {
            if(edges[i].top() == bounds_top) {
                active[num_active] = edges[i];
                activePtrs[num_active] = &(edges[i]);
                num_active++;
                edge_iter += i;
            }
        }
        
        std::sort(active, active + num_active, sort_X);

        for(int y = bounds_top; y < bounds_bot; y++) {
            for(int i = 0; i < num_active; i++) {
                edge = active[i];
                if(winding == 0) {
                    x0 = GRoundToInt(edge.next_x());
                }
                winding += edge.fWinding;
                if(winding == 0) {

                    x1 = GRoundToInt(edge.next_x());
                    n = x1 - x0;

                    if(shader != nullptr) {
                        //printf("In shader is not nullptr");
                        if(!shader->setContext(fCTM)) {
                            //printf("In could not setContext for shader\n");
                            return;
                        }
                        shader->shadeRow(x0, y, n, row);
                    } 
                        //printf("In else for shader is nullptr");
                        for(int x = 0; x < n; x++) {
                            pixelPtr = fDevice.getAddr(x0 + x, y);
                            if(shader != nullptr) {
                                //printf("Entering shading pixel\n");
                                *pixelPtr = blender.blend(row[x], *pixelPtr, paint.getBlendMode());
                            } else {
                                *pixelPtr = blender.blend(premulPixel, *pixelPtr, paint.getBlendMode());
                            }
                        }
                    
                }
            }

            for(int i = 0; i < num_active; i++) {
                (activePtrs[i])->next_x();
            }

            num_active = 0;

            for(int i = 0; i < edges.size(); i++) {
                if((y + 1) >= edges[i].top() && (y + 1) < edges[i].bot()) {
                    active[num_active] = edges[i];
                    activePtrs[num_active] = &(edges[i]);
                    num_active++;
                }
            }
            std::sort(active, active + num_active, sort_X);
        }
    }

    void drawMesh(const GPoint verts[], const GColor colors[], const GPoint texs[], int count, const int indices[], const GPaint& paint) override {
        GPoint pt0, pt1, pt2;
        GColor c0, c1, c2;
        GPoint t0, t1, t2;
        //GShader shader;
        GMatrix t_mat, p_mat, mat, invT;
        GPoint pts[3];
        GColor colors2[3];

        //ProxyShader proxy;
        //TriColorShader triColor;

        if(texs != nullptr) {
            t_mat = GMatrix();
            p_mat = GMatrix();
            mat = GMatrix();
            invT = GMatrix();
        }
        
        int n = 0;
        for(int i = 0; i < count; ++i) {
            pt0 = verts[indices[n+0]];
            pt1 = verts[indices[n+1]];
            pt2 = verts[indices[n+2]];

            pts[0] = pt0;
            pts[1] = pt1;
            pts[2] = pt2;

            //get colors and texs if there and drawTriangle
            if(colors != nullptr && texs != nullptr) {
                c0 = colors[indices[n+0]];
                c1 = colors[indices[n+1]];
                c2 = colors[indices[n+2]];

                t0 = texs[indices[n+0]];
                t1 = texs[indices[n+1]];
                t2 = texs[indices[n+2]];

                colors2[0] = c0;
                colors2[1] = c1;
                colors2[2] = c2;
                TriColorShader triColor(pt0, pt1, pt2, colors2);

                t_mat.set6(t1.fX - t0.fX, t2.fX - t0.fX, t0.fX, t1.fY - t0.fY, t2.fY - t0.fY, t0.fY);
                p_mat.set6(pt1.fX - pt0.fX, pt2.fX - pt0.fX, pt0.fX, pt1.fY - pt0.fY, pt2.fY - pt0.fY, pt0.fY);
                t_mat.invert(&invT);
                mat.setConcat(p_mat, invT);

                ProxyShader proxy(paint.getShader(), mat);

                ComposeShader compose(&triColor, &proxy);
                drawConvexPolygon(pts, 3, GPaint(&compose));
            } else if(colors != nullptr) {
                c0 = colors[indices[n+0]];
                c1 = colors[indices[n+1]];
                c2 = colors[indices[n+2]];

                colors2[0] = c0;
                colors2[1] = c1;
                colors2[2] = c2;

                TriColorShader triColor(pt0, pt1, pt2, colors2);
                drawConvexPolygon(pts, 3, GPaint(&triColor));
            } else if(texs != nullptr) {
                t0 = texs[indices[n+0]];
                t1 = texs[indices[n+1]];
                t2 = texs[indices[n+2]];

                t_mat.set6(t1.fX - t0.fX, t2.fX - t0.fX, t0.fX, t1.fY - t0.fY, t2.fY - t0.fY, t0.fY);
                p_mat.set6(pt1.fX - pt0.fX, pt2.fX - pt0.fX, pt0.fX, pt1.fY - pt0.fY, pt2.fY - pt0.fY, pt0.fY);
                t_mat.invert(&invT);
                mat.setConcat(p_mat, invT);

                ProxyShader proxy(paint.getShader(), mat);
                drawConvexPolygon(pts, 3, GPaint(&proxy));
            }

            n += 3;
        }
    }

    void drawQuad(const GPoint verts[4], const GColor colors[4], const GPoint texs[4], int level, const GPaint& paint) override {
        GPoint mesh_verts[(level+2)*(level+2)];
        GColor mesh_colors[(level+2)*(level+2)];
        GColor* colors_ptr = nullptr;
        GPoint mesh_texs[(level+2)*(level+2)];
        GPoint* texs_ptr = nullptr;
        int count = 0;
        float x, y;
        float a, r, g, b;

        for(float u = 0.0; u <= 1.0; u += 1.0/(level+1.0)) {
            for(float v = 0.0; v <= 1.0; v += 1.0/(level+1.0)) {
                x = (1-u)*(1-v)*verts[0].fX + (1-v)*u*verts[1].fX + (1-u)*v*verts[3].fX + u*v*verts[2].fX;
                y = (1-u)*(1-v)*verts[0].fY + (1-v)*u*verts[1].fY + (1-u)*v*verts[3].fY + u*v*verts[2].fY;
                mesh_verts[count] = GPoint::Make(x, y);

                if(colors != nullptr) {
                    a = (1-u)*(1-v)*colors[0].fA + (1-v)*u*colors[1].fA + (1-u)*v*colors[3].fA + u*v*colors[2].fA;
                    r = (1-u)*(1-v)*colors[0].fR + (1-v)*u*colors[1].fR + (1-u)*v*colors[3].fR + u*v*colors[2].fR;
                    g = (1-u)*(1-v)*colors[0].fG + (1-v)*u*colors[1].fG + (1-u)*v*colors[3].fG + u*v*colors[2].fG;
                    b = (1-u)*(1-v)*colors[0].fB + (1-v)*u*colors[1].fB + (1-u)*v*colors[3].fB + u*v*colors[2].fB;
                    mesh_colors[count] = GColor::MakeARGB(a, r, g, b);
                    colors_ptr = mesh_colors;
                }

                if(texs != nullptr) {
                    x = (1-u)*(1-v)*texs[0].fX + (1-v)*u*texs[1].fX + (1-u)*v*texs[3].fX + u*v*texs[2].fX;
                    y = (1-u)*(1-v)*texs[0].fY + (1-v)*u*texs[1].fY + (1-u)*v*texs[3].fY + u*v*texs[2].fY;
                    mesh_texs[count] = GPoint::Make(x, y);
                    texs_ptr = mesh_texs;
                }
                count++;
            }
        }

        int num_triangles = 2 * ((level+1)*(level+1));
        int indices[3*num_triangles];
        int num_ind = 0;
        for(int n = 0; n < (level+2)*(level+1); n += level+2) {
            for(int m = n; m < n + (level+1); m++) {
                indices[num_ind] = m; //0
                //indices[num_ind] = m + 1; //1
                //indices[num_ind] = m + (level+2); //3
                num_ind++;
                indices[num_ind] = m + 1; //1
                //indices[num_ind] = m + (level+2); //3
                //indices[num_ind] = m; //0
                num_ind++;
                indices[num_ind] = m + (level+2); //3
                //indices[num_ind] = m + 1; //1
                //indices[num_ind] = m; //0
                num_ind++;


                //indices[num_ind] = m + 1; //1
                //indices[num_ind] = m + (level+2) + 1; //4
                indices[num_ind] = m + (level+2); //3
                num_ind++;
                //indices[num_ind] = m + (level+2); //3
                indices[num_ind] = m + 1; //1
                //indices[num_ind] = m + (level+2) + 1; //4
                num_ind++;
                indices[num_ind] = m + (level+2) + 1; //4
                //indices[num_ind] = m + 1; //1
                //indices[num_ind] = m + (level+2); //3
                num_ind++;
            }
        }

        drawMesh(mesh_verts, colors_ptr, texs_ptr, num_triangles, indices, paint);
    }

    void final_strokeLine(GPath* dst, GPoint p0, GPoint p1, float width, bool roundCap) override {
        GPoint u = p1 - p0;
        GPoint v = GPoint::Make(u.fY, -1*u.fX);
        float v_length = sqrtf(v.fX*v.fX + v.fY*v.fY);

        v.fX = (v.fX / v_length) * (width/2);
        v.fY = (v.fY / v_length) * (width/2);
        
        GPoint q0, q1, q2, q3;

        q0 = p0 + v;
        q1 = q0 + u;
        q2 = p1 - v;
        q3 = q2 - u;

        GPoint pts[4] = {q0, q1, q2, q3};

        dst->addPolygon(pts, 4);

        if (roundCap) {
            dst->addCircle(p0, width/2, GPath::kCCW_Direction);
            dst->addCircle(p1, width/2, GPath::kCCW_Direction);
        }
    }

    std::unique_ptr<GShader> final_createRadialGradient(GPoint center, float radius, const GColor colors[], int count, GShader::TileMode mode) override {
        if(count < 2) {
            return nullptr;
        } else {
            return std::unique_ptr<GShader>(new RadialGradient(center, radius, colors, count, mode));
        }
    }

private:
    const GBitmap fDevice;
    std::stack<GMatrix> fCTM_Arr;
    GMatrix fCTM;

    GPixel premulColor(const GColor& color) {
        GColor src_color = color.pinToUnit();
        int ia = GRoundToInt(src_color.fA * 255);
        int premulR = GRoundToInt((src_color.fA * src_color.fR) * 255);
        int premulG = GRoundToInt((src_color.fA * src_color.fG) * 255);
        int premulB = GRoundToInt((src_color.fA * src_color.fB) * 255);
        return GPixel_PackARGB(ia, premulR, premulG, premulB);
    }

    std::vector<Edge> makeEdges(const GPath& path) {
        GPath::Edger edger(path);
        std::vector<Edge> edges;
        GPath::Verb v;
        Clipper clipper = Clipper(2);
        GPoint *clipped;
        Edge e;
        GPoint p0, p1;
        float t, dt;
        int n;
        GPoint pts[4];

        v = edger.next(pts);

        while(v != GPath::Verb::kDone) {
            switch(v){
                case GPath::Verb::kLine:
                    clipper.clip(pts, 2, fDevice);
                    clipped = clipper.getClipped();
                    if(clipper.fNum_pts == 0) {
                        break;
                    }

                    e = Edge();
            
                    if(clipped[0].y() < clipped[1].y()) {
                        if(e.init(clipped[0], clipped[1], -1)) {
                            edges.push_back(e);
                        }
                    } else {
                        if(e.init(clipped[0], clipped[1], 1)) {
                            edges.push_back(e);
                        }
                    }
                    clipper.clearPts();
                    break;
                case GPath::Verb::kQuad:
                    n = findQuadN(pts[0], pts[1], pts[2]);
                    t = 0;
                    dt = 1.0 / n;
                    p0 = quadFunc(pts[0], pts[1], pts[2], t);
                    for(int i = 0; i < n; ++i) {
                        t += dt;
                        p1 = quadFunc(pts[0], pts[1], pts[2], t);
                        clipper.clipPts(p0, p1, fDevice);
                        clipped = clipper.getClipped();
                        if(clipper.fNum_pts == 0) {
                            p0 = p1;
                            continue;
                        }

                        e = Edge();

                        if(clipped[0].y() < clipped[1].y()) {
                            if(e.init(clipped[0], clipped[1], -1)) {
                                edges.push_back(e);
                            }
                        } else {
                            if(e.init(clipped[0], clipped[1], 1)) {
                                edges.push_back(e);
                            }
                        }
                        clipper.clearPts();
                        p0 = p1;
                    }
                    break;
                case GPath::Verb::kCubic:
                    n = findCubicN(pts[0], pts[1], pts[2], pts[3]);
                    t = 0;
                    dt = 1.0 / n;
                    p0 = cubicFunc(pts[0], pts[1], pts[2], pts[3], t);
                    for(int i = 0; i < n; ++i) {
                        t += dt;
                        p1 = cubicFunc(pts[0], pts[1], pts[2], pts[3], t);
                        clipper.clipPts(p0, p1, fDevice);
                        clipped = clipper.getClipped();
                        if(clipper.fNum_pts == 0) {
                            p0 = p1;
                            continue;
                        }

                        e = Edge();

                        if(clipped[0].y() < clipped[1].y()) {
                            if(e.init(clipped[0], clipped[1], -1)) {
                                edges.push_back(e);
                            }
                        } else {
                            if(e.init(clipped[0], clipped[1], 1)) {
                                edges.push_back(e);
                            }
                        }
                        clipper.clearPts();
                        p0 = p1;
                    }
                    break;
            }
            
            v = edger.next(pts);
            
        }
        return edges;
    }

    GPoint quadFunc(GPoint& a, GPoint& b, GPoint& c, float t) {
        float x = (1-t)*(1-t)*a.x() + 2*t*(1-t)*b.x() + t*t*c.x();
        float y = (1-t)*(1-t)*a.y() + 2*t*(1-t)*b.y() + t*t*c.y();
        GPoint pt = {x,y};
        return pt;
    }

    int findQuadN(GPoint& a, GPoint& b, GPoint& c) {
        float x = abs((a.x() - 2*b.x() + c.x())/4);
        float y = abs((a.y() - 2*b.y() + c.y())/4);
        float err = sqrtf(x*x + y*y);
        return ceil(sqrtf(err / 0.25));
    }

    int findCubicN(GPoint& a, GPoint& b, GPoint& c, GPoint& d) {
        float x1 = abs(a.x() - 2*b.x() + c.x());
        float y1 = abs(a.y() - 2*b.y() + c.y());
        float x2 = abs(b.x() - 2*c.x() + d.x());
        float y2 = abs(b.y() - 2*c.y() + d.y());
        
        float err1 = sqrtf(x1*x1 + y1*y1);
        float err2 = sqrtf(x2*x2 + y2*y2);
        float err = std::max(err1, err2);
        return sqrtf(0.75 * (err / 0.25));
    }

    GPoint cubicFunc(GPoint& a, GPoint& b, GPoint& c, GPoint& d, float t) {
        float x = (1-t)*(1-t)*(1-t)*a.x() + 3*t*(1-t)*(1-t)*b.x() + 3*t*t*(1-t)*c.x() + t*t*t*d.x();
        float y = (1-t)*(1-t)*(1-t)*a.y() + 3*t*(1-t)*(1-t)*b.y() + 3*t*t*(1-t)*c.y() + t*t*t*d.y();
        GPoint pt = {x,y};
        return pt;
    }
    
};

std::unique_ptr<GCanvas> GCreateCanvas(const GBitmap& device) {
    if (!device.pixels()) {
        return nullptr;
    }
    return std::unique_ptr<GCanvas>(new MyCanvas(device));
}

void GDrawSomething_rects(GCanvas* canvas) {
    GRect rect = GRect::MakeXYWH(112, 112, 300, 300);
    GColor color = GColor::MakeARGB(0.8, 0.4, 0.1, 0.5);

    canvas->fillRect(rect, color);
}

void GDrawSomething_polys(GCanvas* canvas) {
    GRect rect = GRect::MakeXYWH(112, 112, 300, 300);
    GColor color = GColor::MakeARGB(0.8, 0.4, 0.1, 0.5);

    canvas->fillRect(rect, color);
}