#include "GIOutput.h"

DX::GIOutput::GIOutput() noexcept :
	ScopedComPointer<IDXGIOutput>()
{
	// -
}

DX::GIOutput::GIOutput(IDXGIOutput* ptrOutput) noexcept :
	ScopedComPointer<IDXGIOutput>(ptrOutput)
{
	// -
}

bool DX::GIOutput::getDescription(DXGI_OUTPUT_DESC* ptrDesc) noexcept {
	// Pointer check
	if (m_comPointer) {
		return false;
	}

	// Return descritpion query result
	return SUCCEEDED(m_comPointer->GetDesc(ptrDesc));
}

bool DX::GIOutput::getDescription(DXGI_OUTPUT_DESC1* ptrDesc1) noexcept {
	// Check for interface support
	ScopedComPointer<IDXGIOutput6> output6;
	if (!queryInterface(output6)) {
		return false;
	}

	// Return descritpion query result
	return SUCCEEDED(output6->GetDesc1(ptrDesc1));
}
