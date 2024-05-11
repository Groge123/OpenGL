#pragma once
#include <iostream>
#include <gl\glew.h>; // 包含glad来获取所有的必须OpenGL头文件

class Texture
{
private:
	unsigned int ID;

public:
	Texture(const char* filepath);
	~Texture();
};