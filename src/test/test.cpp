//
// Created by Anya on 2022/12/3.
//
#include "test.h"

void vecTest() {
    {
        anya::Vector2 vec2 = anya::make_Vec(1, 2);
        anya::Vector3 vec3{ 3, 4, 0 };

        std::cout << "测试向量的加减乘除:" << std::endl;
        std::cout << vec2 << std::endl;
        std::cout << (vec3 *= 3) << std::endl;
        std::cout << 3 * vec3 << std::endl;
        std::cout << (vec3 /= 3) << std::endl;
        std::cout << -vec3 << std::endl
                  << std::endl;

        std::cout << "测试向量的点乘及其特殊运算:" << std::endl;
        std::cout << vec3.dot(vec3) << std::endl;
        std::cout << vec3.norm2() << std::endl;
        std::cout << vec3.normalize().norm2() << std::endl;
        std::cout << vec2.angle({ 0, 1 }) << std::endl
                  << std::endl;
    }

    {
        std::cout << "测试叉积:" << std::endl;
        anya::Vector3 a{ 1, 0, 0 };
        anya::Vector3 b{ 0, 1, 0 };
        std::cout << a.cross(b) << std::endl;
        a = { 1, 1, 0 };
        b = { -1, 1, 1 };
        std::cout << a.cross(b) << std::endl
                  << std::endl;
    }

    {
        std::cout << "测试四元数归一化:" << std::endl;
        anya::Vector4 vec4{ 1.0, 2.0, 3.0, 2.0 };
        std::cout << vec4.trim() << std::endl << std::endl;
    }

    {
        std::cout << "测试三维向量转为齐次坐标向量:" << std::endl;
        anya::Vector3 vec3{ 1.0, 2.0, 3.0 };
        std::cout << vec3.to4() << std::endl;
        std::cout << vec3.to4(0.0) << std::endl;
    }
}

void matrixTest() {
    {
        anya::Matrix<3, 4> mat{};
        std::cout << "测试逗号初始化和访问:" << std::endl;
        mat << 1, 2, 3, 5,
            4, 5, 6, 9,
            11, 12, 13, 18;
        std::cout << mat;
        std::cout << mat(2, 3) << std::endl;
        std::cout << mat.rowVec(2) << std::endl;
        std::cout << mat.colVec(2) << std::endl
                  << std::endl;
    }

    {
        std::cout << "测试矩阵乘法:" << std::endl;
        anya::Matrix<2, 2> lhs{};
        lhs << 1, 1,
            2, 0;
        anya::Matrix<2, 3> rhs{};
        rhs << 0, 2, 3,
            1, 1, 2;
        std::cout << lhs * rhs << std::endl;

        anya::Matrix<1, 3> lhs2{};
        lhs2 << 8, 8, 6;
        anya::Matrix<3, 2> rhs2{};
        rhs2 << 5, 2,
            1, 3,
            6, 5;
        std::cout << lhs2 * rhs2 << std::endl;

        anya::Matrix<4, 3> A{};
        A << 2, 3, 1,
            3, 8, -2,
            4, -1, 9,
            1, -2, 4;
        anya::Vector3 x{ -1, 2, 0 };
        std::cout << A * x << std::endl
                  << std::endl;
    }

    {
        std::cout << "测试矩阵加减法和系数乘:" << std::endl;
        anya::Matrix<4, 3> lhs{};
        lhs << 2, 3, 1,
            3, 8, -2,
            4, -1, 9,
            1, -2, 4;
        anya::Matrix<4, 3> rhs{};
        rhs << 4, 3, 3,
            3, 2, -2,
            5, -1, 3,
            1, -23, 2;
        std::cout << lhs + rhs << std::endl;
        std::cout << lhs - rhs << std::endl;
        std::cout << (lhs += rhs) << std::endl;
        std::cout << (lhs -= rhs) << std::endl;
        std::cout << lhs * 25 << std::endl;
        std::cout << 25 * lhs  << std::endl;
        std::cout << lhs / 2 << std::endl;
        std::cout << (lhs *= 25) << std::endl;
        std::cout << (lhs /= 25) << std::endl << std::endl;
    }

    {
        std::cout << "测试矩阵转置:" << std::endl;
        anya::Matrix<4, 3> lhs{};
        lhs << 2, 3, 1,
            3, 8, -2,
            4, -1, 9,
            1, -2, 4;
        anya::Matrix<4, 3> rhs{};
        rhs << 4, 3, 3,
            3, 2, -2,
            5, -1, 3,
            1, -23, 2;
        std::cout << lhs.transpose() << std::endl;
        std::cout << rhs.transpose() << std::endl;
    }

    {
        std::cout << "测试矩阵行列式:" << std::endl;
        anya::Matrix<4, 4> lhs{};
        lhs << 2, 3, 1, 6,
            3, 9, -2, 8,
            4, -1, 9, 4,
            1, -2, 4, 7;
        anya::Matrix<5, 5> rhs{};
        rhs << 1, 1, 1, 2, 1,
            3, 3, 3, 1, 1,
            0, 1, 2, 2, 3,
            1, 1, 1, 0, 0,
            1, 1, 0, 5, 7;

        std::cout << lhs.det() << std::endl;
        std::cout << rhs.det() << std::endl << std::endl;
    }

    {
        std::cout << "测试矩阵求逆:" << std::endl;
        anya::Matrix<4, 4> lhs{};
        lhs << 2, 3, 1, 6,
            3, 9, -2, 8,
            4, -1, 9, 4,
            1, -2, 4, 7;
        anya::Matrix<5, 5> rhs{};
        rhs << 1, 1, 1, 2, 1,
            3, 3, 3, 1, 1,
            0, 1, 2, 2, 3,
            1, 1, 1, 0, 0,
            1, 1, 0, 5, 7;

        std::cout << rhs.inverse() << std::endl;
        std::cout << rhs.inverse() * rhs << std::endl;
    }

    {
        std::cout << "测试3*3矩阵转换为齐次坐标:" << std::endl;
        anya::Matrix<3, 3> lhs{};
        lhs << 2, 3, 1,
            3, 8, -2,
            4, -1, 9;
        anya::Matrix<5, 5> rhs{};
        rhs << 1, 1, 1, 2, 1,
            3, 3, 3, 1, 1,
            0, 1, 2, 2, 3,
            1, 1, 1, 0, 0,
            1, 1, 0, 5, 7;
        anya::Matrix44 mat44 = lhs.to44();
        std::cout << mat44 << std::endl;
        std::cout << rhs.to<3, 3>() << std::endl;
        std::cout << rhs.colVec(3) << std::endl << std::endl;
    }

    {
        std::cout << "测试替换矩阵特定行、列:" << std::endl;
        anya::Matrix<5, 5> rhs{};
        rhs << 1, 1, 1, 2, 1,
               3, 3, 3, 1, 1,
               0, 1, 2, 2, 3,
               1, 1, 1, 0, 0,
               1, 1, 0, 5, 7;
        anya::Vector<5> vec{9, 8, 7, 6, 5};
        rhs.setRowVec(0, vec);
        std::cout << rhs << std::endl;
    }

    {
        std::cout << "测试单位矩阵:" << std::endl;
        anya::Matrix<5, 5> rhs = anya::Matrix<5, 5>::Identity();
        std::cout << rhs << std::endl;
    }

    {
        std::cout << "测试列向量和行向量相乘:" << std::endl;
        anya::Vector3 lhs{3, 2, 1};
        anya::Matrix<1, 3> rhs;
        rhs << 3, 2, 1;
        std::cout << lhs * rhs;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// settings
//const unsigned int SCR_WIDTH = 800;
//const unsigned int SCR_HEIGHT = 600;

//int testGlfw() {
//    //    // glfw: initialize and configure
//    //    // ------------------------------
//    if (glfwInit() == false) {
//        std::cerr << "ERROR: could not start GLFW3\n";
//        return -1;
//    }
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//
//    // glfw window creation
//    // --------------------
//    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
//    if (window == nullptr) {
//        std::cerr << "Failed to create GLFW window" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//
//    glfwMakeContextCurrent(window);
//    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//
//    // glad: load all OpenGL function pointers
//    // ---------------------------------------
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//        std::cerr << "Failed to initialize GLAD" << std::endl;
//        return -1;
//    }
//
//    // tell GL to only draw onto a pixel if the shape is closer to the viewer
//    glEnable(GL_DEPTH_TEST); // enable depth-testing
//    glDepthFunc(GL_LESS);    // depth-testing interprets a smaller value as "closer"
//
//    // 双缓冲交换间隔设置为1，以免交换频繁造成屏幕撕裂
//    glfwSwapInterval(1);
//
//    // 三角形三个顶点
//    anya::Matrix33 triangle{};
//    triangle << 0.0, 0.5, 0.0,
//        0.5, -0.5, 0.0,
//        -0.5, -0.5, 0.0;
//    double points[9]{};
//    triangle.saveToArray(points);
//    for (auto i : points) std::cout << i << " ";
//
//
//    // vertex buffer object (VBO) —— 顶点缓冲区对象的索引
//    GLuint vbo = 0;
//    // 创建一个空的vbo
//    glGenBuffers(1, &vbo);
//
//    // vertex array object (VAO) —— 顶点顶点数组对象的索引
//    GLuint vao = 0;
//    // 创建一个空的vao
//    glGenVertexArrays(1, &vao);
//    // 绑定vao对象至上下文
//    glBindVertexArray(vao);
//
//    // 绑定vbo对象至上下文的缓冲区
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    // 将点复制到当前绑定的vbo缓冲区
//    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(anya::numberType), points, GL_STATIC_DRAW);
//    // 0代表属性号为0的定义布局，3代表每个变量是由缓冲区中的每3个浮点(GL_FLOAT)组成的vec3
//    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 0, nullptr);
//    // 启用属性0
//    glEnableVertexAttribArray(0);
//    // 好习惯，解除绑定
//    glBindVertexArray(0);
//
//    const char* vertex_shader =
//        "#version 400\n"
//        "in vec3 vp;"
//        "void main() {"
//        "  gl_Position = vec4(vp, 1.0);"
//        "}";
//
//    const char* fragment_shader =
//        "#version 400\n"
//        "out vec4 frag_colour;"
//        "void main() {"
//        "  frag_colour = vec4(0.5, 0.0, 0.5, 1.0);"
//        "}";
//
//    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(vs, 1, &vertex_shader, nullptr);
//    glCompileShader(vs);
//    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fs, 1, &fragment_shader, nullptr);
//    glCompileShader(fs);
//
//    GLuint shader_programme = glCreateProgram();
//    glAttachShader(shader_programme, fs);
//    glAttachShader(shader_programme, vs);
//    glLinkProgram(shader_programme);
//
//    // render loop
//    // -----------
//    while (!glfwWindowShouldClose(window)) {
//        // input
//        // -----
//        processInput(window);
//
//        // render
//        // ------
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        glUseProgram(shader_programme);
//        glBindVertexArray(vao);
//
//        glDrawArrays(GL_TRIANGLES, 0, 3);
//        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
//        // -------------------------------------------------------------------------------
//        glfwPollEvents();
//        glfwSwapBuffers(window);  // 双缓冲区交换
//
//        glBindVertexArray(0);
//    }
//
//    // glfw: terminate, clearing all previously allocated GLFW resources.
//    // ------------------------------------------------------------------
//    glfwTerminate();
//
//    return 0;
//}