#ifndef RSIM_SENSORS_RASTERIZER_H_
#define RSIM_SENSORS_RASTERIZER_H_

#include "era/geometry/math.h"

namespace era
{

class Mesh;

namespace sim
{

class Rasterizer
{

public:

    Rasterizer() : near_clip_z_(-0.1), back_face_culling_(true) {}

    virtual ~Rasterizer() {}

    void rasterize(const Mesh& mesh, const Transform3f& pose, float* canvas) const;

    inline Vec2f project3Dto2D(const Vec3f& p) const
    {
        return Vec2f((fx_ * p.x + tx_) / -p.z + cx_, (fy_ * -p.y + ty_) / -p.z + cy_);
    }

    inline float project2Dto3DX(int x) const
    {
        return (x - cx_plus_tx_) / fx_;
    }

    inline float project2Dto3DY(int y) const
    {
        return -(y - cy_plus_ty_) / fy_;
    }

    inline Vec3f project2Dto3D(int x, int y) const
    {
        return Vec3f(project2Dto3DX(x), project2Dto3DY(y), -1.0);
    }

    inline void setFocalLengths(double fx, double fy)
    {
        fx_ = fx;
        fy_ = fy;
    }

    inline void setOpticalCenter(double cx, double cy)
    {
        cx_ = cx;
        cy_ = cy;
        cx_plus_tx_ = cx_ + tx_;
        cy_plus_ty_ = cy_ + ty_;
    }

    inline void setOpticalTranslation(double tx, double ty)
    {
        tx_ = tx;
        ty_ = ty;
        cx_plus_tx_ = cx_ + tx_;
        cy_plus_ty_ = cy_ + ty_;
    }

    inline void setCanvasSize(unsigned int w, unsigned int h)
    {
       canvas_width_ = w;
       canvas_height_ = h;
    }

    inline double getFocalLengthX() const { return fx_; }
    inline double getFocalLengthY() const { return fy_; }

    inline double getOpticalCenterX() const { return cx_; }
    inline double getOpticalCenterY() const { return cy_; }

    inline double getOpticalTranslationX() const { return tx_; }
    inline double getOpticalTranslationY() const { return ty_; }

    inline unsigned int canvas_width() const { return canvas_width_; }
    inline unsigned int canvas_height() const { return canvas_height_; }

private:

    double near_clip_z_;

    double fx_, fy_;
    double cx_, cy_;
    double tx_, ty_;
    double cx_plus_tx_;
    double cy_plus_ty_;

    unsigned int canvas_width_, canvas_height_;

    bool back_face_culling_;

    void drawTriangle(const Vec3f& p1, const Vec3f& p2, const Vec3f& p3, float* canvas) const;

    void drawTriangle2D(const Vec3f& p1, const Vec3f& p2, const Vec3f& p3, float* canvas) const;

    void drawTrianglePart(int y_start, int y_end,
                          float x_start, float x_start_delta, float x_end, float x_end_delta,
                          float d_start, float d_start_delta, float d_end, float d_end_delta,
                          float* canvas) const;

    void sort(const Vec3f& p1, const Vec3f& p2, const Vec3f& p3, int dim,
              Vec3f& p_min, Vec3f& p_mid, Vec3f& p_max) const;

};

}
}

#endif
