#include "VertexBuffer.h"
#include "Renderer.h"

VertexBuffer::VertexBuffer(const void*data,unsigned int size)
{
    GLCall(glGenBuffers(1, &m_RendererID));
    // 将缓冲区绑定为一个数组缓冲区
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
    // 将顶点数据复制到缓冲区的内存中
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));

}

VertexBuffer::~VertexBuffer()
{
    GLCall(glDeleteBuffers(1,&m_RendererID));
}

void VertexBuffer::Bind()
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER,m_RendererID));
}

void VertexBuffer::UnBind()
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER,0));
}
