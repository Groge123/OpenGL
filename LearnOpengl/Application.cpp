#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include "Shader.h"
#include "stb_image.h"
#include "Texture.h"

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) {GLClearError();\
    x;\
    ASSERT(GLLogCall(#x,__FILE__,__LINE__))};
//消息处理函数
void processInput(GLFWwindow * window);
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




float mixValue = 0.2;

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
	//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
		 0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   2.0f, 0.0f,   // 右下
	 	-0.5f, -0.5f, 0.0f,   0.8f, 0.8f, 0.8f,   0.0f, 0.0f,   // 左下
		-0.5f,  0.5f, 0.0f,   0.8f, 0.8f, 0.8f,   0.0f, 2.0f,   // 左上
		 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.8f,   2.0f, 2.0f,   // 右上
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 颜色属性
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	
	IndicesBuffer ib(indices, 6);

	//纹理配置
	Texture texture1("container.jpg");

	Texture texture2("awesomeface.png",GL_RGBA);


	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	shader.use(); // don't forget to activate/use the shader before setting uniforms!
	// either set it manually like so:
	glUniform1i(glGetUniformLocation(shader.ID, "texture1"), 0);
	// or set it via the texture class
	shader.setInt("texture2", 1);

	vb.UnBind();
	ib.UnBind();
	glBindVertexArray(0);

	float temp = 0.004f;
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		glClearColor(1.0f, 0.5f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glActiveTexture(GL_TEXTURE0);
		texture1.Bind();
		glActiveTexture(GL_TEXTURE1);
		texture2.Bind();
		static float b = 0.0f;
		
		shader.use();
		glBindVertexArray(vao);
		vb.Bind();
		ib.Bind();
		
		
		shader.setFloat("offsetx", b);
		shader.setFloat("mixValue", mixValue);
		if (b > 0.5f) temp = -0.004f;
		else if (b < -0.5f) temp = 0.004f; 
		b += temp;

		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL));

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//销毁窗口
	glfwTerminate();
}
//消息处理
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mixValue += 0.001f; // change this value accordingly (might be too slow or too fast based on system hardware)
		if (mixValue >= 1.0f)
			mixValue = 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mixValue -= 0.001f; // change this value accordingly (might be too slow or too fast based on system hardware)
		if (mixValue <= 0.0f)
			mixValue = 0.0f;
	}
}