
//DEOKADD
template< typename... Args >
bool Shader::SetShaderParameter(ShaderType shaderType, UINT bufferNum, const Args&... args) //DEOKADD
{
	// Lock the constant buffer so it can be written to.
	ID3D11DeviceContext * deviceContext = GetContext();
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = deviceContext->Map(mBuffers[shaderType][bufferNum], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//std::vector< Vertex2D > *pBuffer;
	if (FAILED(result))
		return false;

	SetData<Args...>()(mappedResource.pData, args...);

	//pBuffer = reinterpret_cast<std::vector< Vertex2D >*>(mappedResource.pData);

	// Unlock the constant buffer.
	deviceContext->Unmap(mBuffers[shaderType][bufferNum], 0);

	// Finally set the constant buffer in the vertex shader with the updated values.
	if (shaderType == VertexShader)
		deviceContext->VSSetConstantBuffers(bufferNum, 1, mBuffers[shaderType].data() + bufferNum);
	else if (shaderType == PixelShader)
		deviceContext->PSSetConstantBuffers(bufferNum, 1, mBuffers[shaderType].data() + bufferNum);
	else if (shaderType == GeometryShader)
		deviceContext->GSSetConstantBuffers(bufferNum, 1, mBuffers[shaderType].data() + bufferNum);
	else if (shaderType == ComputeShader)
		deviceContext->CSSetConstantBuffers(bufferNum, 1, mBuffers[shaderType].data() + bufferNum);

	return true;
}

template< typename... Args >
bool Shader::SetShaderParameters(ShaderType shaderType, UINT bufferNum, const Args&... args)
{
	// Lock the constant buffer so it can be written to.
	ID3D11DeviceContext * deviceContext = GetContext();
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = deviceContext->Map(mBuffers[shaderType][bufferNum], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(result))
		return false;

	SetData<Args...>()(mappedResource.pData, args...);

	// Unlock the constant buffer.
	deviceContext->Unmap(mBuffers[shaderType][bufferNum], 0);

	// Finally set the constant buffer in the vertex shader with the updated values.
	if (shaderType == VertexShader)
		deviceContext->VSSetConstantBuffers(bufferNum, 1, mBuffers[shaderType].data() + bufferNum);
	else if (shaderType == PixelShader)
		deviceContext->PSSetConstantBuffers(bufferNum, 1, mBuffers[shaderType].data() + bufferNum);
	else if (shaderType == GeometryShader)
		deviceContext->GSSetConstantBuffers(bufferNum, 1, mBuffers[shaderType].data() + bufferNum);
	else if (shaderType == ComputeShader)
		deviceContext->CSSetConstantBuffers(bufferNum, 1, mBuffers[shaderType].data() + bufferNum);

	return true;
}

template< typename T, UINT SIZE >
bool Shader::SetShaderArrayParameters(ShaderType shaderType, UINT bufferNum, const T(&a)[SIZE])
{
	// Lock the constant buffer so it can be written to.
	ID3D11DeviceContext * deviceContext = GetContext();
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = deviceContext->Map(mBuffers[shaderType][bufferNum], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(result))
		return false;

	std::memcpy(mappedResource.pData, a, sizeof(T) * SIZE);

	// Unlock the constant buffer.
	deviceContext->Unmap(mBuffers[shaderType][bufferNum], 0);

	// Finally set the constant buffer in the vertex shader with the updated values.
	if (shaderType == VertexShader)
		deviceContext->VSSetConstantBuffers(bufferNum, 1, mBuffers[shaderType].data() + bufferNum);
	else if (shaderType == PixelShader)
		deviceContext->PSSetConstantBuffers(bufferNum, 1, mBuffers[shaderType].data() + bufferNum);
	else if (shaderType == GeometryShader)
		deviceContext->GSSetConstantBuffers(bufferNum, 1, mBuffers[shaderType].data() + bufferNum);
	else if (shaderType == ComputeShader)
		deviceContext->CSSetConstantBuffers(bufferNum, 1, mBuffers[shaderType].data() + bufferNum);

	return true;
}