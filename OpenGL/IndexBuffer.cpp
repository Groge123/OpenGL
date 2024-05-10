#include "Renderer.h"
#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
{
    GLCall(glGenBuffers(1, &m_RendererID));
    // 将缓冲区绑定为一个数组缓冲区
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
    // 将顶点数据复制到缓冲区的内存中
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*sizeof(GLuint), data, GL_STATIC_DRAW));

}

IndexBuffer::~IndexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID));
}

void IndexBuffer::Bind()
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
}

void IndexBuffer::UnBind()
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
