#include <AnimationDef.h>
#include <Graphics.h>

namespace ursine
{
	namespace FBX_DATA
	{
		void Material_Data::Release()
		{
			SAFE_RELEASE(pMaterialCb);
			for (UINT i = 0; i < 2; ++i)
			{
				SAFE_RELEASE(pSRV[i]);
			}
			SAFE_RELEASE(pSampler);
		}

		void FbxMaterial::Release()
		{
			ambient.Release();
			diffuse.Release();
			emissive.Release();
			specular.Release();
		}

		FbxMaterial& FbxMaterial::operator=(const FbxMaterial& rhs)
		{
			if (this == &rhs)
				return *this;

			name = rhs.name;
			type = rhs.type;
			ambient = rhs.ambient;
			diffuse = rhs.diffuse;
			emissive = rhs.emissive;
			specular = rhs.specular;
			shineness = rhs.shineness;
			TransparencyFactor = rhs.TransparencyFactor;
			return *this;
		}

		void MESH_NODE::Release()
		{
			for (UINT i = 0; i < fbxmtrlData.size(); ++i)
				fbxmtrlData[i].Release();
			fbxmtrlData.clear();

			SAFE_RELEASE(m_pInputLayout);
			SAFE_RELEASE(m_pIB);
			SAFE_RELEASE(m_pVB);
			SAFE_RELEASE(m_pVBC);
		}

		void MESH_NODE::Bind()
		{
			ID3D11DeviceContext* deviceContext = GRAPHICS.GetPipeline()->GetContext();
			// Set vertex buffer stride and offset.
			UINT stride = m_Stride;
			UINT offset = 0;

			// Set the vertex buffer to active in the input assembler so it can be rendered.
			deviceContext->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);

			// Set the index buffer to active in the input assembler so it can be rendered.
			deviceContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);

			// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
			deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}

		void MESH_NODE::BindCompatible()
		{
			ID3D11DeviceContext* deviceContext = GRAPHICS.GetPipeline()->GetContext();
			// Set vertex buffer stride and offset.
			UINT stride = m_StrideCompatible;
			UINT offset = 0;

			// Set the vertex buffer to active in the input assembler so it can be rendered.
			deviceContext->IASetVertexBuffers(0, 1, &m_pVBC, &stride, &offset);

			// Set the index buffer to active in the input assembler so it can be rendered.
			deviceContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);

			// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
			deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
	};
};