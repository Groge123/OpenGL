#pragma once
#include <iostream>
#include <gl\glew.h>; // ����glad����ȡ���еı���OpenGLͷ�ļ�

class Texture
{
private:
	unsigned int ID;

public:
	Texture(const char* filepath);
	~Texture();
};