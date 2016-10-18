#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <vector>
#include <tuple>

#include <RenderTarget.h>
#include <MultiRenderTarget.h>

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
	bool Initialize(std::string vs_filename,
		std::string ps_filename,
		std::string vs_main_function_name,
		std::string ps_main_function_name);

	// Initialise compute shader
	bool Initialize(std::string cs_filename,
		std::string cs_main_function_name);

	void SetDeviceContext(ID3D11DeviceContext* context) { mDeviceContext = context; }
	ID3D11DeviceContext* GetDeviceContext() const { return mDeviceContext; }

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

	// Set texture
	bool SetTexture(ShaderType shaderType, UINT texNum, const RenderTarget * texture);

	// Set texture
	bool SetTexture(ShaderType shaderType, UINT texNum, const MultiRenderTarget * mrt, UINT rtIndex);

	// Render Vertex, (Geometry), Pixel Shader
	void Render(UINT index_count);

	// Dispatch Compute Shader
	void Dispatch(UINT x, UINT y, UINT z);

private:
	// Compile Shader
	ID3DBlob * CompileShader(std::string filename,
		std::string main_function_name,
		std::string profile);

	// Create Vertex layout using reflection.
	bool CreateVertexLayout(ID3DBlob * vertexShaderBuffer);

	// Initialise Vertex and Pixel Shaders
	bool InitializeShader(std::string vs_filename,
		std::string ps_filename,
		std::string vs_main_function_name,
		std::string ps_main_function_name);

	// Initialise Compute Shader
	bool InitializeShader(std::string cs_filename,
		std::string cs_main_function_name);

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
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
};
