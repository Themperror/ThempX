#include "../Headers/Particle2D.h"

Particle2D::Particle2D(LPDIRECT3DDEVICE9 device, D3DXMATRIX* camView, LPDIRECT3DVERTEXBUFFER9 vertexBuffer,LPDIRECT3DINDEXBUFFER9 indexBuffer)
{
	position = D3DXVECTOR3(0,0,0);
	scaling = D3DXVECTOR3(1,1,1);
	currentLife = 0;
	p_Device = device;
	cameraView = camView;
	vBuffer = vertexBuffer;
	iBuffer = indexBuffer;
}
void Particle2D::Draw(D3DXMATRIX m_ViewScale,D3DXMATRIX m_ViewWorld, D3DXMATRIX worldMatrix,D3DXMATRIX m_Scale,D3DXMATRIX m_Translation, D3DXMATRIX* camView)
{
	D3DXMatrixInverse(&m_ViewWorld, 0, camView); //Turn towards camera
	D3DXMatrixScaling(&m_Scale,scaling.x,scaling.y,scaling.z); //scaling
	D3DXMatrixTranslation(&m_Translation,position.x,position.y,position.z); //positioning


	D3DXMatrixMultiply(&m_ViewScale, &m_ViewWorld, &m_Scale);

	D3DXMatrixMultiply(&worldMatrix, &m_Translation, &m_ViewScale);

	worldMatrix._41 = position.x;
	worldMatrix._42 = position.y;
	worldMatrix._43 = position.z;
	HRESULT result;

	result = p_Device->SetFVF( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
	result = p_Device->SetStreamSource(0, vBuffer, 0, sizeof(PosNorTex));
	result = p_Device->SetIndices(iBuffer);
	result = p_Device->SetTransform(D3DTS_WORLD, &worldMatrix);
	result = p_Device->SetMaterial(mat);
	result = p_Device->SetTexture(0,texture);

	result = p_Device->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,0,0,4,0,2);
	switch(result)
	{
	case D3DERR_INVALIDCALL: std::cout << "Invalid Call" << std::endl;
		break;
	case D3DERR_CONFLICTINGRENDERSTATE: std::cout << "Conflicting Renderstate" << std::endl;
		break ;
	case D3DERR_DRIVERINVALIDCALL: std::cout <<"Driver invalid call"<< std::endl;
		break;
	case D3DERR_TOOMANYOPERATIONS : std::cout << "too many operations" << std::endl; 
		break;
	}
}