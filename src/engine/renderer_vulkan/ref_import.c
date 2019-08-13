#include "../qcommon/q_shared.h"
#include "../renderercommon/tr_public.h"
#include "render_export.h"

// extern refexport_t* R_Export(void);

refimport_t	ri;


/*
@@@@@@@@@@@@@@@@@@@@@
GetRefAPI

@@@@@@@@@@@@@@@@@@@@@
*/

#ifdef USE_RENDERER_DLOPEN
Q_EXPORT void QDECL GetRefAPI( int apiVersion, refimport_t * const pRImp, refexport_t* const pRExp)
{
#else
void GetRefAPI(int apiVersion, refimport_t *rimp, refexport_t* const pRExp)
{
#endif

	ri = *pRImp;

	if( apiVersion != REF_API_VERSION )
	{
		ri.Printf(PRINT_WARNING, "Mismatched REF_API_VERSION: expected %i, got %i\n", REF_API_VERSION, apiVersion );
		return ;
	}

	// the RE_ functions are Renderer Entry points
	pRExp->Shutdown = RE_Shutdown;
	pRExp->BeginRegistration = RE_BeginRegistration;
	pRExp->RegisterModel = RE_RegisterModel;
	pRExp->RegisterSkin = RE_RegisterSkin;
	pRExp->RegisterShader = RE_RegisterShader;
	pRExp->RegisterShaderNoMip = RE_RegisterShaderNoMip;
	pRExp->LoadWorld = RE_LoadWorldMap;
	pRExp->SetWorldVisData = RE_SetWorldVisData;
	pRExp->EndRegistration = RE_EndRegistration;
	pRExp->ClearScene = RE_ClearScene;
	pRExp->AddRefEntityToScene = RE_AddRefEntityToScene;
	pRExp->AddPolyToScene = RE_AddPolyToScene;
	pRExp->LightForPoint = RE_LightForPoint;
	pRExp->AddLightToScene = RE_AddLightToScene;
	pRExp->AddAdditiveLightToScene = RE_AddAdditiveLightToScene;

	pRExp->RenderScene = RE_RenderScene;
	pRExp->SetColor = RE_SetColor;
	pRExp->DrawStretchPic = RE_StretchPic;
	pRExp->DrawStretchRaw = RE_StretchRaw;
	pRExp->UploadCinematic = RE_UploadCinematic;

	pRExp->BeginFrame = RE_BeginFrame;
	pRExp->EndFrame = RE_EndFrame;
	pRExp->MarkFragments = RE_MarkFragments;
	pRExp->LerpTag = RE_LerpTag;
	pRExp->ModelBounds = RE_ModelBounds;
	pRExp->RegisterFont = RE_RegisterFont;
	pRExp->RemapShader = RE_RemapShader;
	pRExp->GetEntityToken = RE_GetEntityToken;
	pRExp->inPVS = RE_inPVS;

	pRExp->TakeVideoFrame = RE_TakeVideoFrame;
	pRExp->WinMessage = RE_WinMessage;

    //return R_Export();
}
