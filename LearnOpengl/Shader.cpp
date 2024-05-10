#include "Shader.h"


//获取着色器文件
ShaderSource GetShaderSource(const std::string& filepath)
{
	std::ifstream stream(filepath);
	std::string line;
	std::stringstream str[2];
	enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };
	ShaderType type = ShaderType::NONE;
	while (std::getline(stream, line))
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
		else str[(int)type] << line << '\n';

	}std::cout << str[0].str() << std::endl;
	std::cout << str[1].str() << std::endl;
	return { str[0].str(),str[1].str() };
}


Shader::Shader(const char* filepath)
{
	ShaderSource shadersource = GetShaderSource(filepath);
	const char* vertexshader = shadersource.VertexShader.c_str();
	const char* fragmentshader = shadersource.FragmentShader.c_str();
	// 2. compile shaders
	unsigned int vertex, fragment;
	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexshader, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentshader, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");
	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use()
{
	glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" 
				<< infoLog << "\n " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" 
				<< infoLog << "\n " << std::endl;
		}
	}
}
