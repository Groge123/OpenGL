#include <GL/glew.h> // GLEWͷ�ļ������ڻ�ȡOpenGL����ָ��
#include <GLFW/glfw3.h> // GLFWͷ�ļ������ڴ������ں�������
#include <iostream> // �����������
#include <fstream> // �ļ����⣬���ڶ�ȡ��ɫ���ļ�
#include <sstream> // �ַ������⣬���ڴ����ַ���
#include <string> // �ַ�����
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

// ����һ���ṹ�����洢�����Ƭ����ɫ����Դ����
struct ShaderProgramSource
{
    std::string VertexSource; // ������ɫ��Դ����
    std::string FragmentSource; // Ƭ����ɫ��Դ����
};

// ������ɫ���ļ��ĺ���
static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath); // ����ɫ���ļ�

    // ����һ��ö������ʾ��ɫ������
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line; // �洢�ļ��е�һ��
    std::stringstream ss[2]; // �����ַ�������һ�����ڶ�����ɫ����һ������Ƭ����ɫ��
    ShaderType type = ShaderType::NONE; // ��ǰ��ɫ������
    while (std::getline(stream, line)) // ���ж�ȡ�ļ�
    {

        if (line.find("#shader") != std::string::npos) // ������а���"#shader"
        {
            if (line.find("vertex") != std::string::npos) // ����Ƕ�����ɫ��
            {
                type = ShaderType::VERTEX; // ���õ�ǰ����Ϊ������ɫ��
            }
            else if (line.find("fragment") != std::string::npos) // �����Ƭ����ɫ��
            {
                type = ShaderType::FRAGMENT; // ���õ�ǰ����ΪƬ����ɫ��
            }
        }
        else
        {
            ss[(int)type] << line << '\n'; // ������ӵ���Ӧ���ַ�������
        }
        
    }
    return { ss[0].str(),ss[1].str() }; // ������ɫ��Դ����
}

// ������ɫ���ĺ���
unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type); // ������ɫ������
    const char* src = source.c_str(); // ��Դ����ת��ΪC����ַ���
    glShaderSource(id, 1, &src, nullptr); // ��Դ���뵽��ɫ������
    glCompileShader(id); // ������ɫ��

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); // ������״̬
    if (result == GL_FALSE)
    {
        int lenth;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenth); // ��ȡ������Ϣ����
        char* message = (char*)alloca(sizeof(char) * lenth); // �����ڴ����洢������Ϣ
        glGetShaderInfoLog(id, lenth, &lenth, message); // ��ȡ������Ϣ
        std::cout << "Failed to compile shader!" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") // ���������Ϣ
            << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id); // ɾ����ɫ������
        return 0;
    }

    return id; // �������ɹ���������ɫ������ID
}

// ������ɫ������ĺ���
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram(); // �����������
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader); // ���붥����ɫ��
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader); // ����Ƭ����ɫ��

    glAttachShader(program, vs); // ��������ɫ�����ӵ��������
    glAttachShader(program, fs); // ��Ƭ����ɫ�����ӵ��������
    glLinkProgram(program); // ���ӳ���
    glValidateProgram(program); // ��֤����

    glDeleteShader(vs); // ɾ��������ɫ������
    glDeleteShader(fs); // ɾ��Ƭ����ɫ������

    return program; // ���س������ID
}

int main(void)
{
    GLFWwindow* window;
    // ��ʼ��GLFW��
    if (!glfwInit())
        return -1;

    // ����һ������ģʽ���ڼ���OpenGL������
    window = glfwCreateWindow(1080, 720, "Hello World", NULL, NULL);


    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // ʹ���ڵ������ĳ�Ϊ��ǰ������
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    // ��ʼ��GLEW�Ի�ȡOpenGL�����ķ���Ȩ��
    if (glewInit() != GLEW_OK)
        std::cout << "ERROR" << std::endl;
    {
        // ���������εĶ���λ��
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

        // ����һ��������󣬲�����ID�洢��'buffer'��
        VertexBuffer vb(position, sizeof(position));

        // ��������Ϊ0�Ķ�����������
        glEnableVertexAttribArray(0);
        // ���嶥�����ݵĲ��� (ÿ������2��������)
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

        IndexBuffer ib(indeces, 6);

        // ���ļ��н�����ɫ��Դ����
        ShaderProgramSource source = ParseShader("Basic.shader");
        // ���벢������ɫ������
        unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
        // ʹ�ô�������ɫ�����������Ⱦ
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


        // ѭ��ֱ���û��رմ���
        while (!glfwWindowShouldClose(window))
        {
            // ��������Ⱦ
            glClearColor(0.0f, 0.2f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT); // �����ɫ������
            GLCall(glUseProgram(shader));
            GLCall(glUniform4f(location, r, 0.2f, 0.5f, 1.0f));

            vb.Bind();
            GLCall(glBindVertexArray(vao));
            ib.Bind();


            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL));

            if (r >= 1.0f) increment = -0.01f;
            else if (r <= 0.0f) increment = 0.01f;

            r += increment;
            // ����ǰ�������ͺ󻺳���
            glfwSwapBuffers(window);
            // ��ѯ�������¼�
            glfwPollEvents();
        }

        GLCall(glDeleteProgram(shader));
    }
    // ��ֹGLFW
    glfwTerminate();
    return 0;
}
