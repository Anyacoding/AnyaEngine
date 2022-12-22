//
// Created by Anya on 2022/12/5.
//

#ifndef ANYA_ENGINE_RASTERIZER_HPP
#define ANYA_ENGINE_RASTERIZER_HPP

#include "interface/renderer.hpp"
#include "tool/matrix.hpp"
#include "tool/utils.hpp"

// ��ģ��ʵ��������Ĺ�դ��������Ⱦ��

namespace anya {

class Rasterizer: public Renderer {
private:
    Matrix44 MVP;          // MVP�任����
    Matrix44 viewPortMat;  // �Ӵ��任����
    Matrix44 invMat;       // �����ڷ��ߵľ���

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

        // ��ȡMVP����
        auto viewMat = scene.camera->getViewMat();
        auto projectionMat = scene.camera->getProjectionMat();
        viewPortMat = scene.camera->getViewPortMat();

        // ���������Ϣ��������
        auto[f1, f2] = scene.camera->getFixedArgs();

        // ��Ⱦÿ��model
        for (auto& model : scene.models) {

            auto modelMat = model.modelMat;    // ��ȡÿ��model��modelMat
            MVP =  projectionMat * viewMat * modelMat;
            invMat = (viewMat * modelMat).inverse().transpose();

            for (auto triangle : model.TriangleList) {
                std::vector<Vector4> viewSpace{};       // viewSpace���㼯��
                for (auto& vertex : triangle.vertexes) {
                    viewSpace.push_back(viewMat * modelMat * vertex);
                    vertex = viewPortMat * MVP * vertex;
                    // ���������Ϣ�����������ֵ
                    auto w = vertex.w();
                    vertex /= w;
                    vertex.w() = w;
                    vertex.z() = vertex.z() * f1 + f2;
                }
                for (auto& normal : triangle.normals) {
                    // �Է��߽��б任
                    normal = invMat * normal;
                }
                drawTriangle(triangle, viewSpace, model.fragmentShader);
                // drawTriangle(triangle);
            }
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
    drawTriangleWithMSAA(const Triangle& triangle) {
        // ���������ε���������
        auto a = triangle.a();
        auto b = triangle.b();
        auto c = triangle.c();
        // �����Χ��
        auto[left, right, floor, top] = getBoundingBox(a, b, c);
        // MSAA׼��
        int pid = 0;               // ��ǰ������frame_msaa�е�λ��
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
                        auto[alpha, beta, gamma] = computeBarycentric2DWithFixed(i + dx[k], j + dy[k], triangle);
                        numberType z_interpolated = attributeLerp(alpha, beta, gamma, triangle.vertexes[0].z(), triangle.vertexes[1].z(), triangle.vertexes[2].z());
                        // ��Ȳ���
                        if (z_interpolated < z_msaa[pid + k]) {
                            z_msaa[pid + k] = z_interpolated;
                            frame_msaa[pid + k] = triangle.colors[0] / 4;
                        }
                    }
                }
                frame_buf[getIndex(i, j)] = frame_msaa[pid] + frame_msaa[pid + 1] + frame_msaa[pid + 2] + frame_msaa[pid + 3];
            }
        }
    }

    void
    drawTriangle(const Triangle& triangle) {
        // ���������ε���������
        auto a = triangle.a();
        auto b = triangle.b();
        auto c = triangle.c();
        // �����Χ��
        int left, right, floor, top;
        std::tie(left, right, floor, top) = getBoundingBox(a, b, c);
        // z-buffer�㷨
        for (int i = left; i <= right; ++i) {
            for (int j = floor; j <= top; ++j) {
                if (insideTriangle(i + 0.5, j + 0.5, triangle.vertexes)) {
                    // ��ȡ��ֵ���
                    auto[alpha, beta, gamma] = computeBarycentric2DWithFixed(i + 0.5, j + 0.5, triangle);
                    numberType z_interpolated = attributeLerp(alpha, beta, gamma, triangle.vertexes[0].z(), triangle.vertexes[1].z(), triangle.vertexes[2].z());
                    // ��Ȳ���
                    if (z_interpolated < z_buf[getIndex(i, j)]) {
                        z_buf[getIndex(i, j)] = z_interpolated;
                        frame_buf[getIndex(i, j)] = triangle.colors[0];
                    }
                }
            }
        }
    }

    void
    drawTriangle(const Triangle& triangle, const std::vector<Vector4>& viewSpace, FragmentShader& fragmentShader) {
        // ���������ε���������
        auto a = triangle.a();
        auto b = triangle.b();
        auto c = triangle.c();
        // �����Χ��
        int left, right, floor, top;
        std::tie(left, right, floor, top) = getBoundingBox(a, b, c);
        // z-buffer�㷨
        for (int i = left; i <= right; ++i) {
            for (int j = floor; j <= top; ++j) {
                if (insideTriangle(i + 0.5, j + 0.5, triangle.vertexes)) {
                    // ��ȡ��ֵ���
                    auto[alpha, beta, gamma] = computeBarycentric2DWithFixed(i + 0.5, j + 0.5, triangle);
                    numberType z_lerp = attributeLerp(alpha, beta, gamma, triangle.vertexes[0].z(), triangle.vertexes[1].z(), triangle.vertexes[2].z());
                    // ��Ȳ���
                    if (z_lerp < z_buf[getIndex(i, j)]) {
                        z_buf[getIndex(i, j)] = z_lerp;

                        auto normal_lerp = attributeLerp(alpha, beta, gamma, triangle.normals[0], triangle.normals[1], triangle.normals[2]);
                        auto color_lerp = attributeLerp(alpha, beta, gamma, triangle.colors[0], triangle.colors[1], triangle.colors[2]);
                        auto shadingcoords_lerp = attributeLerp(alpha, beta, gamma, viewSpace[0], viewSpace[1], viewSpace[2]);

                        fragmentShader.init(shadingcoords_lerp, color_lerp, normal_lerp);
                        auto pixel_color = fragmentShader.process(fragmentShader);

                        frame_buf[getIndex(i, j)] = pixel_color;
                    }
                }
            }
        }
    }

private:
#pragma region ��������
    // ����жϵ��Ƿ�����������
    bool
    insideTriangle(numberType x, numberType y, const std::array<Vector4, 3>& vertexes) {
        if (out_range(x, y))
            throw std::out_of_range("Rasterizer::insideTriangle()");
        Vector3 q{x, y, vertexes[0].to<3>()[2]};
        Vector3 ab = vertexes[1].to<3>() - vertexes[0].to<3>(), bc = vertexes[2].to<3>() - vertexes[1].to<3>(), ca = vertexes[0].to<3>() - vertexes[2].to<3>();
        Vector3 aq = q - vertexes[0].to<3>(), bq = q - vertexes[1].to<3>(), cq = q - vertexes[2].to<3>();
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
    std::tuple<numberType, numberType, numberType>
    computeBarycentric2DWithFixed(numberType x, numberType y, const Triangle& triangle) {
        if (out_range(x, y))
            throw std::out_of_range("Rasterizer::computeBarycentric2D()");
        const auto& vertexes = triangle.vertexes;
        numberType alpha = (x*(vertexes[1].y() - vertexes[2].y()) + (vertexes[2].x() - vertexes[1].x())*y + vertexes[1].x()* vertexes[2].y() - vertexes[2].x()* vertexes[1].y()) / (vertexes[0].x()*(vertexes[1].y() - vertexes[2].y()) + (vertexes[2].x() - vertexes[1].x())* vertexes[0].y() + vertexes[1].x()* vertexes[2].y() - vertexes[2].x()* vertexes[1].y());
        numberType beta = (x*(vertexes[2].y() - vertexes[0].y()) + (vertexes[0].x() - vertexes[2].x())*y + vertexes[2].x()* vertexes[0].y() - vertexes[0].x()* vertexes[2].y()) / (vertexes[1].x()*(vertexes[2].y() - vertexes[0].y()) + (vertexes[0].x() - vertexes[2].x())* vertexes[1].y() + vertexes[2].x()* vertexes[0].y() - vertexes[0].x()* vertexes[2].y());
        numberType gamma = (x*(vertexes[0].y() - vertexes[1].y()) + (vertexes[1].x() - vertexes[0].x())*y + vertexes[0].x()* vertexes[1].y() - vertexes[1].x()* vertexes[0].y()) / (vertexes[2].x()*(vertexes[0].y() - vertexes[1].y()) + (vertexes[1].x() - vertexes[0].x())* vertexes[2].y() + vertexes[0].x()* vertexes[1].y() - vertexes[1].x()* vertexes[0].y());
        numberType w_reciprocal = 1.0 / (alpha / vertexes[0].w() + beta / vertexes[1].w() + gamma / vertexes[2].w());
        alpha  = alpha / vertexes[0].w() * (-w_reciprocal);
        beta  = beta / vertexes[1].w() * (-w_reciprocal);
        gamma  = gamma / vertexes[2].w() * (-w_reciprocal);
        return { alpha, beta, gamma };
    }

    template<class T>
    T
    attributeLerp(numberType alpha, numberType beta, numberType gamma, T a, T b, T c) {
        return alpha * a + beta * b + gamma * c;
    }

    // �����Χ��
    static std::tuple<int, int, int, int>
    getBoundingBox(Vector4 a, Vector4 b, Vector4 c) {
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
