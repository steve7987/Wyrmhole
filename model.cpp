#include "model.h"


Model::Model(){
	m_texture = 0;
}

Model::~Model(){

}

	
bool Model::Initialize(ID3D11Device * device, char* modelFilename, WCHAR* textureFilename, bool hasAlpha){
	if (!g_modelManager->AddModel(device, modelFilename)){
		textDump("error loading model file");
		return false;
	}
	modelFile = modelFilename;
	this->hasAlpha = hasAlpha;
	//load the texture
	m_texture = g_textureManager->GetTexture(device, textureFilename);
	xpos = 0;
	ypos = 0;
	zpos = 0;
	rotation = Quaternion(0,0,0,1);
	xscale = 1;
	yscale = 1;
	zscale = 1;
	UpdateWorldMatrix();

	return true;
}

void Model::Shutdown(){

}

bool Model::Render(ID3D11DeviceContext * deviceContext, Vector camlook){
	g_modelManager->RenderBuffers(deviceContext, modelFile);
	return true;
}

int Model::GetIndexCount(){
	return g_modelManager->GetIndexCount(modelFile);
}

bool Model::HasAlpha(){
	return hasAlpha;
}
	
float Model::getDepthSq(Vector campos, Vector look){
	Vector loc = Vector(xpos, ypos, zpos);
	Vector d = campos - loc;
	return ((d*look)*(d*look) / (look*look));
}

void Model::GetBoundingSphere(float& radius, Vector& center){
	radius = xscale + yscale + zscale; //not quite...
	center.x = xpos;
	center.y = ypos;
	center.z = zpos;
}

ID3D11ShaderResourceView* Model::GetTexture(){
	return m_texture->GetTexture();
}

D3DXMATRIX Model::GetWorldMatrix(){
	return worldMatrix;
}

void Model::SetPosition(float x, float y, float z){
	xpos = x;
	ypos = y;
	zpos = z;
	UpdateWorldMatrix();
}
	
void Model::SetPosition(Vector v){
	SetPosition(v.x, v.y, v.z);
}

void Model::SetRotation(Quaternion q){
	rotation = q;
	UpdateWorldMatrix();
}

void Model::SetScale(float x, float y, float z){
	xscale = x;
	yscale = y;
	zscale = z;
	UpdateWorldMatrix();
}

void Model::SetScale(Vector s){
	SetScale(s.x, s.y, s.z);
}

void Model::SetDepthSqPos(Vector pos){
	xpos = pos.x;
	ypos = pos.y;
	zpos = pos.z;
}

void Model::UpdateWorldMatrix(){
	D3DXQUATERNION qat;
	qat.x = rotation.x;
	qat.y = rotation.y;
	qat.z = rotation.z;
	qat.w = rotation.w;
	D3DXMATRIX trans, scale, rot;
	D3DXMatrixScaling(&scale, xscale, yscale, zscale);
	D3DXMatrixTranslation(&trans, xpos, ypos, zpos);
	D3DXMatrixRotationQuaternion(&rot, &qat);
	worldMatrix = scale*rot*trans;
}

void Model::SetWorldMatrix(D3DXMATRIX world){
	worldMatrix = world;
}