#include <GL/glew.h> // GLEW头文件，用于获取OpenGL函数指针
#include <GLFW/glfw3.h> // GLFW头文件，用于创建窗口和上下文
#include <iostream> // 输入输出流库
#include <fstream> // 文件流库，用于读取着色器文件
#include <sstream> // 字符串流库，用于处理字符串
#include <string> // 字符串库
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

// 定义一个结构体来存储顶点和片段着色器的源代码
struct ShaderProgramSource
{
    std::string VertexSource; // 顶点着色器源代码
    std::string FragmentSource; // 片段着色器源代码
};

// 解析着色器文件的函数
static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath); // 打开着色器文件

    // 定义一个枚举来表示着色器类型
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line; // 存储文件中的一行
    std::stringstream ss[2]; // 两个字符串流，一个用于顶点着色器，一个用于片段着色器
    ShaderType type = ShaderType::NONE; // 当前着色器类型
    while (std::getline(stream, line)) // 逐行读取文件
    {

        if (line.find("#shader") != std::string::npos) // 如果行中包含"#shader"
        {
            if (line.find("vertex") != std::string::npos) // 如果是顶点着色器
            {
                type = ShaderType::VERTEX; // 设置当前类型为顶点着色器
            }
            else if (line.find("fragment") != std::string::npos) // 如果是片段着色器
            {
                type = ShaderType::FRAGMENT; // 设置当前类型为片段着色器
            }
        }
        else
        {
            ss[(int)type] << line << '\n'; // 将行添加到相应的字符串流中
        }
        
    }
    return { ss[0].str(),ss[1].str() }; // 返回着色器源代码
}

// 编译着色器的函数
unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type); // 创建着色器对象
    const char* src = source.c_str(); // 将源代码转换为C风格字符串
    glShaderSource(id, 1, &src, nullptr); // 绑定源代码到着色器对象
    glCompileShader(id); // 编译着色器

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); // 检查编译状态
    if (result == GL_FALSE)
    {
        int lenth;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenth); // 获取错误信息长度
        char* message = (char*)alloca(sizeof(char) * lenth); // 分配内存来存储错误信息
        glGetShaderInfoLog(id, lenth, &lenth, message); // 获取错误信息
        std::cout << "Failed to compile shader!" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") // 输出错误信息
            << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id); // 删除着色器对象
        return 0;
    }

    return id; // 如果编译成功，返回着色器对象ID
}

// 创建着色器程序的函数
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram(); // 创建程序对象
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader); // 编译顶点着色器
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader); // 编译片段着色器

    glAttachShader(program, vs); // 将顶点着色器附加到程序对象
    glAttachShader(program, fs); // 将片段着色器附加到程序对象
    glLinkProgram(program); // 链接程序
    glValidateProgram(program); // 验证程序

    glDeleteShader(vs); // 删除顶点着色器对象
    glDeleteShader(fs); // 删除片段着色器对象

    return program; // 返回程序对象ID
}

int main(void)
{
    GLFWwindow* window;
    // 初始化GLFW库
    if (!glfwInit())
        return -1;

    // 创建一个窗口模式窗口及其OpenGL上下文
    window = glfwCreateWindow(1080, 720, "Hello World", NULL, NULL);


    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // 使窗口的上下文成为当前上下文
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    // 初始化GLEW以获取OpenGL函数的访问权限
    if (glewInit() != GLEW_OK)
        std::cout << "ERROR" << std::endl;
    {
        // 定义三角形的顶点位置
        float position[] =
        {
            -0.5f, -0.5f,
             0.5f, -0.5f,
             0.5f,  0.5f,
            -0.5f,  0.5f,
        };

        unsigned int indeces[] =
        {
            0,1,2,
            2,3,0
        };
        unsigned int vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // 生成一个缓冲对象，并将其ID存储在'buffer'中
        VertexBuffer vb(position, sizeof(position));

        // 启用索引为0的顶点属性数组
        glEnableVertexAttribArray(0);
        // 定义顶点数据的布局 (每个顶点2个浮点数)
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

        IndexBuffer ib(indeces, 6);

        // 从文件中解析着色器源代码
        ShaderProgramSource source = ParseShader("Basic.shader");
        // 编译并创建着色器程序
        unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
        // 使用创建的着色器程序进行渲染
        GLCall(glUseProgram(shader));

        GLint location = glGetUniformLocation(shader, "u_Color");

        ASSERT(location != -1);
        GLCall(glUniform4f(location, 1.0f, 0.2f, 0.5f, 1.0f));

        GLCall(glUseProgram(0));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
        GLCall(glBindVertexArray(0));

        float r = 1.0f;
        float increment = 0.0f;


        // 循环直到用户关闭窗口
        while (!glfwWindowShouldClose(window))
        {
            // 在这里渲染
            glClearColor(0.0f, 0.2f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT); // 清除颜色缓冲区
            GLCall(glUseProgram(shader));
            GLCall(glUniform4f(location, r, 0.2f, 0.5f, 1.0f));

            vb.Bind();
            GLCall(glBindVertexArray(vao));
            ib.Bind();


            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL));

            if (r >= 1.0f) increment = -0.01f;
            else if (r <= 0.0f) increment = 0.01f;

            r += increment;
            // 交换前缓冲区和后缓冲区
            glfwSwapBuffers(window);
            // 轮询并处理事件
            glfwPollEvents();
        }

        GLCall(glDeleteProgram(shader));
    }
    // 终止GLFW
    glfwTerminate();
    return 0;
}
