//
// Created by Anya on 2022/12/5.
//

#ifndef ANYA_ENGINE_RASTERIZER_HPP
#define ANYA_ENGINE_RASTERIZER_HPP

#include "interface/renderer.hpp"
#include "math/matrix.hpp"
#include "object/triangle.hpp"

// ��ģ��ʵ��������Ĺ�դ��������Ⱦ��

namespace anya {

class Rasterizer: public Renderer {
private:
    Matrix44 MVP;          // MVP�任����

    GLdouble view_width = 0.0, view_height = 0.0;  // �Ӵ�
public:
    void
    render() override {
        std::tie(view_width, view_height) = scene.camera->getWH();

        auto viewMat = scene.camera->getViewMat();
        auto projectionMat = scene.camera->getProjectionMat();

        // ��Ⱦÿ��model
        for (auto& model : scene.models) {
            auto modelMat = model.modelMat;   // ��ȡÿ��model��modelMat
            MVP =  projectionMat * viewMat * modelMat;
            std::vector<anya::Vector4> worldPositions;
            for (const auto& local : model.localPositions) {
                worldPositions.push_back(MVP * local.to4());
            }
            // TODO ÿ����������Ҫ��������Ϊ������opengl��api,�����Ż�
            model.vertexes.resize(0);
            for (auto& worldPos : worldPositions) {
                worldPos /= worldPos.w();
                model.vertexes.push_back(worldPos.to<3>());
            }
            for (auto& worldPos : worldPositions) {
                worldPos /= worldPos.w();
                model.vertexes.push_back(worldPos.to<3>());
            }
        }
    }
};

}

#endif //ANYA_ENGINE_RASTERIZER_HPP