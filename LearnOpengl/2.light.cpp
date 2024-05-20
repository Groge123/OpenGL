#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include "Shader.h"
#include "stb_image.h"
#include "Texture.h"
#include "Camera.h"
// settings
const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 720;
float angle = 45.0f;
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
// camera
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) {GLClearError();\
    x;\
    ASSERT(GLLogCall(#x,__FILE__,__LINE__))};
//��Ϣ������
//����
void KeyInput(GLFWwindow* window);
//���
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//���ڴ�С�ص�����
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//���֮ǰ�Ĵ���
void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}
//��鵱ǰ�Ƿ��д���
bool GLLogCall(const char* function, const char* file, const GLuint line)
{
	if (GLuint error = glGetError())
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
	VertexBuffer(const void* data, const unsigned int size)
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
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * count, indices, GL_STATIC_DRAW);
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

int main()
{
	GLFWwindow* window;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1080, 720, "OpenGL", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	//֡����ʾ
	glfwSwapInterval(1);
	//�������
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//�ص�����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (glewInit() != GLEW_OK) std::cout << "GLEW_ERROR" << std::endl;
	//������ɫ��
	Shader light("light.glsl");
	Shader lightCube("lightCube.glsl");
	//��������
	float positions[] = {
		//     ---- λ�� ----    ������         - �������� -
		-0.5f, -0.5f, -0.5f, 0.0f,0.0f,-1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 0.0f,0.0f,-1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, 0.0f,0.0f,-1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f, 0.0f,0.0f,-1.0f, 0.0f, 1.0f,

		-0.5f, -0.5f,  0.5f, 0.0f,0.0f,1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 0.0f,0.0f,1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 0.0f,0.0f,1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, 0.0f,0.0f,1.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,0.0f,0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,0.0f,0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,0.0f,0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,0.0f,0.0f, 0.0f, 0.0f,

		 0.5f,  0.5f,  0.5f, 1.0f,0.0f,0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, 1.0f,0.0f,0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f, 1.0f,0.0f,0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f, 1.0f,0.0f,0.0f,  0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 0.0f,-1.0f,0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f, 0.0f,-1.0f,0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f, 0.0f,-1.0f,0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 0.0f,-1.0f,0.0f, 0.0f, 0.0f,

		-0.5f,  0.5f,  -0.5f, 0.0f,1.0f,0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  -0.5f, 0.0f,1.0f,0.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,   0.5f, 0.0f,1.0f,0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,   0.5f, 0.0f,1.0f,0.0f, 0.0f, 0.0f,
	};
	//��������
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);


	IndicesBuffer ib(indices, sizeof(indices) / sizeof(indices[0]));
	std::cout << sizeof(indices) / sizeof(indices[0]) << std::endl;
	
	glBindVertexArray(0);
	
	
	unsigned int lightvao;
	glGenVertexArrays(1, &lightvao);
	glBindVertexArray(lightvao);
	vb.Bind();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	IndicesBuffer ib2(indices, sizeof(indices) / sizeof(indices[0]));
	//��Ȳ���
	glEnable(GL_DEPTH_TEST);
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	// positions all containers
	glm::vec3 cubePositions[] = {
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

	Texture texture("woodBox.png", GL_RGBA);
	Texture texture1("Boxspecular.png", GL_RGBA);
	Texture texture2("matrix.jpg", GL_RGB);

	light.use();
	light.setInt("material.diffuse", 0);
	light.setInt("material.specular", 1);
	//light.setInt("material.emission", 2);
	while (!glfwWindowShouldClose(window))
	{
		KeyInput(window);
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		

		light.use();
		// material properties
		
		light.setFloat("material.shininess", 64.0f);

		light.setVec3("light.position", lightPos);
		light.setVec3("viewPos", camera.Position);

		// light properties
		light.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		light.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
		light.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		light.setVec3("light.position", lightPos);
		light.setVec3("viewPos",camera.Position);
		//light.setVec3("lightColor",lightColor);
		
		// view/projection transformations
		
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		light.setMat4("projection", projection);
		light.setMat4("view", view);
		glm::mat4 model = glm::mat4(1.0f);
		// world transformation
		for (int i = 0; i < 10; i++)
		{
			
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model,float(i), glm::vec3(1.0f, 0.0f, 0.0f));
			light.setMat4("model", model);
			// render the cube
			
			glActiveTexture(GL_TEXTURE0);
			texture.Bind();
			glActiveTexture(GL_TEXTURE1);
			texture1.Bind();
			/*glActiveTexture(GL_TEXTURE2);
			texture2.Bind();*/
			glBindVertexArray(vao); 
			GLCall(glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, NULL));
		}
		

		// also draw the lamp object
		lightCube.use();
		lightCube.setVec3("lightColor", lightColor);
		lightCube.setMat4("projection", projection);
		lightCube.setMat4("view", view);
		model = glm::mat4(1.0f);
		lightPos = glm::vec3(sin(glfwGetTime()*0.5), sin(glfwGetTime() * 0.5),cos(glfwGetTime()*0.5));
		//lightPos = glm::vec3(0.0f, 0.0f, 2.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		lightCube.setMat4("model", model);

		GLCall(glBindVertexArray(lightvao));
		GLCall(glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, NULL));

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//���ٴ���
	glfwTerminate();
}
//��Ϣ����
void KeyInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);

}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
	std::cout << yoffset << std::endl;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}