#include "DefHeader.h"
#include "Shader.h"

dx::Shader::Shader(const Shader& other) :
	m_size(other.m_size),
	m_type(other.m_type)
{
	// Allocate memory
	m_ptrBlob = malloc(other.m_size);
	EXPP_ASSERT(m_ptrBlob, "Memory allocation failed");

	// Copy from other
	memcpy(m_ptrBlob, other.m_ptrBlob, other.m_size);
}

dx::Shader::Shader(Shader&& other) noexcept :
	m_ptrBlob(std::move(other.m_ptrBlob)),
	m_size(std::move(other.m_size)),
	m_type(std::move(other.m_type))
{ }

dx::Shader::Shader(LPCWSTR name, SHADER_TYPE type) {
	// Create output directorys
	CreateDirectory(SHADER_TARGET_FOLDER, NULL);
	
	// Set members
	m_type = type;
	wcscpy_s<MAX_PATH>(m_name, name);
	
	// (Re)load shader
	reload();
}

dx::Shader::~Shader() {
	if (m_ptrBlob) {
		free(m_ptrBlob);
	}
}

dx::Shader& dx::Shader::operator=(const Shader& other) {
	// Free own if required
	if (m_ptrBlob) {
		free(m_ptrBlob);
		m_ptrBlob = nullptr;
	}

	// Copy other
	m_size = other.m_size;
	m_type = other.m_type;
	
	// Allocate new memory
	if (other.m_ptrBlob) {
		m_ptrBlob = malloc(other.m_size);
		EXPP_ASSERT(m_ptrBlob, "Memory allocation failed");

		// Copy blob
		memcpy(m_ptrBlob, other.m_ptrBlob, other.m_size);
	}

	// Return self
	return *this;
}

dx::Shader::operator bool() noexcept {
	return m_ptrBlob != nullptr;
}

bool dx::Shader::setBuffer(void** ppVoid, UINT64* ptrSize) noexcept {
	// Check if blob is allocated
	if (m_ptrBlob) {
		// Set and return
		*ppVoid = m_ptrBlob;
		*ptrSize = m_size;
		return true;
	}
	
	// Fallback false
	return false;
}

void dx::Shader::reload() {
	// Free old data
	if (m_ptrBlob) {
		free(m_ptrBlob);
	}

	// Bild source filename
	std::wstringstream wss;
	wss << SHADER_SOURCE_FOLDER << m_name << L".hlsl";
	std::wstring sourceFileName = wss.str();

	// Build target filename (binary)
	wss.str(L"");
	wss << SHADER_TARGET_FOLDER << m_name << L".bin";
	std::wstring targetBinFileName = wss.str();

	// Build target filename (debug)
	wss.str(L"");
	wss << SHADER_TARGET_FOLDER << m_name << L".pdb";
	std::wstring targetDebugFileName = wss.str();

	// Build target filename (hash)
	wss.str(L"");
	wss << SHADER_TARGET_FOLDER << m_name << L".xh3";
	std::wstring targetHashFileName = wss.str();

	// Hash input file
	XXH128_hash_t sourceHash = { 0,0 };
	EXPP_ASSERT(common::File::Hashing::hash128bit(sourceFileName.c_str(), &sourceHash), "Failed to open source file");

	// Read current hash
	BOOL sourceRecompile = true;
	ScopedHandle hFileCurrentHash = CreateFile(targetHashFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	if (hFileCurrentHash && sourceHash.high64 != 0 && sourceHash.low64 != 0) {
		// Read current hash
		XXH128_hash_t currentHash = { 0,0 };
		EXPP_ASSERT(GetFileSize(hFileCurrentHash, NULL) == sizeof(XXH128_hash_t), "Invalid target hash");
		SetFilePointer(hFileCurrentHash, 0, NULL, FILE_BEGIN);
		EXPP_ASSERT(ReadFile(hFileCurrentHash, &currentHash, sizeof(XXH128_hash_t), NULL, NULL), "Failed to read target hash");

		// Check for equality
		if (sourceHash.low64 == currentHash.low64 && sourceHash.high64 == currentHash.high64) {
			sourceRecompile = false;
		}
	}

	// Check if recompile is required
	if (sourceRecompile) {
		// Compile shader
		dx::ShaderCompiler::compileShader(sourceFileName.c_str(), targetBinFileName.c_str(), targetDebugFileName.c_str(), m_type);

		// Close current hash file
		if (hFileCurrentHash) {
			hFileCurrentHash.close();
		}

		// Reopen for write
		hFileCurrentHash = CreateFile(targetHashFileName.c_str(), GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, NULL, NULL);

		// Write new hash
		SetFilePointer(hFileCurrentHash, 0, NULL, FILE_BEGIN);
		EXPP_ASSERT(WriteFile(hFileCurrentHash, &sourceHash, sizeof(XXH128_hash_t), NULL, NULL), "Failed to write hash");
	}

	// Load shader
	ScopedHandle hFileShaderBinary = CreateFile(targetBinFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	EXPP_ASSERT(hFileShaderBinary, "Shader binary not found!");

	// Allocate shader memory
	m_size = GetFileSize(hFileShaderBinary, NULL);
	m_ptrBlob = malloc(m_size);
	EXPP_ASSERT(m_ptrBlob, "Memory allocation failed");

	// Read shader file content
	SetFilePointer(hFileShaderBinary, 0, NULL, FILE_BEGIN);
	EXPP_ASSERT(ReadFile(hFileShaderBinary, m_ptrBlob, m_size, NULL, NULL), "Failed to read shader binary");
}

bool dx::Shader::isUptodata() {
	// Build file paths
	std::wstringstream wss;
	wss << SHADER_TARGET_FOLDER << m_name << L".xh3";
	std::wstring hashFileName = wss.str();

	wss.str(L"");
	wss << SHADER_SOURCE_FOLDER << m_name << L".hlsl";
	std::wstring sourceFileName = wss.str();
	
	// Retrive lates hash
	XXH128_hash_t sourceHash = { 0, 0 };
	if (!common::File::Hashing::hash128bit(sourceFileName.c_str(), &sourceHash)) {
		return false;
	}

	// Open shader hash file
	ScopedHandle hFileCurrentHash = CreateFile(hashFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	if (!hFileCurrentHash) {
		return false;
	}

	// Read current hash
	XXH128_hash_t currentHash = { 0,0 };
	EXPP_ASSERT(GetFileSize(hFileCurrentHash, NULL) == sizeof(XXH128_hash_t), "Invalid target hash");
	SetFilePointer(hFileCurrentHash, 0, NULL, FILE_BEGIN);
	EXPP_ASSERT(ReadFile(hFileCurrentHash, &currentHash, sizeof(XXH128_hash_t), NULL, NULL), "Failed to read target hash");

	// Compare hash return
	return (
		(currentHash.high64 == sourceHash.high64) &&
		(currentHash.low64 == sourceHash.low64)
	);
}
