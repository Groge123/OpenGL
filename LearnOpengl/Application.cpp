#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include "Shader.h"
#include "stb_image.h"
#include "Texture.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float angle = 45.0f;

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) {GLClearError();\
    x;\
    ASSERT(GLLogCall(#x,__FILE__,__LINE__))};
//��Ϣ������
void processInput(GLFWwindow * window);
//���֮ǰ�Ĵ���
void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}
//��鵱ǰ�Ƿ��д���
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


//���㻺��������
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
//��������������
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
	//������ɫ��
	Shader shader("Basic.Shader");
	//��������
	float positions[] = {
		//     ---- λ�� ----    - �������� -
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	};
	//��������261 156
	unsigned int indices[] =
	{
				// ǰ��
		  0, 1, 2, // ���½ǵ����½ǵ����Ͻ�
		  2, 3, 0, // ���Ͻǵ����Ͻǵ����½�

		  // ����
		  4, 5, 6, // ���½ǵ����½ǵ����Ͻ�
		  6, 7, 4, // ���Ͻǵ����Ͻǵ����½�

		  // �����
		  8, 9, 10, // ���½ǵ����½ǵ����Ͻ�
		  10,11, 8, // ���Ͻǵ����Ͻǵ����½�

		  // �Ҳ���
		  12, 13, 14, // ���½ǵ����½ǵ����Ͻ�
		  14, 15, 12, // ���Ͻǵ����Ͻǵ����½�

		  // ����
		  16, 17, 18, // ���½ǵ����½ǵ����Ͻ�
		  18, 19, 16, // ���Ͻǵ����Ͻǵ����½�

		  // ����
		  20, 21, 22, // ���½ǵ����½ǵ����Ͻ�
		  22, 23, 20, // ���Ͻǵ����Ͻǵ����½�
	};
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	VertexBuffer vb(positions, sizeof(positions));
	// λ������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// ��ɫ����
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	IndicesBuffer ib(indices, sizeof(indices)/sizeof(indices[0]));
	std::cout << sizeof(indices) / sizeof(indices[0]) << std::endl;

	//��������
	Texture texture1("container.jpg");

	Texture texture2("awesomeface.png",GL_RGBA);

	
	//trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
	

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

	glEnable(GL_DEPTH_TEST);

	glm::vec3 translate[] = {
		  glm::vec3(0.0f,  0.0f,  0.0f),
		  glm::vec3(2.0f,  5.0f, -15.0f),
		  glm::vec3(-1.5f, -2.2f, -2.5f),
		  glm::vec3(-3.8f, -2.0f, -12.3f),
		  glm::vec3(2.4f, -0.4f, -3.5f),
		  glm::vec3(-1.7f,  3.0f, -7.5f),
		  glm::vec3(1.3f, -2.0f, -2.5f),
		  glm::vec3(1.5f,  2.0f, -2.5f),
		  glm::vec3(1.5f,  0.2f, -1.5f),
		  glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		glClearColor(0.4f, 0.3f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glActiveTexture(GL_TEXTURE0);
		texture1.Bind();
		glActiveTexture(GL_TEXTURE1);
		texture2.Bind();

		static float b = 0.0f;
		shader.setFloat("mixValue", mixValue);
		shader.use();
		glBindVertexArray(vao);
		vb.Bind();
		ib.Bind();

		glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(angle), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		// pass transformation matrices to the shader
		shader.setMat4("view", view);
		shader.setMat4("projection",projection);
		//�˷��෴��˳��
		//trans = glm::translate(trans, glm::vec3(-0.5, 0.0, 0.0));
		for (int i = 0; i < 10; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, translate[i]);
			float angle = 20.0f * i;
			if(i==1||i%3==0)
			model = glm::rotate(model, glm::radians(angle)+(float)glfwGetTime(), glm::vec3(1.0f, 0.3f, 0.5f));
			
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			shader.setMat4("model", model);

			GLCall(glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, NULL));
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//���ٴ���
	glfwTerminate();
}
//��Ϣ����
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		angle += 1;
		mixValue += 0.01f; // change this value accordingly (might be too slow or too fast based on system hardware)
		if (mixValue >= 1.0f)
			mixValue = 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		angle -= 1;
		mixValue -= 0.01f; // change this value accordingly (might be too slow or too fast based on system hardware)
		if (mixValue <= 0.0f)
			mixValue = 0.0f;
	}
}