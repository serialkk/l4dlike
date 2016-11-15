#include "_stdafx.h"

GSeqSinglePlay * GSeqSinglePlay::pInstance_ = 0;


bool GSeqSinglePlay::UpdateGunPosition() {

	D3DXMATRIX matWorld, matScl, matRot;

	D3DXVECTOR3 vScl, vTrans;
	D3DXQUATERNION qRot;

	D3DXQUATERNION  qRotation; //���ʹϿ� ����
	D3DXQuaternionRotationYawPitchRoll(&qRotation,  // �̰������� X��� Y���� ȸ���� ���
		(float)D3DXToRadian(180.0f),    // �Ͽ� ������� ����
		0.0f,
		0.0f);

	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixIdentity(&matScl);
	D3DXMatrixIdentity(&matRot);

	if (!m_pCamera)
		return false;

	D3DXMatrixInverse(&matWorld, NULL, m_pCamera->GetViewMatrix());
	D3DXMatrixDecompose(&vScl, &qRot, &vTrans, &matWorld);

	D3DXMatrixScaling(&matScl, vScl.x, vScl.y, vScl.z);

	//vTrans.x += 10.0f;
	vTrans.y -= 5.0f;
	vTrans.z += 10.0f;
	qRot = qRot * qRotation;

	D3DXMatrixAffineTransformation(&matRot, 1.0f, NULL, &qRot, &vTrans);

	m_ObjGun.m_matWorld = matScl * matRot;// *matTrans;

	return true;
}

#ifdef G_MACRO_CHAR_ADD
bool GSeqSinglePlay::LoadFileDlg(TCHAR* szExt, TCHAR* szTitle)
{
	OPENFILENAME    ofn;
	TCHAR           szFile[MAX_PATH] = { 0, };
	TCHAR			szFileTitle[MAX_PATH] = { 0, };
	static TCHAR    *szFilter;

	TCHAR lpCurBuffer[256] = { 0, };
	GetCurrentDirectory(256, lpCurBuffer);

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	_tcscpy_s(szFile, _T("*."));
	_tcscat_s(szFile, szExt);
	_tcscat_s(szFile, _T("\0"));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = GetActiveWindow();
	ofn.lpstrFilter = szFilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0L;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = szFileTitle;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrInitialDir = _T("../../data/Character/");
	ofn.lpstrTitle = szTitle;
	ofn.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = szExt;

	if (!GetOpenFileName(&ofn))
	{
		return false;
	}
	TCHAR* load = _tcstok(szFile, _T("\n"));
	T_STR dir = szFile;
	load = &load[_tcslen(load) + 1];
	if (*load == 0)
	{
		m_LoadFiles.push_back(dir);
	}

	while (*load != 0)
	{
		T_STR dir = szFile;
		load = _tcstok(load, _T("\n"));
		dir += _T("\\");
		dir += load;
		m_LoadFiles.push_back(dir);
		load = &load[_tcslen(load) + 1];
	}
	SetCurrentDirectory(lpCurBuffer);
	return true;
}
#endif

#ifdef G_MACRO_CHAR_ADD

bool GSeqSinglePlay::Load()
{

	if (!I_CharMgr.Load(g_pd3dDevice, g_pImmediateContext, _T("CharTable3.gci") /*_T("CharTable.gci")*/))
	{
		return false;
	}

	GCharacter* pChar0 = I_CharMgr.GetPtr(L"TESTCHAR6");


	shared_ptr<GHeroObj> pObjA = make_shared<GHeroObj>();
	pObjA->Set(pChar0,
		pChar0->m_pBoneObject,
		pChar0->m_pBoneObject->m_Scene.iFirstFrame,
		pChar0->m_pBoneObject->m_Scene.iLastFrame);
	m_HeroObj.push_back(pObjA);
	return true;
}
#endif 
bool GSeqSinglePlay::Init()
{


	m_ObjGun.Init();

	if (!m_ObjGun.Load(g_pd3dDevice, _T("data/object/fps_shotgun/shotgun3.GBS"), L"data/shader/box.hlsl"))
	{
		return false;
	}
#ifdef G_MACRO_CHAR_ADD
	I_CharMgr.Init();

	Load();

#endif

#ifdef G_MACRO_EFFECT_ADD
	m_pPS.Attach(DX::LoadPixelShaderFile(g_pd3dDevice, 
		L"data/shader/Blend.hlsl", "PS_MATERIAL"));
	//--------------------------------------------------------------------------------------
	// ��� �κ�
	//--------------------------------------------------------------------------------------
	m_pScreen = make_shared<GPlaneShape>();
	if (m_pScreen->Create(g_pd3dDevice, G_SHA_PLANE) == false)
	{
		MessageBox(0, _T("m_pPlane ����"), _T("Fatal error"), MB_OK);
		return 0;
	}
	//SAFE_NEW(m_pSprite, GSprite);
	m_pSprite = make_shared<GSprite>();
	//--------------------------------------------------------------------------------------
	// ����  ���
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matRotation, matScale;
	D3DXMatrixScaling(&matScale, 5.0f, 5.0f, 5.0f);
	D3DXMatrixRotationX(&matRotation, D3DX_PI*0.5f);
	D3DXMatrixMultiply(&m_mPlanWorld, &matScale, &matRotation);
	D3DXMatrixIdentity(&m_matPlaneWorld);


	//play ��ư�� init() �κ�
	m_pSprite.get()->Create(g_pd3dDevice, G_SHA_PLANE, L"data/effect/ds1.dds");
	// �ִϸ��̼� ����, ����4x4
	m_pSprite.get()->SetRectAnimation(1.0f, 4, 128, 4, 128);
#endif

#ifdef G_MACRO_MAP_ADD
	
	for (int i = 0; i < G_OBJ_CNT; i++) {
		D3DXMatrixIdentity(&m_matObjWorld[i]);
		D3DXMatrixIdentity(&matObjScale[i]);
		D3DXMatrixIdentity(&matObjRotation[i]);
		D3DXMatrixIdentity(&matObjTrans[i]);
	}

	m_iDrawDepth = 0;
	m_bDebugRender = false;

	//--------------------------------------------------------------------------------------
	// ����� ���� ����
	//--------------------------------------------------------------------------------------
	if (FAILED(m_DrawLine.Create(g_pd3dDevice, G_SHA_LINE)))
	{
		MessageBox(0, _T("m_DrawLine ����"), _T("Fatal error"), MB_OK);
		return 0;
	}
#endif

	//--------------------------------------------------------------------------------------
	// ī�޶�  ��� 
	//--------------------------------------------------------------------------------------	
	m_pDebugCamera = make_shared<GCamera>();
	m_pDebugCamera->SetViewMatrix(D3DXVECTOR3(0.0f, 2500.0f, -2500.0f), D3DXVECTOR3(0.0f, 10.0f, 0.0f));

	float fAspectRatio = g_pMain->m_iWindowWidth / (FLOAT)g_pMain->m_iWindowHeight;
	m_pDebugCamera->SetProjMatrix(D3DX_PI / 4, fAspectRatio, 0.1f, 10000.0f);
	m_pDebugCamera->SetWindow(g_pMain->m_iWindowWidth, g_pMain->m_iWindowHeight);


	m_pFPSCamera = make_shared<GFPSCamera>();
	m_pFPSCamera->SetViewMatrix(D3DXVECTOR3(10.0f, 2500.0f, -2500.0f), D3DXVECTOR3(-10.0f, 10.0f, 50.0f));

	fAspectRatio = g_pMain->m_iWindowWidth / (FLOAT)g_pMain->m_iWindowHeight;
	m_pFPSCamera->SetProjMatrix(D3DX_PI / 4, fAspectRatio, 0.1f, 10000.0f);
	m_pFPSCamera->SetWindow(g_pMain->m_iWindowWidth, g_pMain->m_iWindowHeight);



	m_pCamera = m_pDebugCamera.get();
#ifdef G_MACRO_MAP_ADD
	//--------------------------------------------------------------------------------------
	// ��ī�� �ڽ�
	//--------------------------------------------------------------------------------------
	m_pSkyBoxObj = make_shared<GN2Skybox>();
	if (m_pSkyBoxObj->Create(g_pd3dDevice, L"data/shader/SkyBox.hlsl") == false)
	{
		MessageBox(0, _T("m_pDirectionLIne ����"), _T("Fatal error"), MB_OK);
		return 0;
	}
	m_pSkyBoxObj->CreateTextureArray(g_pd3dDevice, g_pImmediateContext);

	//--------------------------------------------------------------------------------------
	// ī�޶� ���ν��� �������� �ڽ� ������Ʈ ����
	//--------------------------------------------------------------------------------------
	m_pCamera->CreateRenderBox(g_pd3dDevice, g_pImmediateContext);
	m_pPixelShader.Attach(DX::LoadPixelShaderFile(g_pd3dDevice, G_SHA_BOX, "PS_Color"));

	//--------------------------------------------------------------------------------------
	// Ŀ���Ҹ� ����
	//--------------------------------------------------------------------------------------
	TMapDesc MapDesc = { 50, 50, 50.0f, 0.0f,L"data/sand.jpg", L"data/shader/CustomizeMap.hlsl" };
	m_CustomMap.Init(g_pd3dDevice, g_pImmediateContext);
	if (FAILED(m_CustomMap.Load(MapDesc)))
	{
		return false;
	}
	//--------------------------------------------------------------------------------------
	// ������Ʈ ����
	//--------------------------------------------------------------------------------------
	for (int i = 0; i < G_OBJ_CNT; i++) {
		m_Obj[i].Init();
	}

	m_Obj[G_OBJ_LAB].Load(g_pd3dDevice,G_OBJ_LOC_LAB, G_SHA_BOX);
	D3DXMatrixScaling(&m_matObjWorld[G_OBJ_LAB], 2, 2, 2);
	m_matObjWorld[G_OBJ_LAB]._41 = 1000.0f;
	m_matObjWorld[G_OBJ_LAB]._42 = 0.0f;
	m_matObjWorld[G_OBJ_LAB]._43 = 1000.0f;

	m_Obj[G_OBJ_DROPSHIP].Load(g_pd3dDevice, G_OBJ_LOC_DROPSHIP_LAND, G_SHA_BOX);
	D3DXMatrixScaling(&matObjScale[G_OBJ_DROPSHIP], 2.f, 2.f, 2.f);
	D3DXMatrixRotationY(&matObjRotation[G_OBJ_DROPSHIP], D3DXToRadian(45.0f+180.0f));
	m_matObjWorld[G_OBJ_DROPSHIP] = matObjScale[G_OBJ_DROPSHIP] * matObjRotation[G_OBJ_DROPSHIP];
	m_matObjWorld[G_OBJ_DROPSHIP]._41 = -1000.0f;
	m_matObjWorld[G_OBJ_DROPSHIP]._42 = 0.0f;
	m_matObjWorld[G_OBJ_DROPSHIP]._43 = -1000.0f;

	m_Obj[G_OBJ_CAR].Load(g_pd3dDevice, G_OBJ_LOC_CAR, G_SHA_BOX);
	D3DXMatrixScaling(&m_matObjWorld[G_OBJ_CAR], 0.3, 0.3, 0.3);
	m_matObjWorld[G_OBJ_CAR]._41 = 500.0f;
	m_matObjWorld[G_OBJ_CAR]._42 = 0.0f;
	m_matObjWorld[G_OBJ_CAR]._43 = -700.0f;

	m_Obj[G_OBJ_CAR1].Load(g_pd3dDevice, G_OBJ_LOC_CAR, G_SHA_BOX);
	D3DXMatrixScaling(&m_matObjWorld[G_OBJ_CAR1], 0.3, 0.3, 0.3);
	m_matObjWorld[G_OBJ_CAR1]._41 = -900.0f;
	m_matObjWorld[G_OBJ_CAR1]._42 = 0.0f;
	m_matObjWorld[G_OBJ_CAR1]._43 = 700.0f;

	//--------------------------------------------------------------------------------------
	//  ���� Ʈ��
	//--------------------------------------------------------------------------------------
	m_QuadTree.Build(MapDesc.iNumCols, MapDesc.iNumRows);

	m_QuadTree.Update(g_pd3dDevice, m_pCamera);

	//m_QuadTree.AddObject(&m_tbsobj[iBox]);


#endif

	return true;
}

#ifdef G_MACRO_MAP_ADD
void GSeqSinglePlay::DrawSelectTreeLevel(D3DXMATRIX* pView, D3DXMATRIX* pProj)
{
	//for (int iObj = 0; iObj < m_QuadTree.m_DrawObjList.size(); iObj++)
	//{
	//	GMapObject* pBox = dynamic_cast<GMapObject*>(m_QuadTree.m_DrawObjList[iObj]);
	//	m_pBoxs->SetAmbientColor(pBox->m_vColor.x, pBox->m_vColor.y, pBox->m_vColor.z, pBox->m_vColor.w);
	//	m_pBoxs->SetMatrix(&pBox->m_matWorld, pView, pProj);
	//	m_pBoxs->Render(g_pImmediateContext);
	//}
}

bool GSeqSinglePlay::DrawQuadLine(GNode* pNode)
{
	if (pNode == NULL) return true;
	if (m_QuadTree.m_iRenderDepth == pNode->m_iDepth ||
		(pNode->m_isLeaf &&  m_QuadTree.m_iRenderDepth < 0))
	{
		m_DrawLine.SetMatrix(m_pCamera->GetWorldMatrix(), m_pCamera->GetViewMatrix(), m_pCamera->GetProjMatrix());

		D3DXVECTOR4 vColor = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
		if (pNode->m_iDepth == 1) vColor = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		if (pNode->m_iDepth == 2) vColor = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		if (pNode->m_iDepth == 3) vColor = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
		if (pNode->m_iDepth == 4) vColor = D3DXVECTOR4(1.0f, 0.0f, 1.0f, 1.0f);
		if (pNode->m_iDepth == 5) vColor = D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f);
		if (pNode->m_iDepth == 6) vColor = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		if (pNode->m_iDepth == 7) vColor = D3DXVECTOR4(0.5f, 0.5f, 0.5f, 1.0f);
		if (pNode->m_iDepth == 8) vColor = D3DXVECTOR4(1.0f, 0.5f, 0.5f, 1.0f);
		if (pNode->m_iDepth == 9) vColor = D3DXVECTOR4(1.0f, 0.5f, 1.0f, 1.0f);

		D3DXVECTOR3 vPoint[8];
		vPoint[0] = D3DXVECTOR3(pNode->m_tBox.vMin.x, pNode->m_tBox.vMax.y, pNode->m_tBox.vMin.z);
		vPoint[1] = D3DXVECTOR3(pNode->m_tBox.vMax.x, pNode->m_tBox.vMax.y, pNode->m_tBox.vMin.z);
		vPoint[2] = D3DXVECTOR3(pNode->m_tBox.vMax.x, pNode->m_tBox.vMin.y, pNode->m_tBox.vMin.z);
		vPoint[3] = D3DXVECTOR3(pNode->m_tBox.vMin.x, pNode->m_tBox.vMin.y, pNode->m_tBox.vMin.z);
		vPoint[4] = D3DXVECTOR3(pNode->m_tBox.vMin.x, pNode->m_tBox.vMax.y, pNode->m_tBox.vMax.z);
		vPoint[5] = D3DXVECTOR3(pNode->m_tBox.vMax.x, pNode->m_tBox.vMax.y, pNode->m_tBox.vMax.z);
		vPoint[6] = D3DXVECTOR3(pNode->m_tBox.vMax.x, pNode->m_tBox.vMin.y, pNode->m_tBox.vMax.z);
		vPoint[7] = D3DXVECTOR3(pNode->m_tBox.vMin.x, pNode->m_tBox.vMin.y, pNode->m_tBox.vMax.z);

		m_DrawLine.Draw(g_pImmediateContext, vPoint[0], vPoint[1], vColor);
		m_DrawLine.Draw(g_pImmediateContext, vPoint[1], vPoint[2], vColor);
		m_DrawLine.Draw(g_pImmediateContext, vPoint[2], vPoint[3], vColor);
		m_DrawLine.Draw(g_pImmediateContext, vPoint[3], vPoint[0], vColor);

		m_DrawLine.Draw(g_pImmediateContext, vPoint[4], vPoint[5], vColor);
		m_DrawLine.Draw(g_pImmediateContext, vPoint[5], vPoint[6], vColor);
		m_DrawLine.Draw(g_pImmediateContext, vPoint[6], vPoint[7], vColor);
		m_DrawLine.Draw(g_pImmediateContext, vPoint[7], vPoint[0], vColor);

		m_DrawLine.Draw(g_pImmediateContext, vPoint[0], vPoint[4], vColor);
		m_DrawLine.Draw(g_pImmediateContext, vPoint[1], vPoint[5], vColor);
		m_DrawLine.Draw(g_pImmediateContext, vPoint[2], vPoint[6], vColor);
		m_DrawLine.Draw(g_pImmediateContext, vPoint[3], vPoint[7], vColor);
	}
	for (int iNode = 0; iNode < pNode->m_ChildList.size(); iNode++)
	{
		DrawQuadLine(pNode->m_ChildList[iNode]);
	}
	return true;
}
#endif

bool GSeqSinglePlay::Render()
{
	//float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
	//g_pImmediateContext->ClearRenderTargetView(GetRenderTargetView(), ClearColor);


#ifdef G_MACRO_MAP_ADD
	//--------------------------------------------------------------------------------------
	// ��ī�� �ڽ�
	//--------------------------------------------------------------------------------------
	DX::ApplyRS(g_pImmediateContext, DX::GDxState::g_pRSNoneCullSolid);
	DX::ApplyDSS(g_pImmediateContext, DX::GDxState::g_pDSSDepthDisable);
	DX::ApplyBS(g_pImmediateContext, DX::GDxState::g_pAlphaBlend);

	g_pImmediateContext->PSSetSamplers(0, 1, &DX::GDxState::g_pSSWrapLinear);
	m_pSkyBoxObj->SetMatrix(0, m_pMainCamera->GetViewMatrix(), m_pMainCamera->GetProjMatrix());
	m_pSkyBoxObj->Render(g_pImmediateContext);
	//--------------------------------------------------------------------------------------
	// Ŀ���� ��
	//--------------------------------------------------------------------------------------
	m_CustomMap.SetMatrix(m_pMainCamera->GetWorldMatrix(), m_pMainCamera->GetViewMatrix(),
		m_pMainCamera->GetProjMatrix());
	m_CustomMap.Render(g_pImmediateContext);

	DrawSelectTreeLevel(m_pCamera->GetViewMatrix(), m_pCamera->GetProjMatrix());
	if (m_bDebugRender)
	{
		DrawQuadLine(m_QuadTree.m_pRootNode);
	}

	for (int i = 0; i < G_OBJ_CNT; i++) {
	m_Obj[i].SetMatrix(&m_matObjWorld[i], m_pCamera->GetViewMatrix(), m_pCamera->GetProjMatrix());
	m_Obj[i].Render(g_pImmediateContext);		
	}

#endif


#ifdef G_MACRO_CHAR_ADD 

	D3DXMATRIX matCharWld;
	matCharWld = m_matWorld;
	matCharWld._42 = 20.0f;

	for (int iChar = 0; iChar < m_HeroObj.size(); iChar++)
	{
		//m_matWorld._41 = -50.0f + iChar * 25.0f;
		m_HeroObj[iChar]->SetMatrix(&matCharWld, m_pCamera->GetViewMatrix(), m_pCamera->GetProjMatrix());
		m_HeroObj[iChar]->Render(g_pImmediateContext);
	}
#endif

	m_ObjGun.SetMatrix(NULL, m_pCamera->GetViewMatrix(), m_pCamera->GetProjMatrix());
	
	//if(!m_bDebugCamera)
	//	m_ObjGun.Render(g_pImmediateContext);

#ifdef G_MACRO_EFFECT_ADD
	D3DXVECTOR4 vColor = D3DXVECTOR4(0, 0, 0, 0);
	float fValue = cosf(g_pMain->m_Timer.GetElapsedTime())*0.5f + 0.5f;
	FLOAT fFactor[4] = { 0 , };
	m_vMaterial.x = 1.0f;
	m_vMaterial.y = 1.0f;
	m_vMaterial.z = 1.0f;
	m_vMaterial.w = fValue;
	m_pSprite.get()->m_cbData.Color = m_vMaterial;
	ApplyBS(g_pImmediateContext, GDxState::g_pBSAlphaOne, fFactor, 0xffffffff);

	//if (m_RT.Begin(g_pImmediateContext, vColor))
	//	{
	ApplyRS(g_pMain->GetContext(), GDxState::g_pRSBackCullSolid);
	ApplyDSS(g_pMain->GetContext(), GDxState::g_pDSSDepthEnable);

	D3DXMATRIX matEffectWld;
	matEffectWld = m_matWorld;
	matEffectWld._42 = 10.0f;
	m_pSprite.get()->SetMatrix(&matEffectWld, m_pCamera->GetViewMatrix(), m_pCamera->GetProjMatrix());
	m_pSprite.get()->Render(g_pImmediateContext);

	//	m_RT.Apply(g_pImmediateContext, GetRenderTargetView(), GetDepthStencilView());
	//	m_RT.End(g_pImmediateContext);
	//	}

	ApplyDSS(g_pMain->GetContext(), GDxState::g_pDSSDepthDisable);
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity(&matIdentity);
	m_pScreen->SetMatrix(&matIdentity, &matIdentity, &matIdentity);
	m_pScreen->PreRender(g_pImmediateContext);
	g_pImmediateContext->OMSetBlendState(GDxState::g_pBSAlphaOne, 0, -1);
	//	g_pImmediateContext->PSSetShaderResources(0, 1, m_RT.m_pSRV.GetAddressOf());
	g_pImmediateContext->PSSetShader(m_pPS.Get(), NULL, 0);
	g_pImmediateContext->PSSetConstantBuffers(0, 1, m_pSprite.get()->m_dxobj.g_pConstantBuffer.GetAddressOf());
	m_pScreen->PostRender(g_pImmediateContext);




	ApplyDSS(g_pImmediateContext, GDxState::g_pDSSDepthDisable);
	ApplyBS(g_pImmediateContext, GDxState::g_pBSOneZero);

	ClearD3D11DeviceContext(g_pImmediateContext);
#endif
	return true;
}
bool GSeqSinglePlay::Release()
{
	m_ObjGun.Release();
#ifdef G_MACRO_MAP_ADD
	m_CustomMap.Release();
	m_QuadTree.Release();

	for (int i = 0; i < G_OBJ_CNT; i++) {
		m_Obj[i].Release();
	}
#endif
#ifdef G_MACRO_CHAR_ADD 
	I_CharMgr.Release();

#endif
	return true;
}

bool GSeqSinglePlay::Frame()
{
	if (I_Input.KeyCheck(DIK_LCONTROL) == KEY_UP) {
		
		if(m_bDebugCamera){
			m_pCamera = m_pFPSCamera.get();
			m_bDebugCamera = false;
		}
		else {
			m_pCamera = m_pDebugCamera.get();
			m_bDebugCamera = true;
		}
	}

	// 2�ʴ� 1ȸ��( 1 �� * D3DX_PI = 3.14 )
	float t = g_pMain->m_Timer.GetElapsedTime() * D3DX_PI;
	//m_pMainCamera->Update(g_fSecPerFrame);


	m_pCamera->Frame();


	UpdateGunPosition();
	m_ObjGun.Frame();

#ifdef G_MACRO_MAP_ADD

	

	if (I_Input.KeyCheck(DIK_F1) == KEY_UP)
	{
		if (++m_iDrawDepth > 7) m_iDrawDepth = -1;
		m_QuadTree.SetRenderDepth(m_iDrawDepth);
	}
	m_QuadTree.Update(g_pd3dDevice, m_pCamera);

	
	if (I_Input.KeyCheck(DIK_O) == KEY_UP)
	{
		m_bDebugRender = !m_bDebugRender;
	}
	if (I_Input.KeyCheck(DIK_GRAVE) == KEY_UP) //���ΰ� ��� ~
	{
		m_QuadTree.SetThresHold(!m_QuadTree.m_bThresHoldValue);
	}



	m_QuadTree.Frame();
	

	for (int i = 0; i < G_OBJ_CNT; i++) {
		m_Obj[i].Frame();
	}
#endif

	m_matWorld = *m_pCamera->GetWorldMatrix();
	m_matWorld._41 = 0.0f;
	m_matWorld._42 = 0.0f;
	m_matWorld._43 = 0.0f;



#ifdef G_MACRO_EFFECT_ADD
	//--------------------------------------------------------------------------------------
	// ������ ���
	//-----------------------------------------------------------------------------------
	FLOAT fDeterminant;
	D3DXMATRIX matBillboard;
	D3DXMatrixInverse(&matBillboard, &fDeterminant, m_pCamera->GetViewMatrix());
	matBillboard._41 = 0.0f;
	matBillboard._42 = 0.0f;
	matBillboard._43 = 0.0f;
	matBillboard._44 = 1.0f;


	m_pSprite.get()->SetMatrix(&matBillboard, m_pCamera->GetViewMatrix(), m_pCamera->GetProjMatrix());
	m_pSprite.get()->Frame(g_pImmediateContext, g_pMain->m_Timer.GetElapsedTime(), g_fSecPerFrame);
#endif


#ifdef G_MACRO_CHAR_ADD 




	for (int iChar = 0; iChar < m_HeroObj.size(); iChar++)
	{
		if (I_Input.KeyCheck(DIK_ADD))
		{
			m_HeroObj[iChar]->m_fSpeed += g_fSecPerFrame;
		}
		if (I_Input.KeyCheck(DIK_SUBTRACT))
		{
			m_HeroObj[iChar]->m_fSpeed -= g_fSecPerFrame;
			if (m_HeroObj[iChar]->m_fSpeed < 0.0f) m_HeroObj[iChar]->m_fSpeed = 0.01f;
		}
		m_HeroObj[iChar]->Frame();
	}
	if (I_Input.KeyCheck(DIK_F10) == KEY_UP && m_HeroObj.size() > 1)
	{
		m_HeroObj[1]->SetActionFrame(120, 205);//jump		
	}
	if (I_Input.KeyCheck(DIK_F11) == KEY_UP&& m_HeroObj.size() > 1)
	{
		m_HeroObj[1]->SetActionFrame(205, 289);//attack		
	}

	if (I_Input.KeyCheck(DIK_F12) == KEY_UP)
	{
		for (int iChar = 0; iChar < m_HeroObj.size(); iChar++)
		{
			m_HeroObj[iChar]->m_bBoneRender = !m_HeroObj[iChar]->m_bBoneRender;
		}
	}

	if (I_Input.KeyCheck(DIK_O) == KEY_UP)
	{
		Load();
	}
#endif
	return true;


}

//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
HRESULT GSeqSinglePlay::CreateResource()
{
	HRESULT hr;
	if (m_pCamera)
	{
		float fAspectRatio = g_pMain->m_SwapChainDesc.BufferDesc.Width /
			(float)g_pMain->m_SwapChainDesc.BufferDesc.Height;
		m_pCamera->SetProjMatrix(D3DX_PI / 4, fAspectRatio, 0.1f, 500.0f);
	}
	return S_OK;
}
//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
HRESULT GSeqSinglePlay::DeleteResource()
{
	HRESULT hr = S_OK;
	if (g_pImmediateContext) g_pImmediateContext->ClearState();
	return S_OK;
}
GSeqSinglePlay::GSeqSinglePlay(void)
{
	m_bDebugCamera = true;
	m_pCamera = nullptr;
#ifdef G_MACRO_EFFECT_ADD
	m_pSprite = nullptr;
#endif
}

GSeqSinglePlay::~GSeqSinglePlay(void)
{
}
int GSeqSinglePlay::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (m_pCamera != nullptr)
	{
		m_pCamera->WndProc(hWnd, msg, wParam, lParam);
	}
	return -1;
}




