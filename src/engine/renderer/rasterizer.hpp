//
// Created by Anya on 2022/12/5.
//

#ifndef ANYA_ENGINE_RASTERIZER_HPP
#define ANYA_ENGINE_RASTERIZER_HPP

#include "interface/renderer.hpp"
#include "math/matrix.hpp"
#include "math/utils.hpp"

// ��ģ��ʵ��������Ĺ�դ��������Ⱦ��

namespace anya {

class Rasterizer: public Renderer {
private:
    Matrix44 MVP;          // MVP�任����
    Matrix44 viewPortMat;  // �Ӵ��任����

    std::vector<Vector3> frame_buf;  // ֡����
    std::vector<numberType> z_buf;   // ��Ȼ���

    GLdouble view_width = 0.0, view_height = 0.0;  // �Ӵ�
public:
    void
    render() override {
        std::tie(view_width, view_height) = scene.camera->getWH();

        // ��ʼ��buffer�Ĵ�С
        frame_buf.assign(static_cast<long long>(view_width * view_height), {0, 0, 0});
        z_buf.assign(static_cast<long long>(view_width * view_height), inf);

        auto viewMat = scene.camera->getViewMat();
        auto projectionMat = scene.camera->getProjectionMat();
        viewPortMat = scene.camera->getViewPortMat();

        // ��Ⱦÿ��model
        for (auto& model : scene.models) {
            auto modelMat = model.modelMat;   // ��ȡÿ��model��modelMat
            MVP =  projectionMat * viewMat * modelMat;
            std::vector<anya::Vector4> worldPositions;
            for (const auto& local : model.localPositions) {
                worldPositions.push_back(viewPortMat * MVP * local.to4());
            }
            model.vertexes.clear();
            for (auto& worldPos : worldPositions) {
                worldPos /= worldPos.w();
                model.vertexes.push_back(worldPos.to<3>());
            }
            drawTriangle(model);
        }
    }

    // ��ȡ������Ϣ
    [[nodiscard]] Vector3
    getPixel(int x, int y) const override {
        return frame_buf[getIndex(x, y)];
    }

private:
    // ����
    void
    drawTriangle(const Model& triangle) {
        // ���������ε���������
        auto a = triangle.vertexes[0];
        auto b = triangle.vertexes[1];
        auto c = triangle.vertexes[2];
        // �����Χ��
        int left = std::min(static_cast<int>(a.x()), std::min(static_cast<int>(b.x()), static_cast<int>(c.x())));
        int right = std::max(static_cast<int>(a.x()), std::max(static_cast<int>(b.x()), static_cast<int>(c.x()))) + 1;
        int floor = std::min(static_cast<int>(a.y()), std::min(static_cast<int>(b.y()), static_cast<int>(c.y())));
        int top = std::max(static_cast<int>(a.y()), std::max(static_cast<int>(b.y()), static_cast<int>(c.y()))) + 1;
        // z-buffer�㷨
        auto v = triangle.toVector4();
        for (int i = left; i <= right; ++i) {
            for (int j = floor; j <= top; ++j) {
                if (insideTriangle(i + 0.5, j + 0.5, triangle.vertexes)) {
                    numberType alpha, beta, gamma;
                    std::tie(alpha, beta, gamma) = computeBarycentric2D(i + 0.5, j + 0.5, triangle.vertexes);
                    numberType w_reciprocal = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                    numberType z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                    z_interpolated *= -w_reciprocal;
                    // ��Ȳ���
                    if (z_interpolated < z_buf[getIndex(i, j)]) {
                        z_buf[getIndex(i, j)] = z_interpolated;
                        frame_buf[getIndex(i, j)] = triangle.color;
                    }
                }
            }
        }
    }

    // ����жϵ��Ƿ�����������
    static bool
    insideTriangle(numberType x, numberType y, const std::vector<Vector3>& vertexes) {
        Vector3 q{x, y, vertexes[0][2]};
        Vector3 ab = vertexes[1] - vertexes[0], bc = vertexes[2] - vertexes[1], ca = vertexes[0] - vertexes[2];
        Vector3 aq = q - vertexes[0], bq = q - vertexes[1], cq = q - vertexes[2];
        return ab.cross(aq).dot(bc.cross(bq)) > 0 && ab.cross(aq).dot(ca.cross(cq)) > 0 && bc.cross(bq).dot(ca.cross(cq)) > 0;
    }

    // ��ȡbuffer���±�
    [[nodiscard]] int
    getIndex(int x, int y) const {
        return (static_cast<int>(view_height) - 1 - y) * static_cast<int>(view_width) + x;
    }

    // ��ֵ��ȡ���
    static std::tuple<numberType, numberType, numberType>
    computeBarycentric2D(numberType x, numberType y, const std::vector<Vector3>& v) {
        numberType c1 = (x*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*y + v[1].x()*v[2].y() - v[2].x()*v[1].y()) / (v[0].x()*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*v[0].y() + v[1].x()*v[2].y() - v[2].x()*v[1].y());
        numberType c2 = (x*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*y + v[2].x()*v[0].y() - v[0].x()*v[2].y()) / (v[1].x()*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*v[1].y() + v[2].x()*v[0].y() - v[0].x()*v[2].y());
        numberType c3 = (x*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*y + v[0].x()*v[1].y() - v[1].x()*v[0].y()) / (v[2].x()*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*v[2].y() + v[0].x()*v[1].y() - v[1].x()*v[0].y());
        return {c1,c2,c3};
    }

};

}

#endif //ANYA_ENGINE_RASTERIZER_HPP
