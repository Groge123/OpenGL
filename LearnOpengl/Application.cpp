#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include "Shader.h"

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) {GLClearError();\
    x;\
    ASSERT(GLLogCall(#x,__FILE__,__LINE__))};
//清除之前的错误
void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}
//检查当前是否有错误
bool GLLogCall(const char*function,const char*file,const GLuint line)
{
	if (GLuint error=glGetError())
	{
		std::cout << "GL_ERROR IS (" << error << "):" << function <<
			" " << file << " " << line << std::endl;
		return false;
	}
	return true;
}

//顶点缓冲区对象
class VertexBuffer
{
private:
	unsigned int m_vertexBuffer;
public:
	VertexBuffer(const void*data,const unsigned int size )
	{
		glGenBuffers(1, &m_vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}
	~VertexBuffer()
	{
		glDeleteBuffers(1, &m_vertexBuffer);
	}
	void Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	}
	void UnBind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

};
//索引缓冲区对象
class IndicesBuffer
{
private:
	unsigned int m_indicesBuffer;
public:
	IndicesBuffer(const void* indices, unsigned int count)
	{
		glGenBuffers(1, &m_indicesBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint)*count, indices, GL_STATIC_DRAW);
	}
	~IndicesBuffer()
	{
		glDeleteBuffers(1, &m_indicesBuffer);
	}
	void Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesBuffer);
	}
	void UnBind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
};


//创建着色器program
unsigned int CreateShader(const GLuint&vertexShader, const GLuint& fragmentShader)
{
	unsigned int shaderprogram;
	shaderprogram = glCreateProgram();
	GLCall(glAttachShader(shaderprogram, vertexShader));
	GLCall(glAttachShader(shaderprogram, fragmentShader));
	GLCall(glLinkProgram(shaderprogram));

	return shaderprogram;

}

int main()
{
	GLFWwindow* window;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window=glfwCreateWindow(1080, 720, "hello", nullptr,nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	if (glewInit() != GLEW_OK) std::cout << "GLEW_ERROR" << std::endl;
	//配置着色器
	Shader shader("Basic.Shader");
	//
	float positions[] = {
		// 位置              // 颜色
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
		-0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   // 左上
		 0.5f,  0.5f, 0.0f,  0.5f, 0.5f, 0.0f    // 右上
	};
	unsigned int indices[] =
	{
		1,0,3,
		3,2,1
	};
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	VertexBuffer vb(positions, sizeof(positions));
	

	// 位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 颜色属性
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	IndicesBuffer ib(indices, 6);

	/*unsigned int location;
	location = glGetUniformLocation(shaderprogram, "u_Color"); 
	if (location ==-1) std::cout << "SHADER_ERROR" << std::endl;
	glUniform4f(location, 1.0f, 0.0f, 0.0f, 1.0f);*/

	vb.UnBind();
	ib.UnBind();
	glBindVertexArray(0);

	float temp = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(1.0f, 0.5f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		static float b = 0.0f;
		//glUniform4f(location, 0.5f, 0.2f, b, 1.0f);
		shader.use();
		glBindVertexArray(vao);
		vb.Bind();
		ib.Bind();
		
		
		/*shader.setFloat("offsetx", b);
		if (b > 0.5f) temp = -0.01f;
		else if (b <= 0.0f) temp = 0.01f;
		b += temp;*/

		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL));

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

}