#include <Shader.h>
#include <SMat4.h>
#include <Graphics.h>

Shader::Shader()
	: mVertexShader(nullptr)
	, mPixelShader(nullptr)
	, mComputeShader(nullptr)
	, mLayout(nullptr)
{

}

Shader::~Shader()
{
}

bool Shader::Initialize(
	const std::string& vs_filename,
	const std::string& ps_filename,
	const std::string& vs_main_function_name,
	const std::string& ps_main_function_name,
	const D3D_SHADER_MACRO * vs_macros,
	const D3D_SHADER_MACRO * ps_macros,
	ID3DInclude * vs_includes,
	ID3DInclude * ps_includes)
{
	return InitializeShaders(vs_filename.c_str(),
		ps_filename.c_str(),
		vs_main_function_name,
		ps_main_function_name,
		vs_macros,
		ps_macros,
		vs_includes,
		ps_includes);
}

bool Shader::Initialize(const std::string& cs_filename,
	const std::string& cs_main_function_name,
	const D3D_SHADER_MACRO * cs_macros,
	ID3DInclude * cs_includes)
{
	return InitializeShader(cs_filename, cs_main_function_name, cs_macros, cs_includes);
}

void Shader::Shutdown()
{
	ShutdownShader();
}

bool Shader::CreaterBuffer(ShaderType shaderType, UINT bufferNum, UINT size)
{
	// Check vector size
	if (mBuffers[shaderType].size() <= bufferNum)
		mBuffers[shaderType].resize(bufferNum + 1, nullptr);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = size;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	ID3D11Device * device = GRAPHICS.GetPipeline()->GetDevice();
	HRESULT result = device->CreateBuffer(&bufferDesc, NULL, mBuffers[shaderType].data() + bufferNum);

	return !(FAILED(result));
}

// Create Sampler (For texture)
bool Shader::CreateSampler(ShaderType shaderType, UINT samplerNum)
{
	// Check vector size
	if (mSamplerStates[shaderType].size() <= samplerNum)
		mSamplerStates[shaderType].resize(samplerNum + 1, nullptr);

	D3D11_SAMPLER_DESC samplerDesc;

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0.f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	ID3D11Device * device = GRAPHICS.GetPipeline()->GetDevice();
	HRESULT result = device->CreateSamplerState(&samplerDesc, &mSamplerStates[shaderType][samplerNum]);

	return !FAILED(result);
}

//// Set texture
//bool Shader::SetTexture(ShaderType shaderType, UINT texNum, const Texture * texture)
//{
//	if (!texture || !texture->mTextureView)
//	return false;
//	
//	if (shaderType == VertexShader)
//	mDeviceContext->VSSetShaderResources(texNum, 1, &texture->mTextureView);
//	else if (shaderType == PixelShader)
//	mDeviceContext->PSSetShaderResources(texNum, 1, &texture->mTextureView);
//	else if (shaderType == GeometryShader)
//	mDeviceContext->GSSetShaderResources(texNum, 1, &texture->mTextureView);
//	else if (shaderType == ComputeShader)
//	mDeviceContext->CSSetShaderResources(texNum, 1, &texture->mTextureView);
//	
//	return true;
//}

// Set texture
bool Shader::SetTexture(ShaderType shaderType, UINT texNum, const HDRTexture * texture)
{
	if (!texture || !texture->mTextureView)
		return false;

	ID3D11DeviceContext * deviceContext = GRAPHICS.GetPipeline()->GetContext();
	if (shaderType == VertexShader)
		deviceContext->VSSetShaderResources(texNum, 1, &texture->mTextureView);
	else if (shaderType == PixelShader)
		deviceContext->PSSetShaderResources(texNum, 1, &texture->mTextureView);
	else if (shaderType == GeometryShader)
		deviceContext->GSSetShaderResources(texNum, 1, &texture->mTextureView);
	else if (shaderType == ComputeShader)
		deviceContext->CSSetShaderResources(texNum, 1, &texture->mTextureView);

	return true;
}

// Set texture
bool Shader::SetTexture(ShaderType shaderType, UINT texNum, const RenderTarget * texture)
{
	if (!texture || !texture->m_shaderResourceView)
		return false;

	ID3D11DeviceContext * deviceContext = GRAPHICS.GetPipeline()->GetContext();

	if (shaderType == VertexShader)
		deviceContext->VSSetShaderResources(texNum, 1, &texture->m_shaderResourceView);
	else if (shaderType == PixelShader)
		deviceContext->PSSetShaderResources(texNum, 1, &texture->m_shaderResourceView);
	else if (shaderType == GeometryShader)
		deviceContext->GSSetShaderResources(texNum, 1, &texture->m_shaderResourceView);
	else if (shaderType == ComputeShader)
		deviceContext->CSSetShaderResources(texNum, 1, &texture->m_shaderResourceView);

	return true;
}

// Set texture
bool Shader::SetTexture(ShaderType shaderType, UINT texNum, const DepthRenderTarget * texture)
{
	if (!texture || !texture->m_shaderResourceView)
		return false;

	ID3D11DeviceContext * deviceContext = GRAPHICS.GetPipeline()->GetContext();

	if (shaderType == VertexShader)
		deviceContext->VSSetShaderResources(texNum, 1, &texture->m_shaderResourceView);
	else if (shaderType == PixelShader)
		deviceContext->PSSetShaderResources(texNum, 1, &texture->m_shaderResourceView);
	else if (shaderType == GeometryShader)
		deviceContext->GSSetShaderResources(texNum, 1, &texture->m_shaderResourceView);
	else if (shaderType == ComputeShader)
		deviceContext->CSSetShaderResources(texNum, 1, &texture->m_shaderResourceView);

	return true;
}

//// Set texture
//bool Shader::SetTexture(ShaderType shaderType, UINT texNum, const GPUOBuffer * buffer)
//{
//	if (!buffer || !buffer->mShaderResourceView)
//	return false;
//	
//	if (shaderType == VertexShader)
//	mDeviceContext->VSSetShaderResources(texNum, 1, &buffer->mShaderResourceView);
//	else if (shaderType == PixelShader)
//	mDeviceContext->PSSetShaderResources(texNum, 1, &buffer->mShaderResourceView);
//	else if (shaderType == GeometryShader)
//	mDeviceContext->GSSetShaderResources(texNum, 1, &buffer->mShaderResourceView);
//	else if (shaderType == ComputeShader)
//	mDeviceContext->CSSetShaderResources(texNum, 1, &buffer->mShaderResourceView);
//}

// Set texture
bool Shader::SetTexture(ShaderType shaderType, UINT texNum, const MultiRenderTarget * mrt, UINT rtIndex)
{
	if (!mrt || mrt->GetRenderTargetsCount() <= rtIndex)
		return false;

	ID3D11DeviceContext * deviceContext = GRAPHICS.GetPipeline()->GetContext();

	if (shaderType == VertexShader)
		deviceContext->VSSetShaderResources(texNum, 1, &mrt->m_shaderResourceViews[rtIndex]);
	else if (shaderType == PixelShader)
		deviceContext->PSSetShaderResources(texNum, 1, &mrt->m_shaderResourceViews[rtIndex]);
	else if (shaderType == GeometryShader)
		deviceContext->GSSetShaderResources(texNum, 1, &mrt->m_shaderResourceViews[rtIndex]);
	else if (shaderType == ComputeShader)
		deviceContext->CSSetShaderResources(texNum, 1, &mrt->m_shaderResourceViews[rtIndex]);

	return true;
}

//// Set unordered buffer
//bool Shader::SetUnorderedBuffer(UINT num, const GPUOBuffer * buffer)
//{
//	if (!buffer || !buffer->mDataBufferView)
//	return false;
//	
//	mDeviceContext->CSSetUnorderedAccessViews(num, 1, &buffer->mDataBufferView, NULL);
//	
//	return true;
//}

// Set unordered Texture
bool Shader::BindUnorderedTexture(UINT num, const UAVTexture * texture)
{
	if (!texture || !texture->mUnorderedAccessView)
		return false;

	ID3D11DeviceContext * deviceContext = GRAPHICS.GetPipeline()->GetContext();
	deviceContext->CSSetUnorderedAccessViews(num, 1, &texture->mUnorderedAccessView, NULL);

	return true;
}

// UnBind data from GPU
void Shader::UnBindUnorderedTexture(UINT num)
{
	ID3D11Device * device = GRAPHICS.GetPipeline()->GetDevice();
	ID3D11DeviceContext * deviceContext = GRAPHICS.GetPipeline()->GetContext();
	static ID3D11UnorderedAccessView * UAV_NULL = nullptr;

	deviceContext->CSSetUnorderedAccessViews(num, 1, &UAV_NULL, NULL);
}

// Set texture
bool Shader::BindTexture(ShaderType shaderType, UINT texNum, const UAVTexture * texture)
{
	if (!texture || !texture->mShaderResourceView)
		return false;

	ID3D11DeviceContext * deviceContext = GRAPHICS.GetPipeline()->GetContext();

	if (shaderType == VertexShader)
		deviceContext->VSSetShaderResources(texNum, 1, &texture->mShaderResourceView);
	else if (shaderType == PixelShader)
		deviceContext->PSSetShaderResources(texNum, 1, &texture->mShaderResourceView);
	else if (shaderType == GeometryShader)
		deviceContext->GSSetShaderResources(texNum, 1, &texture->mShaderResourceView);
	else if (shaderType == ComputeShader)
		deviceContext->CSSetShaderResources(texNum, 1, &texture->mShaderResourceView);

	return true;
}

// Unbind texture
void Shader::UnBindTexture(ShaderType shaderType, UINT texNum)
{
	static ID3D11ShaderResourceView * SRV_NULL = nullptr;

	ID3D11DeviceContext * deviceContext = GRAPHICS.GetPipeline()->GetContext();

	if (shaderType == VertexShader)
		deviceContext->VSSetShaderResources(texNum, 1, &SRV_NULL);
	else if (shaderType == PixelShader)
		deviceContext->PSSetShaderResources(texNum, 1, &SRV_NULL);
	else if (shaderType == GeometryShader)
		deviceContext->GSSetShaderResources(texNum, 1, &SRV_NULL);
	else if (shaderType == ComputeShader)
		deviceContext->CSSetShaderResources(texNum, 1, &SRV_NULL);
}

// Render Vertex, (Geometry), Pixel Shader
void Shader::Render(UINT index_count)
{
	ID3D11DeviceContext * deviceContext = GRAPHICS.GetPipeline()->GetContext();

	// Set the vertex input layout.
	deviceContext->IASetInputLayout(mLayout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(mVertexShader, NULL, 0);
	deviceContext->PSSetShader(mPixelShader, NULL, 0);

	for (UINT i = 0; i < TotalShaderType; ++i)
	{
		UINT size = mSamplerStates[i].size();
		for (UINT j = 0; j < size; ++j)
		{
			if (i == VertexShader)
				deviceContext->VSSetSamplers(j, 1, &mSamplerStates[i][j]);
			else if (i == PixelShader)
				deviceContext->PSSetSamplers(j, 1, &mSamplerStates[i][j]);
			else if (i == GeometryShader)
				deviceContext->GSSetSamplers(j, 1, &mSamplerStates[i][j]);
			else if ( i == ComputeShader )
				deviceContext->CSSetSamplers(j, 1, &mSamplerStates[i][j] );
		}
	}

	// Render the triangle.
	deviceContext->DrawIndexed(index_count, 0, 0);
}

// Dispatch Compute Shader
void Shader::Dispatch(UINT x, UINT y, UINT z)
{
	ID3D11DeviceContext * deviceContext = GRAPHICS.GetPipeline()->GetContext();

	deviceContext->CSSetShader(mComputeShader, NULL, 0);

	UINT size = mSamplerStates[ComputeShader].size();
	for (UINT j = 0; j < size; ++j)
	{
		deviceContext->CSSetSamplers(j, 1, &mSamplerStates[ComputeShader][j]);
	}

	deviceContext->Dispatch(x, y, z);
}

// Compile Shader
ID3DBlob * Shader::CompileShader(
	const std::string& filename,
	const std::string& main_function_name,
	const std::string& profile,						//!< vs_5_0, ps_5_0 (Used to determine which type of shader)
	const D3D_SHADER_MACRO * macros,
	ID3DInclude * includes)
{
	HRESULT result = S_OK;
	ID3DBlob * shaderBuffer = nullptr;
	ID3DBlob * errorMessage = nullptr;
	DWORD compileOption = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	compileOption |= D3DCOMPILE_DEBUG;
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
#endif

	D3DX11CompileFromFile(
		filename.c_str(),				//!< Shader File Name
		macros,							//!< Define Macros to be used in shader
		includes,						//!< Handle #include within shader
		main_function_name.c_str(),		//!< Entry Function Name where shader calls to begin
		profile.c_str(),				//!< Shader Features
		compileOption,					//!< Compile Options 1
		0,								//!< Compile Options 2
		0,								//!< ThreadPump
		&shaderBuffer,					//!< Shader Code
		&errorMessage,					//!< Error messages
		&result);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputDebugStringA((char*)errorMessage->GetBufferPointer());
			errorMessage->Release();
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(nullptr, filename.c_str(), "Missing Shader File", MB_OK);
		}

		return nullptr;
	}

	return shaderBuffer;
}

// Create Vertex layout using reflection.
bool Shader::CreateVertexLayout(ID3DBlob * vertexShaderBuffer)
{
	// Create the vertex input layout description.
	// Reflect shader info
	ID3D11ShaderReflection * pVertexShaderReflection = nullptr;
	HRESULT result = D3DReflect(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(),
		IID_ID3D11ShaderReflection,
		reinterpret_cast<void**>(&pVertexShaderReflection));

	if (FAILED(result))
		return false;

	// Get shader info
	D3D11_SHADER_DESC shaderDesc;
	pVertexShaderReflection->GetDesc(&shaderDesc);

	// Read input layout description from shader info
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (UINT i = 0; i< shaderDesc.InputParameters; ++i)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		pVertexShaderReflection->GetInputParameterDesc(i, &paramDesc);

		// fill out input element desc
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		// determine DXGI format
		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		//save element desc
		inputLayoutDesc.push_back(elementDesc);
	}

	ID3D11Device * device = GRAPHICS.GetPipeline()->GetDevice();

	device->CreateInputLayout(&inputLayoutDesc[0], inputLayoutDesc.size(), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &mLayout);

	//Free allocation shader reflection memory
	pVertexShaderReflection->Release();

	return true;
}

// Initialise Vertex and Pixel Shaders
bool Shader::InitializeShaders(const std::string& vs_filename,
	const std::string& ps_filename,
	const std::string& vs_main_function_name,
	const std::string& ps_main_function_name,
	const D3D_SHADER_MACRO * vs_macros,
	const D3D_SHADER_MACRO * ps_macros,
	ID3DInclude * vs_includes,
	ID3DInclude * ps_includes)
{
	// Compile the vertex shader code.
	ID3DBlob * vertexShaderBuffer = CompileShader(vs_filename, vs_main_function_name, "vs_5_0", vs_macros, vs_includes);
	if (!vertexShaderBuffer)
		return false;

	// Compile the pixel shader code.
	ID3DBlob * pixelShaderBuffer = CompileShader(ps_filename, ps_main_function_name, "ps_5_0", ps_macros, ps_includes);
	if (!pixelShaderBuffer)
		return false;

	// Create the vertex shader from the buffer.
	ID3D11Device * device = GRAPHICS.GetPipeline()->GetDevice();
	HRESULT result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &mVertexShader);
	if (FAILED(result))
		return false;

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &mPixelShader);
	if (FAILED(result))
		return false;

	// Create the vertex input layout description.
	if (!CreateVertexLayout(vertexShaderBuffer))
		return false;

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	SAFE_RELEASE(vertexShaderBuffer);
	SAFE_RELEASE(pixelShaderBuffer);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	// world,view, proj, wvp
	return CreaterBuffer(VertexShader, 0, sizeof(ursine::SMat4) * 4);
}

// Initialize Compute Shader
bool Shader::InitializeShader(const std::string& cs_filename,
	const std::string& cs_main_function_name,
	const D3D_SHADER_MACRO * cs_macros,
	ID3DInclude * cs_includes)
{
	// Compile the vertex shader code.
	ID3DBlob * computeShaderBuffer = CompileShader(cs_filename, cs_main_function_name, "cs_5_0", cs_macros, cs_includes);
	if (!computeShaderBuffer)
	{
		return false;
	}

	// Create the compute shader from the buffer.
	ID3D11Device * device = GRAPHICS.GetPipeline()->GetDevice();
	HRESULT result = device->CreateComputeShader(computeShaderBuffer->GetBufferPointer(), computeShaderBuffer->GetBufferSize(), NULL, &mComputeShader);
	if (FAILED(result))
		return false;

	// Setup the description of the dynamic resolution constant buffer that is in the blur compute shader.
	// resolution width, height, blur kernal radius
	return CreaterBuffer(ComputeShader, 0, sizeof(UINT) * 4);
}

// Shutdown Shader
void Shader::ShutdownShader()
{
	// Release the constant buffers.
	for (UINT j = 0; j < TotalShaderType; ++j)
	{
		for (auto & i : mBuffers[j])
			SAFE_RELEASE(i)
			mBuffers[j].clear();
	}

	// Release the samplers.
	for (UINT j = 0; j < TotalShaderType; ++j)
	{
		for (auto & i : mSamplerStates[j])
			SAFE_RELEASE(i);
		mSamplerStates[j].clear();
	}

	// Release the layout.
	SAFE_RELEASE(mLayout);
	// Release the pixel shader.
	SAFE_RELEASE(mPixelShader);
	// Release the vertex shader.
	SAFE_RELEASE(mVertexShader);
	// Release the compute shader.
	SAFE_RELEASE(mComputeShader);
}

ID3D11DeviceContext * Shader::GetContext()
{
	return GRAPHICS.GetPipeline()->GetContext();
}