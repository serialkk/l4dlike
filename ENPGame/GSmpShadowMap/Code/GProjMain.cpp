#include "GProjMain.h"

GProjMain* g_pMain;

//const float g_fMaxSize = 1024;
bool GProjMain::Init()
{
	I_CharMgr.Init();
	if (!I_CharMgr.Load(GetDevice(), m_pImmediateContext, _T("data/CharZombie_shadow.gci")))
	{
		return false;
	}
	GCharacter* pChar0 = I_CharMgr.GetPtr(L"ZOMBIE_IDLE");
	shared_ptr<GZombie> pObjA = make_shared<GZombie>();
	pObjA->Set(pChar0,
		pChar0->m_pBoneObject,
		pChar0->m_pBoneObject->m_Scene.iFirstFrame,
		pChar0->m_pBoneObject->m_Scene.iLastFrame);
	m_HeroObj.push_back(pObjA);


	D3DXMatrixIdentity(&m_matObjWld);
	D3DXMatrixIdentity(&m_matObjScl);
	D3DXMatrixIdentity(&m_matObjRot);
	D3DXMatrixIdentity(&m_matObjTrans);

	int iIndex = -1;
	//iIndex = I_ObjMgr.Load(g_pd3dDevice, G_OBJ_LOC_DROPSHIP_LAND, G_SHA_OBJ_SPECULAR_SHADOW, G_LIGHT_TYPE_SPECULAR);				if (iIndex < 0) return false;

	iIndex = I_ObjMgr.Load(g_pd3dDevice, G_OBJ_LOC_DROPSHIP_LAND, G_SHA_OBJ_DIFFUSE_SHADOW, G_LIGHT_TYPE_DIFFUSE);				if (iIndex < 0) return false;

	//연구소 로드
	m_Obj = (GGbsObj*)I_ObjMgr.GetPtr(G_OBJ_NAME_DROPSHIP_LAND);
	m_Obj2 = (GGbsObj*)I_ObjMgr.GetPtr(G_OBJ_NAME_DROPSHIP_LAND);
	D3DXMatrixScaling(&m_matObjScl, 1, 1, 1);
	D3DXMatrixTranslation(&m_matObjTrans, 0.0f, 0.0f, 0.0f);
	m_matObjWld = m_matObjScl * m_matObjRot * m_matObjTrans;





	TMapDesc MapDesc = { 33, 33, 1.0f,1.0f, L"data_test/castle.jpg"/* L"data_test/write.png"*/, L"data/shader_shadow/CustomizeMap_shadow.hlsl" };
	m_CustomMap.Init( GetDevice(), m_pImmediateContext );
	if( FAILED( m_CustomMap.Load(MapDesc) ))
	{
		return false;
	}	
	if( FAILED( CreateResource() ))
	{
		return false;
	}	

	SAFE_NEW( m_pQuad, GPlaneShape );
	m_pQuad->SetScreenVertex(15, 15, 300, 300, 
		D3DXVECTOR2( m_iWindowWidth, m_iWindowHeight ) );
	if( FAILED( m_pQuad->Create(GetDevice(), L"data/shader/plane.hlsl", L"data_test/castle.jpg") ) )
	{
		MessageBox( 0, _T("m_pLIne 실패"), _T("Fatal error"), MB_OK );
		return false;
	}
	
	for( int iObj=0; iObj < MAX_OBJECT_CNT; iObj++ )
	{
		D3DXMatrixIdentity(&m_matWorld[iObj]);
	}
	//--------------------------------------------------------------------------------------
	// 박스 오브젝트 생성
	//--------------------------------------------------------------------------------------
	SAFE_NEW(m_pBoxShape, GBoxShape);
	if (FAILED(m_pBoxShape->Create(GetDevice(), L"data/shader_shadow/Obj_Diffuse_shadow.hlsl", L"data_test/castle.jpg")))
	{
		MessageBox(0, _T("m_pBoxShape 실패"), _T("Fatal error"), MB_OK);
		return 0;
	}
	ComPtr<ID3DBlob> pVSBlob;
	m_pShadowVS.Attach(DX::LoadVertexShaderFile(GetDevice(), L"data/shader_shadow/CustomizeMap_shadow.hlsl", pVSBlob.GetAddressOf(), "SHADOW_VS"));
	m_pShadowPS.Attach(DX::LoadPixelShaderFile(GetDevice(), L"data/shader_shadow/CustomizeMap_shadow.hlsl", "SHADOW_PS"));
	
	m_pObjShadowVS.Attach(DX::LoadVertexShaderFile(GetDevice(), G_SHA_OBJ_DIFFUSE_SHADOW, pVSBlob.GetAddressOf(), "SHADOW_VS"));
	m_pObjShadowPS.Attach(DX::LoadPixelShaderFile(GetDevice(), G_SHA_OBJ_DIFFUSE_SHADOW, "SHADOW_PS"));

	m_pCharShadowVS.Attach(DX::LoadVertexShaderFile(GetDevice(), L"data/shader_shadow/Character_shadow.hlsl", pVSBlob.GetAddressOf(), "SHADOW_VS"));
	m_pCharShadowPS.Attach(DX::LoadPixelShaderFile(GetDevice(), L"data/shader_shadow/Character_shadow.hlsl", "SHADOW_PS"));

	// Create Source and Dest textures
	m_RT.m_DSFormat = DXGI_FORMAT_R32_TYPELESS;
	m_RT.Create(GetDevice(), g_fMaxSize, g_fMaxSize);// m_bColorTexRender );	
	//--------------------------------------------------------------------------------------
	// 카메라  행렬 
	//--------------------------------------------------------------------------------------	
	m_pMainCamera = make_shared<GCamera>();
	m_pMainCamera->SetViewMatrix(D3DXVECTOR3(0.0f, 10.0f, -20.0f),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_pMainCamera->SetProjMatrix(D3DX_PI * 0.25f, m_SwapChainDesc.BufferDesc.Width / (float)(m_SwapChainDesc.BufferDesc.Height),
		1.0f, 3000.0f);


	//m_ShaderEditer.Create( L"CustomizeMap.fx", m_hWnd );
	m_matTexture = D3DXMATRIX(	  0.5f, 0.0f, 0.0f, 0.0f
								, 0.0f,-0.5f, 0.0f, 0.0f
								, 0.0f, 0.0f, 1.0f, 0.0f
								, 0.5f, 0.5f, 0.0f, 1.0f);
	m_pShadowConstantBuffer.Attach(DX::CreateConstantBuffer(m_pd3dDevice, &m_cbShadow, 1, sizeof(SHADOW_CONSTANT_BUFFER)));


	//m_vLightPos = D3DXVECTOR3( 15, 40, -35 );
	m_vLightPos = D3DXVECTOR3(100, 100, 0);

	float fWidthLength = m_CustomMap.m_fSellDistance*m_CustomMap.m_iNumCols*
		m_CustomMap.m_fSellDistance*m_CustomMap.m_iNumCols;
	float fHeightLength = m_CustomMap.m_fSellDistance*m_CustomMap.m_iNumRows*
		m_CustomMap.m_fSellDistance*m_CustomMap.m_iNumRows;
	
	// 지형의 대각선의 길이, 텍스처에 적합하게 배치하려고 작업한다.
	float fMaxViewDistance = sqrt(fWidthLength + fHeightLength);
	//D3DXMatrixPerspectiveFovLH( &m_matShadowProj, D3DX_PI*0.25f, 1.0f, 20.0f, 200.0f );
	//D3DXMatrixOrthoLH( &m_matShadowProj, fMaxViewDistance, fMaxViewDistance, 0.0f, 100.0f );
	
	//D3DXMatrixOrthoOffCenterLH( &m_matShadowProj,	
	//	-fMaxViewDistance/2, fMaxViewDistance/2, -fMaxViewDistance/2, fMaxViewDistance/2, 0.0f, 10000.0f );
	D3DXMatrixOrthoOffCenterLH(&m_matShadowProj,
		-fMaxViewDistance*2, fMaxViewDistance*2, -fMaxViewDistance*2, fMaxViewDistance*2, 0.0f, 10000.0f);


	D3DXMATRIX matScale, matRotation;
	for( int iObj=0; iObj < MAX_OBJECT_CNT; iObj++ )
	{
		D3DXMatrixTranslation( &m_matWorld[iObj],	randstep(-15.0f, 15.0f), 
													randstep(0, 20), 
													randstep(-15.0f, 15.0f) );
		D3DXMatrixScaling( &matScale,	randstep(1.0f, 5.0f), 
										randstep(1.0f, 5.0f), 
										randstep(1.0f, 5.0f) );
		m_matWorld[iObj]._11 = matScale._11;
		m_matWorld[iObj]._22 = matScale._22;
		m_matWorld[iObj]._33 = matScale._33;

		m_matWorld[iObj]._41 = 0.0f;
		m_matWorld[iObj]._42 = m_matWorld[iObj]._22*(iObj+1);
		m_matWorld[iObj]._43 = 0.0f;		
	}

	return true;
}
bool GProjMain::Render()
{	
	ID3D11ShaderResourceView* pSRVs[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	GetContext()->PSSetShaderResources(0, 16, pSRVs);

	D3DXVECTOR4 vClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	D3DXVECTOR3 vLookat = D3DXVECTOR3( 0, 0, 0);
	D3DXVECTOR3 vUp     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	//-----------------------------------------------------
	// 지형 및 오브젝트의 그림자맵 생성
	//-----------------------------------------------------		
	if( m_RT.Begin(m_pImmediateContext,vClearColor) )
	{					
		D3DXMatrixLookAtLH( &m_matShadowView, &m_vLightPos, &vLookat, &vUp );		
		RenderShadow( NULL, &m_matShadowView, &m_matShadowProj );
		m_RT.End(m_pImmediateContext);
	}	
	////-----------------------------------------------------
	//// 오브젝트 + 쉐도우 랜더링
	////-----------------------------------------------------	
	RenderObject( m_pMainCamera->GetViewMatrix(), m_pMainCamera->GetProjMatrix());
	////-----------------------------------------------------
	//// 1패스: 지형 + 쉐도우 랜더링
	////-----------------------------------------------------		
	ApplySS(GetContext(), GDxState::g_pSSClampLinear, 1);
	ApplySS(GetContext(), GDxState::g_pSSShadowMap, 2);
	ApplyRS(GetContext(), GDxState::g_pRSBackCullSolid);

	m_CustomMap.SetMatrix( NULL,m_pMainCamera->GetViewMatrix(), m_pMainCamera->GetProjMatrix());	
	D3DXMATRIX matWVPT = m_matShadowView * m_matShadowProj * m_matTexture;
	D3DXMatrixTranspose(&m_cbShadow.g_matShadow, &matWVPT);
	m_cbShadow.g_ShadowID = 0;
	m_cbShadow.g_iNumKernel = 3;
	GetContext()->UpdateSubresource(m_pShadowConstantBuffer.Get(), 0, NULL, &m_cbShadow, 0, 0);
	GetContext()->VSSetConstantBuffers(2, 1, m_pShadowConstantBuffer.GetAddressOf());
	GetContext()->PSSetConstantBuffers(2, 1, m_pShadowConstantBuffer.GetAddressOf());

	m_CustomMap.PreRender(GetContext());
		GetContext()->PSSetShaderResources(1, 1, m_RT.m_pDsvSRV.GetAddressOf());
	m_CustomMap.PostRender(GetContext());
	
	/////////////////////////////////////////
	// 디버깅
	/////////////////////////////////////////
	if( m_bColorTexRender )
	{
		m_pQuad->SetMatrix(NULL, NULL, NULL );
		m_pQuad->PreRender(GetContext());
		//GetContext()->PSSetShaderResources(0, 1, m_RT.m_pSRV.GetAddressOf());
		GetContext()->PSSetShaderResources(0, 1, m_RT.m_pDsvSRV.GetAddressOf());
		m_pQuad->PostRender(GetContext());
	}
	return true;
}
void GProjMain::RenderObject( D3DXMATRIX* matView, D3DXMATRIX* matProj )
{
	D3DXMATRIX matInvView;
	ApplySS(GetContext(), GDxState::g_pSSClampLinear, 1);
	ApplySS(GetContext(), GDxState::g_pSSShadowMap, 2);
	ApplyRS(GetContext(), GDxState::g_pRSBackCullSolid);

	m_pBoxShape->PreRender(GetContext());
	for( int iObj=0; iObj < MAX_OBJECT_CNT; iObj++ )
	{
		D3DXMatrixInverse(&matInvView, 0, matView );
		D3DXMATRIX matWVPT1 = m_matWorld[iObj] * m_matShadowView * m_matShadowProj * m_matTexture;
		D3DXMatrixTranspose(&m_cbShadow.g_matShadow, &matWVPT1);
		m_cbShadow.g_ShadowID = m_fObjID[iObj];
		m_cbShadow.g_iNumKernel = 3;
		GetContext()->UpdateSubresource(m_pShadowConstantBuffer.Get(), 0, NULL, &m_cbShadow, 0, 0);
		GetContext()->VSSetConstantBuffers(2, 1, m_pShadowConstantBuffer.GetAddressOf());
		GetContext()->PSSetConstantBuffers(2, 1, m_pShadowConstantBuffer.GetAddressOf());
		GetContext()->PSSetShaderResources(1, 1, m_RT.m_pDsvSRV.GetAddressOf() );

		m_pBoxShape->SetMatrix(&m_matWorld[iObj], matView, matProj);		
		m_pBoxShape->PostRender(GetContext());
	}

	for (int iChar = 0; iChar < m_HeroObj.size(); iChar++)
	{
		D3DXMatrixInverse(&matInvView, 0, matView);
		D3DXMATRIX matWVPT1 = m_HeroObj[iChar]->m_matWorld * m_matShadowView * m_matShadowProj * m_matTexture;
		D3DXMatrixTranspose(&m_cbShadow.g_matShadow, &matWVPT1);
		//m_cbShadow.g_ShadowID = m_fObjID[iObj];
		m_cbShadow.g_iNumKernel = 3;
		GetContext()->UpdateSubresource(m_pShadowConstantBuffer.Get(), 0, NULL, &m_cbShadow, 0, 0);
		GetContext()->VSSetConstantBuffers(2, 1, m_pShadowConstantBuffer.GetAddressOf());
		GetContext()->PSSetConstantBuffers(2, 1, m_pShadowConstantBuffer.GetAddressOf());
		GetContext()->PSSetShaderResources(1, 1, m_RT.m_pDsvSRV.GetAddressOf());

		m_HeroObj[iChar]->SetMatrix(&m_HeroObj[iChar]->m_matWorld, matView, matProj);
		
		//m_HeroObj[iChar]->Render(GetContext());
		
		// 메쉬 랜더링
		for (int iObj = 0; iObj < m_HeroObj[iChar]->m_pChar->m_pModelList.size(); iObj++)
		{
			GSkinObj* pModel = (GSkinObj*)m_HeroObj[iChar]->m_pChar->m_pModelList[iObj]->m_pModel;
			_ASSERT(pModel);
			pModel->SetMatrix(&m_HeroObj[iChar]->m_matWorld, &m_HeroObj[iChar]->m_matView, &m_HeroObj[iChar]->m_matProj);
			ID3D11ShaderResourceView* aRViews[1] = { m_HeroObj[iChar]->m_pBoneBufferRV.Get() };
			g_pImmediateContext->VSSetShaderResources(1, 1, aRViews);
			//pModel->Render(GetContext());
			pModel->PreRender(GetContext());
			pModel->Draw(GetContext(), pModel);
		}
		
	}



	/*
	m_Obj->SetMatrix(&m_matObjWld, matView, matProj);
	m_Obj->Render(GetContext());
	*/
	m_matObjWld._41 = -40.0f;
	m_Obj->SetMatrix(&m_matObjWld, matView, matProj);
	


	D3DXMatrixInverse(&matInvView, 0, matView);
	D3DXMATRIX matWVPT1 = m_matObjWld * m_matShadowView * m_matShadowProj * m_matTexture;
	D3DXMatrixTranspose(&m_cbShadow.g_matShadow, &matWVPT1);
	m_cbShadow.g_ShadowID = 0;
	m_cbShadow.g_iNumKernel = 3;
	GetContext()->UpdateSubresource(m_pShadowConstantBuffer.Get(), 0, NULL, &m_cbShadow, 0, 0);
	GetContext()->VSSetConstantBuffers(2, 1, m_pShadowConstantBuffer.GetAddressOf());
	GetContext()->PSSetConstantBuffers(2, 1, m_pShadowConstantBuffer.GetAddressOf());

	//m_Obj->m_cbLight.g_vEyeDir.x = m_pMainCamera->m_vLookVector.x;
	//m_Obj->m_cbLight.g_vEyeDir.y = m_pMainCamera->m_vLookVector.y;
	//m_Obj->m_cbLight.g_vEyeDir.z = m_pMainCamera->m_vLookVector.z;

	

	//ID3D11ShaderResourceView* aRViews[1] = { m_Obj->m_pBoneBufferRV.Get() };
	//g_pImmediateContext->VSSetShaderResources(1, 1, aRViews);
	m_Obj->PreRender(GetContext());
	GetContext()->PSSetShaderResources(1, 1, m_RT.m_pDsvSRV.GetAddressOf());
	m_Obj->Draw(GetContext(), m_Obj);





	m_matObjWld._41 = 30.0f;
	m_Obj2->SetMatrix(&m_matObjWld, matView, matProj);
	


	D3DXMatrixInverse(&matInvView, 0, matView);
	matWVPT1 = m_matObjWld * m_matShadowView * m_matShadowProj * m_matTexture;
	D3DXMatrixTranspose(&m_cbShadow.g_matShadow, &matWVPT1);
	m_cbShadow.g_ShadowID = 0;
	m_cbShadow.g_iNumKernel = 3;
	GetContext()->UpdateSubresource(m_pShadowConstantBuffer.Get(), 0, NULL, &m_cbShadow, 0, 0);
	GetContext()->VSSetConstantBuffers(2, 1, m_pShadowConstantBuffer.GetAddressOf());
	GetContext()->PSSetConstantBuffers(2, 1, m_pShadowConstantBuffer.GetAddressOf());

	//m_Obj->m_cbLight.g_vEyeDir.x = m_pMainCamera->m_vLookVector.x;
	//m_Obj->m_cbLight.g_vEyeDir.y = m_pMainCamera->m_vLookVector.y;
	//m_Obj->m_cbLight.g_vEyeDir.z = m_pMainCamera->m_vLookVector.z;

	

	//ID3D11ShaderResourceView* aRViews[1] = { m_Obj->m_pBoneBufferRV.Get() };
	//g_pImmediateContext->VSSetShaderResources(1, 1, aRViews);
	m_Obj2->PreRender(GetContext());
	GetContext()->PSSetShaderResources(1, 1, m_RT.m_pDsvSRV.GetAddressOf());
	m_Obj2->Draw(GetContext(), m_Obj2);
	
}
void GProjMain::RenderShadow( D3DXMATRIX* matShadow, 
						   D3DXMATRIX* matView, D3DXMATRIX* matProj )
{
	ApplyDSS(GetContext(), GDxState::g_pDSSDepthEnable);
	//ApplyRS(GetContext(), TDxState::g_pRSBackCullSolid);
	ApplyBS(GetContext(), GDxState::g_pAlphaBlend);
	ApplyRS(GetContext(), GDxState::g_pRSSlopeScaledDepthBias);

	m_CustomMap.SetMatrix( NULL,matView, matProj);
	m_CustomMap.PreRender(GetContext());
		GetContext()->VSSetShader(m_pShadowVS.Get(), NULL, 0);
		//GetContext()->PSSetShader(m_pShadowPS.Get(), NULL, 0);
		// 깊이스텐실 버퍼를 리소스로 전달하면 되기 때문에 픽쉘쉐이더를 사용하지 않아도 된다.
		// 하지만, 화면에 쿼드로 깊이맵 결과를 확인하고자 위 문장에서 픽쉘쉐이더를 활성화 하였다.
		GetContext()->PSSetShader( NULL, NULL, 0);
	m_CustomMap.PostRender(GetContext());

	for( int iObj=0; iObj < MAX_OBJECT_CNT; iObj++ )
	{
		m_pBoxShape->SetMatrix( &m_matWorld[iObj], matView, matProj );
		m_pBoxShape->PreRender(GetContext());
			GetContext()->VSSetShader(m_pShadowVS.Get(), NULL, 0);
			//GetContext()->PSSetShader(m_pShadowPS.Get(), NULL, 0);
			// 깊이스텐실 버퍼를 리소스로 전달하면 되기 때문에 픽쉘쉐이더를 사용하지 않아도 된다.
			// 하지만, 화면에 쿼드로 깊이맵 결과를 확인하고자 위 문장에서 픽쉘쉐이더를 활성화 하였다.
			GetContext()->PSSetShader( NULL, NULL, 0);
		m_pBoxShape->PostRender(GetContext());
	}

	for (int iChar = 0; iChar < m_HeroObj.size(); iChar++)
	{
		m_HeroObj[iChar]->SetMatrix(&m_HeroObj[iChar]->m_matWorld, matView, matProj);
		
		//m_HeroObj[iChar]->Render(GetContext());
		
		// 메쉬 랜더링
		for (int iObj = 0; iObj < m_HeroObj[iChar]->m_pChar->m_pModelList.size(); iObj++)
		{
			GSkinObj* pModel = (GSkinObj*)m_HeroObj[iChar]->m_pChar->m_pModelList[iObj]->m_pModel;
			_ASSERT(pModel);
			pModel->SetMatrix(&m_HeroObj[iChar]->m_matWorld, matView, matProj);
			ID3D11ShaderResourceView* aRViews[1] = { m_HeroObj[iChar]->m_pBoneBufferRV.Get() };
			g_pImmediateContext->VSSetShaderResources(1, 1, aRViews);
			//pModel->Render(GetContext());
			pModel->PreRender(GetContext());
			
			GetContext()->VSSetShader(m_pCharShadowVS.Get(), NULL, 0);
			GetContext()->PSSetShader(NULL, NULL, 0);
			
			pModel->Draw(GetContext(), pModel);
		}
		
	}
	m_matObjWld._41 = -40.0f;
	m_Obj->SetMatrix(&m_matObjWld, matView, matProj);
	//m_Obj->Render(GetContext());
	m_Obj->PreRender(GetContext());
	GetContext()->VSSetShader(m_pObjShadowVS.Get(), NULL, 0);
	GetContext()->PSSetShader(NULL, NULL, 0);
	m_Obj->Draw(GetContext(), m_Obj);

	m_matObjWld._41 = 30.0f;
	m_Obj2->SetMatrix(&m_matObjWld, matView, matProj);
	//m_Obj->Render(GetContext());
	m_Obj->PreRender(GetContext());
	GetContext()->VSSetShader(m_pObjShadowVS.Get(), NULL, 0);
	GetContext()->PSSetShader(NULL, NULL, 0);
	m_Obj->Draw(GetContext(), m_Obj);


}
bool GProjMain::Release()
{	
	SAFE_DEL(m_pQuad);
	SAFE_DEL(m_pBoxShape);
	
	I_CharMgr.Release();
	I_ObjMgr.Release();

	return m_CustomMap.Release();
}
bool GProjMain::Frame()
{
	// 2초당 1회전( 1 초 * D3DX_PI = 3.14 )
	float t = m_Timer.GetElapsedTime() * D3DX_PI;	
	m_pMainCamera->Frame();



	//m_Obj->SetMatrix(&m_matObjWld, m_pMainCamera->GetViewMatrix(), m_pMainCamera->GetProjMatrix());
	m_Obj->Frame();
	m_Obj2->Frame();

	for (int iChar = 0; iChar < m_HeroObj.size(); iChar++)
	{
		m_HeroObj[iChar]->m_matWorld._41 = -80.0f;
		m_HeroObj[iChar]->m_matWorld._42 = 36.0f;
		m_HeroObj[iChar]->m_matWorld._43 = -10.0f;
		m_HeroObj[iChar]->Frame();
	}

	return m_CustomMap.Frame();
}

//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
HRESULT GProjMain::CreateResource()
{
	HRESULT hr;
	if (m_pMainCamera != nullptr)
	{
		m_pMainCamera->SetProjMatrix((float)D3DX_PI * 0.25f,
			m_SwapChainDesc.BufferDesc.Width / (FLOAT)m_SwapChainDesc.BufferDesc.Height, 1.0f, 3000.0f);
	}
	return S_OK;
}
//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
HRESULT GProjMain::DeleteResource()
{
	HRESULT hr = S_OK;
	if (m_pImmediateContext) m_pImmediateContext->ClearState();
	return S_OK;
}

GProjMain::GProjMain(void)
{
	SAFE_ZERO(m_pBoxShape);
	SAFE_ZERO(m_pQuad);
	m_bColorTexRender = true;
}

GProjMain::~GProjMain(void)
{
}

GCORE_RUN(GBasisSample ShadowMap)