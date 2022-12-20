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

    std::vector<Vector3> frame_msaa; // MSAA 4������
    std::vector<numberType> z_msaa;

    GLdouble view_width = 0.0, view_height = 0.0;  // �Ӵ�
public:
    void
    render() override {
        std::tie(view_width, view_height) = scene.camera->getWH();

        // ��ʼ��buffer�Ĵ�С
        frame_buf.assign(static_cast<long long>(view_width * view_height), {0, 0, 0});
        z_buf.assign(static_cast<long long>(view_width * view_height), inf);
        frame_msaa.assign(static_cast<long long>(view_width * view_height * 4), {0, 0, 0});
        z_msaa.assign(static_cast<long long>(view_width * view_height * 4), inf);

        auto viewMat = scene.camera->getViewMat();
        auto projectionMat = scene.camera->getProjectionMat();
        viewPortMat = scene.camera->getViewPortMat();
        // ��Ⱦÿ��model
        for (auto& model : scene.models) {
            auto modelMat = model.modelMat;    // ��ȡÿ��model��modelMat
            MVP =  projectionMat * viewMat * modelMat;
            std::vector<anya::Vector4> worldPositions;
            // TODO: ������ӿڱ任�߼������⣬��Ҫ����
            for (const auto& local : model.localPositions) {
                worldPositions.push_back(viewPortMat * MVP * local.to4());
            }
            model.vertexes.clear();
            for (auto& worldPos : worldPositions) {
                worldPos /= worldPos.w();
                model.vertexes.push_back(worldPos.to<3>());
            }
            drawTriangleWithMSAA(model);
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
    drawTriangleWithMSAA(const Model& triangle) {
        // ���������ε���������
        auto a = triangle.vertexes[0];
        auto b = triangle.vertexes[1];
        auto c = triangle.vertexes[2];
        // �����Χ��
        int left, right, floor, top;
        std::tie(left, right, floor, top) = getBoundingBox(a, b, c);
        // MSAA׼��
        int pid = 0;               // ��ǰ������frame_msaa�е�λ��
        numberType z_pixel = inf;  // ��ǰ���ص����
        const std::array<numberType, 4> dx = { 0.25, 0.25, 0.75, 0.75 };
        const std::array<numberType, 4> dy = { 0.25, 0.75, 0.25, 0.75 };
        // z-buffer�㷨
        for (int i = left; i <= right; ++i) {
            for (int j = floor; j <= top; ++j) {
                pid = getIndex(i, j) * 4;
                // 4����������ģ��
                for (int k = 0; k < 4; ++k) {
                    if (insideTriangle(i + dx[k], j + dy[k], triangle.vertexes)) {
                        // ��ȡ��ֵ���
                        numberType z_interpolated = computeBarycentric2D(i + dx[k], j + dy[k], triangle);
                        // ��Ȳ���
                        if (z_interpolated < z_msaa[pid + k]) {
                            z_msaa[pid + k] = z_interpolated;
                            frame_msaa[pid + k] = triangle.color / 4;
                        }
                    }
                }
                frame_buf[getIndex(i, j)] = frame_msaa[pid] + frame_msaa[pid + 1] + frame_msaa[pid + 2] + frame_msaa[pid + 3];
            }
        }
    }

    void
    drawTriangle(const Model& triangle) {
        // ���������ε���������
        auto a = triangle.vertexes[0];
        auto b = triangle.vertexes[1];
        auto c = triangle.vertexes[2];
        // �����Χ��
        int left, right, floor, top;
        std::tie(left, right, floor, top) = getBoundingBox(a, b, c);
        // z-buffer�㷨
        for (int i = left; i <= right; ++i) {
            for (int j = floor; j <= top; ++j) {
                if (insideTriangle(i + 0.5, j + 0.5, triangle.vertexes)) {
                    // ��ȡ��ֵ���
                    numberType z_interpolated = computeBarycentric2D(i + 0.5, j + 0.5, triangle);
                    // ��Ȳ���
                    if (z_interpolated < z_buf[getIndex(i, j)]) {
                        z_buf[getIndex(i, j)] = z_interpolated;
                        frame_buf[getIndex(i, j)] = triangle.color;
                    }
                }
            }
        }
    }

private:
#pragma region ��������
    // ����жϵ��Ƿ�����������
    bool
    insideTriangle(numberType x, numberType y, const std::vector<Vector3>& vertexes) {
        if (out_range(x, y))
            throw std::out_of_range("Rasterizer::insideTriangle()");
        Vector3 q{x, y, vertexes[0][2]};
        Vector3 ab = vertexes[1] - vertexes[0], bc = vertexes[2] - vertexes[1], ca = vertexes[0] - vertexes[2];
        Vector3 aq = q - vertexes[0], bq = q - vertexes[1], cq = q - vertexes[2];
        return ab.cross(aq).dot(bc.cross(bq)) > 0 && ab.cross(aq).dot(ca.cross(cq)) > 0 && bc.cross(bq).dot(ca.cross(cq)) > 0;
    }

    // ��ȡbuffer���±�
    [[nodiscard]] int
    getIndex(int x, int y) const {
        if (out_range(x, y))
            throw std::out_of_range("Rasterizer::getIndex(int x, int y)");
        return (static_cast<int>(view_height) - 1 - y) * static_cast<int>(view_width) + x;
    }

    // ��ֵ��ȡ���
    numberType
    computeBarycentric2D(numberType x, numberType y, const Model& triangle) {
        if (out_range(x, y))
            throw std::out_of_range("Rasterizer::computeBarycentric2D()");
        const std::vector<Vector3>& vertexes = triangle.vertexes;
        auto v = triangle.toVector4();
        numberType alpha = (x*(vertexes[1].y() - vertexes[2].y()) + (vertexes[2].x() - vertexes[1].x())*y + vertexes[1].x()* vertexes[2].y() - vertexes[2].x()* vertexes[1].y()) / (vertexes[0].x()*(vertexes[1].y() - vertexes[2].y()) + (vertexes[2].x() - vertexes[1].x())* vertexes[0].y() + vertexes[1].x()* vertexes[2].y() - vertexes[2].x()* vertexes[1].y());
        numberType beta = (x*(vertexes[2].y() - vertexes[0].y()) + (vertexes[0].x() - vertexes[2].x())*y + vertexes[2].x()* vertexes[0].y() - vertexes[0].x()* vertexes[2].y()) / (vertexes[1].x()*(vertexes[2].y() - vertexes[0].y()) + (vertexes[0].x() - vertexes[2].x())* vertexes[1].y() + vertexes[2].x()* vertexes[0].y() - vertexes[0].x()* vertexes[2].y());
        numberType gamma = (x*(vertexes[0].y() - vertexes[1].y()) + (vertexes[1].x() - vertexes[0].x())*y + vertexes[0].x()* vertexes[1].y() - vertexes[1].x()* vertexes[0].y()) / (vertexes[2].x()*(vertexes[0].y() - vertexes[1].y()) + (vertexes[1].x() - vertexes[0].x())* vertexes[2].y() + vertexes[0].x()* vertexes[1].y() - vertexes[1].x()* vertexes[0].y());
        numberType w_reciprocal = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
        numberType z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
        z_interpolated *= -w_reciprocal;
        return z_interpolated;
    }

    // �����Χ��
    static std::tuple<int, int, int, int>
    getBoundingBox(Vector3 a, Vector3 b, Vector3 c) {
        int left = std::min(static_cast<int>(a.x()), std::min(static_cast<int>(b.x()), static_cast<int>(c.x())));
        int right = std::max(static_cast<int>(a.x()), std::max(static_cast<int>(b.x()), static_cast<int>(c.x()))) + 1;
        int floor = std::min(static_cast<int>(a.y()), std::min(static_cast<int>(b.y()), static_cast<int>(c.y())));
        int top = std::max(static_cast<int>(a.y()), std::max(static_cast<int>(b.y()), static_cast<int>(c.y()))) + 1;
        return {left, right, floor, top};
    }

    // Խ���ж�
    [[nodiscard]] constexpr bool
    out_range(numberType x, numberType y) const {
        return x < 0 || x >= view_width || y < 0 || y >= view_height;
    }
#pragma endregion

};

}

#endif //ANYA_ENGINE_RASTERIZER_HPP
