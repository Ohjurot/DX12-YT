#include "DefHeader.h"
#include "XDescHeap.h"

DX::XDescHeap::XDescHeap(ID3D12Device* ptrDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT size, bool shaderVisble) {
	EXPP_ASSERT_DBG(ptrDevice, "Invalid device pointer");
	EXPP_ASSERT(size, "Descritor Heap size cannot be 0");

	// Get handle increment value
	m_increment = ptrDevice->GetDescriptorHandleIncrementSize(type);
	EXPP_ASSERT(m_increment, "Invalid handle increment value recived");

	// Create DescHeap descriptor
	D3D12_DESCRIPTOR_HEAP_DESC desc;
	ZeroMemory(&desc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));

	// Describe heap
	desc.Type = type;
	desc.NumDescriptors = size;
	desc.Flags = (shaderVisble ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	desc.NodeMask = NULL;

	// Create descritor heap
	EVALUATE_HRESULT(ptrDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_comPointer)), "ID3D12Device->CreateDescriptorHeap(...)");
	m_size = size;
}

DX::XDescHeap::XDescHeap_Itterator DX::XDescHeap::begin() {
	return Itterator(*this, 0);
}

DX::XDescHeap::XDescHeap_Itterator DX::XDescHeap::end() {
	return Itterator(*this, m_size);
}

DX::DescHeapHandles DX::XDescHeap::get(unsigned int index) {
	EXPP_ASSERT(index < m_size, "Descritor heap index out of range");

	// Build start handles
	DX::DescHeapHandles handles;
	handles.cpuHandle = m_comPointer->GetCPUDescriptorHandleForHeapStart();
	handles.gpuHandle = m_comPointer->GetGPUDescriptorHandleForHeapStart();

	// Increment handles
	handles.cpuHandle.ptr += index * m_increment;
	handles.gpuHandle.ptr += index * m_increment;

	// Return 
	return handles;
}

DX::DescHeapHandles DX::XDescHeap::operator[](unsigned int index) {
	return get(index);
}

UINT DX::XDescHeap::size() noexcept {
	return m_size;
}

DX::XDescHeap::XDescHeap_Itterator::XDescHeap_Itterator(XDescHeap& refHeap, UINT index) :
	m_refHeap(refHeap),
	m_itIndex(index)
{
	// Assert valid heap
	EXPP_ASSERT(refHeap, "Itterator cannot be constructed from invalid heap");
}

DX::XDescHeap::XDescHeap_Itterator& DX::XDescHeap::XDescHeap_Itterator::operator=(const XDescHeap_Itterator& other) {
	EXPP_ASSERT(&m_refHeap == &other.m_refHeap, "Assigning itterators of diffrent heaps in not allowed!");

	m_itIndex = other.m_itIndex;
	return *this;
}

DX::XDescHeap::XDescHeap_Itterator DX::XDescHeap::XDescHeap_Itterator::operator++(int) {
	EXPP_ASSERT(m_itIndex < m_refHeap.m_size, "Incrementing itterator past end is invalid");
	m_itIndex++;

	return *this;
}

DX::XDescHeap::XDescHeap_Itterator DX::XDescHeap::XDescHeap_Itterator::operator--(int) {
	EXPP_ASSERT(m_itIndex > 0, "Decrementing itterator before begin is invalid");
	m_itIndex--;

	return *this;
}

DX::XDescHeap::XDescHeap_Itterator& DX::XDescHeap::XDescHeap_Itterator::operator+(unsigned int offset) {
	EXPP_ASSERT((m_itIndex + offset) <= m_refHeap.m_size, "Incrementing itterator past end is invalid");
	m_itIndex += offset;

	return *this;
}

DX::XDescHeap::XDescHeap_Itterator& DX::XDescHeap::XDescHeap_Itterator::operator+(const DX::XDescHeap::XDescHeap_Itterator& other) {
	EXPP_ASSERT((m_itIndex + other.m_itIndex) <= m_refHeap.m_size, "Incrementing itterator past end is invalid");
	m_itIndex += other.m_itIndex;

	return *this;
}

DX::XDescHeap::XDescHeap_Itterator& DX::XDescHeap::XDescHeap_Itterator::operator+=(unsigned int offset) {
	EXPP_ASSERT((m_itIndex + offset) <= m_refHeap.m_size, "Incrementing itterator past end is invalid");
	m_itIndex += offset;

	return *this;
}

DX::XDescHeap::XDescHeap_Itterator& DX::XDescHeap::XDescHeap_Itterator::operator-(unsigned int offset) {
	EXPP_ASSERT(offset <= m_itIndex, "Decrementing itterator before begin is invalid");
	m_itIndex -= offset;

	return *this;
}

DX::XDescHeap::XDescHeap_Itterator& DX::XDescHeap::XDescHeap_Itterator::operator-(const DX::XDescHeap::XDescHeap_Itterator& other) {
	EXPP_ASSERT(other.m_itIndex <= m_itIndex, "Decrementing itterator before begin is invalid");
	m_itIndex -= other.m_itIndex;

	return *this;
}

DX::XDescHeap::XDescHeap_Itterator& DX::XDescHeap::XDescHeap_Itterator::operator-=(unsigned int offset) {
	EXPP_ASSERT(offset <= m_itIndex, "Decrementing itterator before begin is invalid");
	m_itIndex -= offset;

	return *this;
}

bool DX::XDescHeap::XDescHeap_Itterator::operator==(const DX::XDescHeap::XDescHeap_Itterator& other) const {
	EXPP_ASSERT(&m_refHeap == &other.m_refHeap, "Comparing itterators of diffrent heaps in not allowed!");

	// Compare indicies
	return m_itIndex == other.m_itIndex;
}

DX::XDescHeap::XDescHeap_Itterator::operator unsigned int() noexcept {
	return m_itIndex;
}

DX::DescHeapHandles DX::XDescHeap::XDescHeap_Itterator::operator*() {
	return m_refHeap.get(m_itIndex);
}

DX::DescHeapHandles DX::XDescHeap::XDescHeap_Itterator::second() {
	return m_refHeap.get(m_itIndex);
}

DX::XDescHeap& DX::XDescHeap::XDescHeap_Itterator::first() noexcept{
	return m_refHeap;
}
