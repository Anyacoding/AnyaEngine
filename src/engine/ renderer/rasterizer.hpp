//
// Created by Anya on 2022/12/5.
//

#ifndef ANYA_ENGINE_RASTERIZER_HPP
#define ANYA_ENGINE_RASTERIZER_HPP

#include "interface/renderer.hpp"
#include "math/matrix.hpp"

// ��ģ��ʵ��������Ĺ�դ��������Ⱦ��

namespace anya {

class Rasterizer: public Renderer {
private:
    Matrix44 MVP;       // MVP�任����
    Matrix44 viewPort;  // �ӿڱ任����
public:
    void render() override {

    }
};

}

#endif //ANYA_ENGINE_RASTERIZER_HPP
