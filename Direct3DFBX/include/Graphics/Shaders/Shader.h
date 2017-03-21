#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <vector>
#include <tuple>

#include <RenderTarget.h>
#include <DepthRenderTarget.h>
#include <MultiRenderTarget.h>
#include <HDRTexture.h>
#include <UAVTexture.h>

enum ShaderType
{
	VertexShader = 0,
	PixelShader,
	GeometryShader,
	ComputeShader,

	TotalShaderType
};

class Shader
{
public:
	// Constructor
	Shader();

	// Destructor
	~Shader();

	// Initialise vertex and pixel shaders
	bool Initialize(
		const std::string& vs_filename,
		const std::string& ps_filename,
		const std::string& vs_main_function_name,
		const std::string& ps_main_function_name,
		const D3D_SHADER_MACRO * vs_macros = nullptr,
		const D3D_SHADER_MACRO * ps_macros = nullptr,
		ID3DInclude * vs_includes = nullptr,
		ID3DInclude * ps_includes = nullptr);

	// Initialise compute shader
	bool Initialize(const std::string& cs_filename,
		const std::string& cs_main_function_name,
		const D3D_SHADER_MACRO * cs_macross = nullptr,
		ID3DInclude * cs_includess = nullptr);

	// Shutdown
	void Shutdown();

public:
	/*
	Create Buffer (Please note that buffer is aligned by 4 byte)
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb509632%28v=vs.85%29.aspx
	*/
	bool CreaterBuffer(ShaderType shaderType, UINT bufferNum, UINT size);

	// Set buffer for vertex shader
	template< typename... Args >
	bool SetShaderParameters(ShaderType shaderType,
		UINT bufferNum,				//!< Position in shader
		const Args&... args);

	template< typename... Args >
	bool SetShaderParameter(ShaderType shaderType,
		UINT bufferNum,
		const Args&... args); //DEOKADD

	template< typename T, UINT SIZE >
	bool SetShaderArrayParameters(ShaderType shaderType,
		UINT bufferNum,			//!< Position in shader
		const T(&a)[SIZE]);

	// Create Sampler (For texture)
	bool CreateSampler(ShaderType shaderType, UINT samplerNum);

	//// Set texture
	//bool SetTexture(ShaderType shaderType, UINT texNum, const Texture * texture);

	// Set texture
	bool SetTexture(ShaderType shaderType, UINT texNum, const HDRTexture * texture);

	// Set texture
	bool SetTexture(ShaderType shaderType, UINT texNum, const RenderTarget * texture);

	// Set texture
	bool SetTexture(ShaderType shaderType, UINT texNum, const DepthRenderTarget * texture);

	//// Set texture
	//bool SetTexture(ShaderType shaderType, UINT texNum, const GPUOBuffer * buffer);

	// Set texture
	bool SetTexture(ShaderType shaderType, UINT texNum, const MultiRenderTarget * mrt, UINT rtIndex);

	//// Set unordered buffer
	//bool SetUnorderedBuffer(UINT num, const GPUOBuffer * buffer);

	// Set unordered Texture
	bool BindUnorderedTexture(UINT num, const UAVTexture * texture);

	// UnBind data from GPU
	void UnBindUnorderedTexture(UINT num);

	// Set texture
	bool BindTexture(ShaderType shaderType, UINT texNum, const UAVTexture * texture);

	// Unbind texture
	void UnBindTexture(ShaderType shaderType, UINT texNum);

	// Render Vertex, (Geometry), Pixel Shader
	void Render(UINT index_count);

	// Dispatch Compute Shader
	void Dispatch(UINT x, UINT y, UINT z);

private:
	// Compile Shader
	ID3DBlob * CompileShader(const std::string& filename,
		const std::string& main_function_name,
		const std::string& profile,						//!< vs_5_0, ps_5_0 (Used to determine which type of shader)
		const D3D_SHADER_MACRO * macros = nullptr,
		ID3DInclude * includes = nullptr);

	// Create Vertex layout using reflection.
	bool CreateVertexLayout(ID3DBlob * vertexShaderBuffer);

	// Initialise Vertex and Pixel Shaders
	bool InitializeShaders(const std::string& vs_filename,
		const std::string& ps_filename,
		const std::string& vs_main_function_name,
		const std::string& ps_main_function_name,
		const D3D_SHADER_MACRO * vs_macros = nullptr,
		const D3D_SHADER_MACRO * ps_macros = nullptr,
		ID3DInclude * vs_includes = nullptr,
		ID3DInclude * ps_includes = nullptr);;

	// Initialize Compute Shader
	bool InitializeShader(const std::string& cs_filename,
		const std::string& cs_main_function_name,
		const D3D_SHADER_MACRO * cs_macros = nullptr,
		ID3DInclude * cs_includes = nullptr);

	// Shutdown Shader
	void ShutdownShader();

	// Get Device Context
	ID3D11DeviceContext * GetContext();

private:
	// Helper functor to set data of any parameters
	template< typename First, typename... Rest >
	class SetData
	{
	public:
		void operator()(void * dataPtr, const First & f, const Rest&... r)
		{
			*reinterpret_cast<First *>(dataPtr) = f;
			SetData<Rest...>()(reinterpret_cast<char *>(dataPtr) + sizeof(First), r...);
		}
	};

	template< typename First >
	class SetData<First>
	{
	public:
		void operator()(void * dataPtr, const First & f)
		{
			*reinterpret_cast<First *>(dataPtr) = f;
		}
	};

private:
	ID3DBlob * mVSBlob;
	ID3DBlob * mPSBlob;
	ID3D11VertexShader * mVertexShader;
	ID3D11PixelShader * mPixelShader;
	ID3D11ComputeShader * mComputeShader;
	ID3D11InputLayout * mLayout;
	std::vector<ID3D11Buffer *> mBuffers[TotalShaderType];
	std::vector<ID3D11SamplerState *> mSamplerStates[TotalShaderType];
};

#include "Shader.inl"
