#include <AnimationDef.h>

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
				//SAFE_RELEASE(pSampler[i]);
				//SAFE_RELEASE(pSRV);
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

			SAFE_RELEASE(m_meshColorBuffer);
			SAFE_RELEASE(m_pInputLayout);
			SAFE_RELEASE(m_pIB);
			SAFE_RELEASE(m_pVB);
			SAFE_RELEASE(m_pVBDepth);
		}
	};
};