#pragma once

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "Buffer.hpp"
#include "BufferView.hpp"
#include "BufferAccessor.hpp"
#include <vector>

namespace BufferHelper {
	/** 
	 * @brief Creates a buffer accessor with BufferView and Buffer from vector
	 * @argument target : the buffer's target, see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBindBuffer.xhtml
	 */
	template <typename T>
	std::shared_ptr<BufferAccessor> CreateAccessor(std::vector<T> bufferVector, GLenum target = GL_ARRAY_BUFFER, bool normalized = false);
	template <typename T>
	std::shared_ptr<BufferAccessor> CreateAccessor(size_t size, GLenum target = GL_ARRAY_BUFFER, bool normalized = false);
	template <typename T>
	std::shared_ptr<BufferView> CreateBufferView(std::vector<T> bufferVector, GLenum target = GL_ARRAY_BUFFER);
	template <typename T>
	std::shared_ptr<Buffer> CreateBuffer(std::vector<T> bufferVector);
	template <typename T>
	T Get(std::shared_ptr<BufferAccessor>, size_t index);
	template <typename T>
	T Get(std::shared_ptr<Buffer>, size_t index);
	template <typename T>
	void Set(std::shared_ptr<BufferAccessor>, size_t index, T value);
}

template <typename T>
inline std::shared_ptr<Buffer> BufferHelper::CreateBuffer(std::vector<T> bufferVector)
{
	auto byteLength(bufferVector.size() * sizeof(T));
	auto buffer(Buffer::Create(byteLength));
	memcpy(buffer->RawData().data(), bufferVector.data(), byteLength);
	return buffer;
}

template <typename T>
inline std::shared_ptr<BufferView> BufferHelper::CreateBufferView(std::vector<T> bufferVector, GLenum target)
{
	auto buffer(BufferHelper::CreateBuffer(bufferVector));
	auto bufferView(BufferView::Create(buffer->ByteLength(), buffer));
	bufferView->SetTarget(target);
	return bufferView;
}

template <>
inline std::shared_ptr<BufferAccessor> BufferHelper::CreateAccessor(std::vector<unsigned> bufferVector, GLenum target, bool normalized) {
	auto bufferView(BufferHelper::CreateBufferView(bufferVector, target));
	auto bufferAccessor(BufferAccessor::Create(GL_UNSIGNED_INT, bufferVector.size(), "SCALAR"));
	bufferAccessor->SetBufferView(bufferView);
	bufferAccessor->SetNormalized(normalized);
	return bufferAccessor;
}

template <>
inline std::shared_ptr<BufferAccessor> BufferHelper::CreateAccessor(std::vector<glm::vec3> bufferVector, GLenum target, bool normalized) {
	auto bufferView(BufferHelper::CreateBufferView(bufferVector, target));
	auto bufferAccessor(BufferAccessor::Create(GL_FLOAT, bufferVector.size(), "VEC3"));
	bufferAccessor->SetBufferView(bufferView);
	bufferAccessor->SetNormalized(normalized);
	return bufferAccessor;
}

template <>
inline std::shared_ptr<BufferAccessor> BufferHelper::CreateAccessor(std::vector<glm::vec2> bufferVector, GLenum target, bool normalized) {
	auto bufferView(BufferHelper::CreateBufferView(bufferVector, target));
	auto bufferAccessor(BufferAccessor::Create(GL_FLOAT, bufferVector.size(), "VEC2"));
	bufferAccessor->SetBufferView(bufferView);
	bufferAccessor->SetNormalized(normalized);
	return bufferAccessor;
}

template <typename T>
std::shared_ptr<BufferAccessor> BufferHelper::CreateAccessor(size_t size, GLenum target, bool normalized)
{
	return BufferHelper::CreateAccessor(std::vector<T>(size), target, normalized);
}

template <typename T>
inline T BufferHelper::Get(std::shared_ptr<Buffer> buffer, size_t index)
{
	if (buffer == nullptr)
		return T();
	if (index + sizeof(T) > buffer->ByteLength())
		throw std::runtime_error(std::string("Buffer index(") + std::to_string(index + sizeof(T)) + ") out of bound(" + std::to_string(buffer->ByteLength()) + ")");
	auto pointer(buffer->RawData().data() + index);
	return *reinterpret_cast<T*>(pointer);
}

template <typename T>
inline T BufferHelper::Get(std::shared_ptr<BufferAccessor> accessor, size_t index) {
	
	if (accessor == nullptr)
		return T();
	if (sizeof(T) != accessor->TotalComponentByteSize())
		throw std::runtime_error(
			std::string("Accessor total byte size(") + std::to_string(accessor->TotalComponentByteSize()) + ") different from size of " + typeid(T).name() + "(" + std::to_string(sizeof(T)) + ")");
	if (index >= accessor->Count())
		throw std::runtime_error(std::string("Index(") + std::to_string(index) + ") greater or equal to accessor Count(" + std::to_string(accessor->Count()) + ")");
	auto bufferView(accessor->GetBufferView());
	if (bufferView == nullptr)
		return T();
	auto stride(bufferView->ByteStride() ? bufferView->ByteStride() : accessor->TotalComponentByteSize());
	auto bufferIndex(accessor->ByteOffset() + bufferView->ByteOffset() + index * stride);
	return BufferHelper::Get<T>(bufferView->GetBuffer(), bufferIndex);
}

template <typename T>
inline void BufferHelper::Set(std::shared_ptr<BufferAccessor> accessor, size_t index, T value) {
	if (accessor == nullptr)
		return;
	auto bufferView(accessor->GetBufferView());
	if (bufferView == nullptr)
		return;
	auto buffer(bufferView->GetBuffer());
	if (buffer == nullptr)
		return;
	auto bufferIndex(accessor->ByteOffset() + bufferView->ByteOffset() + index * accessor->TotalComponentByteSize() * bufferView->ByteStride());
	auto pointer(buffer->RawData().data() + bufferIndex);
	memcpy(pointer, &value, std::min(sizeof(T), accessor->TotalComponentByteSize()));
}