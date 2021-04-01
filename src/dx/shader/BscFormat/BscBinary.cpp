#include "DefHeader.h"
#include "BscBinary.h"

BSC::Binary::Binary(LONG64* ptrRef, void* ptrBlob, UINT64 shaderSize, UINT64 debugSize, UINT64 reflectionSize) noexcept :
	m_refCounter(ptrRef)
{
	// Incrment ref
	InterlockedIncrement64(m_refCounter);

	// Set shader
	m_ptrShader = ptrBlob;
	m_sizeShader = shaderSize;

	// Set debug
	m_ptrDebug = (void*)(((UINT64)ptrBlob) + shaderSize);
	m_sizeDebug = debugSize;

	// Set Reflection
	m_ptrReflection = (void*)(((UINT64)ptrBlob) + shaderSize + debugSize);
	m_sizeReflection = reflectionSize;
}

BSC::Binary::Binary(const Binary& other) noexcept :
	m_refCounter(other.m_refCounter),
	m_ptrShader(other.m_ptrShader),
	m_ptrDebug(other.m_ptrDebug),
	m_ptrReflection(other.m_ptrReflection),
	m_sizeShader(other.m_sizeShader),
	m_sizeDebug(other.m_sizeDebug),
	m_sizeReflection(other.m_sizeReflection)
{
	// Increment ref if required
	if (m_refCounter) {
		InterlockedIncrement64(m_refCounter);
	}
}

BSC::Binary::~Binary() noexcept {
	// Decrement ref in case object is valid
	if (m_refCounter) {
		InterlockedDecrement64(m_refCounter);
	}
}

BSC::Binary& BSC::Binary::operator=(const Binary& other) noexcept {
	// Decrement ref if required
	if (m_refCounter) {
		InterlockedDecrement64(m_refCounter);
	}

	// Copy from other
	m_refCounter = other.m_refCounter;
	m_ptrShader = other.m_ptrShader;
	m_ptrDebug = other.m_ptrDebug;
	m_ptrReflection = other.m_ptrReflection;
	m_sizeShader = other.m_sizeShader;
	m_sizeDebug = other.m_sizeDebug;
	m_sizeReflection = other.m_sizeReflection;

	// Increment if required
	if (m_refCounter) {
		InterlockedIncrement64(m_refCounter);
	}

	return *this;
}

BSC::Binary::operator bool() noexcept {
	return (m_refCounter != nullptr);
}

UINT64 BSC::Binary::getShader(void** ppData) noexcept {
	*ppData = m_ptrShader;
	return m_sizeShader;
}

UINT64 BSC::Binary::getDebug(void** ppData) noexcept {
	*ppData = m_ptrDebug;
	return m_sizeDebug;
}

UINT64 BSC::Binary::getReflection(void** ppData) noexcept {
	*ppData = m_ptrReflection;
	return m_sizeReflection;
}
