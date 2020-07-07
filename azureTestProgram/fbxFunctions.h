#pragma once

#include <vector>

#ifdef IOS_REF
	#undef  IOS_REF
	#define IOS_REF (*(pManager->GetIOSettings()))
#endif

void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
{
	//The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
	pManager = FbxManager::Create();
	if (!pManager)
	{
		FBXSDK_printf("Error: Unable to create FBX Manager!\n");
		exit(1);
	}
	else FBXSDK_printf("Autodesk FBX SDK version %s\n", pManager->GetVersion());

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
	pManager->SetIOSettings(ios);

	//Load plugins from the executable directory (optional)
	FbxString lPath = FbxGetApplicationDirectory();
	pManager->LoadPluginsDirectory(lPath.Buffer());

	//Create an FBX scene. This object holds most objects imported/exported from/to files.
	pScene = FbxScene::Create(pManager, "My Scene");
	if (!pScene)
	{
		FBXSDK_printf("Error: Unable to create FBX scene!\n");
		exit(1);
	}
}

void DestroySdkObjects(FbxManager* pManager, bool pExitStatus)
{
	//Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
	if (pManager) pManager->Destroy();
	if (pExitStatus) FBXSDK_printf("Program Success!\n");
}

bool SaveScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename, int pFileFormat, bool pEmbedMedia)
{
	int lMajor, lMinor, lRevision;
	bool lStatus = true;

	// Create an exporter.
	FbxExporter* lExporter = FbxExporter::Create(pManager, "");

	if (pFileFormat < 0 || pFileFormat >= pManager->GetIOPluginRegistry()->GetWriterFormatCount())
	{
		// Write in fall back format in less no ASCII format found
		pFileFormat = pManager->GetIOPluginRegistry()->GetNativeWriterFormat();

		//Try to export in ASCII if possible
		int lFormatIndex, lFormatCount = pManager->GetIOPluginRegistry()->GetWriterFormatCount();

		for (lFormatIndex = 0; lFormatIndex < lFormatCount; lFormatIndex++)
		{
			if (pManager->GetIOPluginRegistry()->WriterIsFBX(lFormatIndex))
			{
				FbxString lDesc = pManager->GetIOPluginRegistry()->GetWriterFormatDescription(lFormatIndex);
				const char *lASCII = "ascii";
				if (lDesc.Find(lASCII) >= 0)
				{
					pFileFormat = lFormatIndex;
					break;
				}
			}
		}
	}

	// Set the export states. By default, the export states are always set to 
	// true except for the option eEXPORT_TEXTURE_AS_EMBEDDED. The code below 
	// shows how to change these states.
	IOS_REF.SetBoolProp(EXP_FBX_MATERIAL, true);
	IOS_REF.SetBoolProp(EXP_FBX_TEXTURE, true);
	IOS_REF.SetBoolProp(EXP_FBX_EMBEDDED, pEmbedMedia);
	IOS_REF.SetBoolProp(EXP_FBX_SHAPE, true);
	IOS_REF.SetBoolProp(EXP_FBX_GOBO, true);
	IOS_REF.SetBoolProp(EXP_FBX_ANIMATION, true);
	IOS_REF.SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);

	// Initialize the exporter by providing a filename.
	if (lExporter->Initialize(pFilename, pFileFormat, pManager->GetIOSettings()) == false)
	{
		FBXSDK_printf("Call to FbxExporter::Initialize() failed.\n");
		FBXSDK_printf("Error returned: %s\n\n", lExporter->GetStatus().GetErrorString());
		return false;
	}

	FbxManager::GetFileFormatVersion(lMajor, lMinor, lRevision);
	FBXSDK_printf("FBX file format version %d.%d.%d\n\n", lMajor, lMinor, lRevision);

	// Export the scene.
	lStatus = lExporter->Export(pScene);

	// Destroy the exporter.
	lExporter->Destroy();
	return lStatus;
}

void createNodes(FbxScene* pScene, std::vector<FbxNode*> *nodes) {
	FbxNode* pNodePelvis = FbxNode::Create(pScene, "Pelvis");
	nodes->push_back(pNodePelvis);
	FbxNode* pNodeSpineNaval = FbxNode::Create(pScene, "Spine Naval");
	nodes->push_back(pNodeSpineNaval);
	FbxNode* pNodeSpineChest = FbxNode::Create(pScene, "Spine Chest");
	nodes->push_back(pNodeSpineChest);
	FbxNode* pNodeNeck = FbxNode::Create(pScene, "Neck");
	nodes->push_back(pNodeNeck);
	FbxNode* pNodeClavicleLeft = FbxNode::Create(pScene, "Clavicle Left");
	nodes->push_back(pNodeClavicleLeft);
	FbxNode* pNodeShoulderLeft = FbxNode::Create(pScene, "Shoulder Left");
	nodes->push_back(pNodeShoulderLeft);
	FbxNode* pNodeElbowLeft = FbxNode::Create(pScene, "Elbow Left");
	nodes->push_back(pNodeElbowLeft);
	FbxNode* pNodeWristLeft = FbxNode::Create(pScene, "Wrist Left");
	nodes->push_back(pNodeWristLeft);
	FbxNode* pNodeHandLeft = FbxNode::Create(pScene, "Hand Left");
	nodes->push_back(pNodeHandLeft);
	FbxNode* pNodeHandtipLeft = FbxNode::Create(pScene, "Handtip Left");
	nodes->push_back(pNodeHandtipLeft);
	FbxNode* pNodeThumbLeft = FbxNode::Create(pScene, "Thumb Left");
	nodes->push_back(pNodeThumbLeft);
	FbxNode* pNodeClavicleRight = FbxNode::Create(pScene, "Clavical Right");
	nodes->push_back(pNodeClavicleRight);
	FbxNode* pNodeShoulderRight = FbxNode::Create(pScene, "Shoulder Right");
	nodes->push_back(pNodeShoulderRight);
	FbxNode* pNodeElbowRight = FbxNode::Create(pScene, "Elbow Right");
	nodes->push_back(pNodeElbowRight);
	FbxNode* pNodeWristRight = FbxNode::Create(pScene, "Wrist Right");
	nodes->push_back(pNodeWristRight);
	FbxNode* pNodeHandRight = FbxNode::Create(pScene, "Hand Right");
	nodes->push_back(pNodeHandRight);
	FbxNode* pNodeHandtipRight = FbxNode::Create(pScene, "Handtip Right");
	nodes->push_back(pNodeHandtipRight);
	FbxNode* pNodeThumbRight = FbxNode::Create(pScene, "Thumb Right");
	nodes->push_back(pNodeThumbRight);
	FbxNode* pNodeHipLeft = FbxNode::Create(pScene, "Hip Left");
	nodes->push_back(pNodeHipLeft);
	FbxNode* pNodeKneeLeft = FbxNode::Create(pScene, "Knee Left");
	nodes->push_back(pNodeKneeLeft);
	FbxNode* pNodeAnkleLeft = FbxNode::Create(pScene, "Ankle Left");
	nodes->push_back(pNodeAnkleLeft);
	FbxNode* pNodeFootLeft = FbxNode::Create(pScene, "Foot Left");
	nodes->push_back(pNodeFootLeft);
	FbxNode* pNodeHipRight = FbxNode::Create(pScene, "Hip Right");
	nodes->push_back(pNodeHipRight);
	FbxNode* pNodeKneeRight = FbxNode::Create(pScene, "Knee Right");
	nodes->push_back(pNodeKneeRight);
	FbxNode* pNodeAnkleRight = FbxNode::Create(pScene, "Ankle Right");
	nodes->push_back(pNodeAnkleRight);
	FbxNode* pNodeFootRight = FbxNode::Create(pScene, "Foot Right");
	nodes->push_back(pNodeFootRight);
	FbxNode* pNodeHead = FbxNode::Create(pScene, "Head");
	nodes->push_back(pNodeHead);
}

void CreateScene(FbxManager *pSdkManager, FbxScene* pScene, std::vector<k4abt_skeleton_t> skeletons)
{
	//Create scene info
	FbxDocumentInfo* sceneInfo = FbxDocumentInfo::Create(pSdkManager, "SceneInfo");
	sceneInfo->mTitle = "Azure Kinect Skeleton Export";
	sceneInfo->mSubject = "Converts Azure Kinect skeleton to FBX animation.";
	sceneInfo->mAuthor = "Sam Lally";
	sceneInfo->mRevision = "v1.0";
	sceneInfo->mKeywords = "azure kinect choreographic machine learning";
	sceneInfo->mComment = "Created for Choreographic Machine Learning research at Virginia Tech.";

	pScene->SetSceneInfo(sceneInfo);

	//Create nodes 
	std::vector<FbxNode*> nodes;
	createNodes(pScene, &nodes);
	
	//Set up animation
	FbxAnimStack* myAnimStack = FbxAnimStack::Create(pScene, "Animation Stack");
	FbxAnimLayer* myAnimBaseLayer = FbxAnimLayer::Create(pScene, "Layer0");
	myAnimStack->AddMember(myAnimBaseLayer);
	FbxTime lTime;	
	int lKeyIndex = 0;
	FbxAnimCurve* xTranCurve = NULL;
	FbxAnimCurve* yTranCurve = NULL;
	FbxAnimCurve* zTranCurve = NULL;

	//Create animation
	for (int i = 0; i < nodes.size(); i++) { //Loop through 27 different skeleton nodes
		//Set up animation curves for each axis
		xTranCurve = nodes[i]->LclTranslation.GetCurve(myAnimBaseLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
		yTranCurve = nodes[i]->LclTranslation.GetCurve(myAnimBaseLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
		zTranCurve = nodes[i]->LclTranslation.GetCurve(myAnimBaseLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);
		
		//Start animation curves
		xTranCurve->KeyModifyBegin();
		yTranCurve->KeyModifyBegin();
		zTranCurve->KeyModifyBegin();

		for (int j = 0; j < skeletons.size(); j++) { //Loop through every frame from Kinect
			//Set time
			lTime.SetSecondDouble(j*(1.0 / 30));
			
			//Set x-axis position
			lKeyIndex = xTranCurve->KeyAdd(lTime);
			xTranCurve->KeySet(lKeyIndex, lTime,
				skeletons[j].joints[i].position.xyz.x, 
				FbxAnimCurveDef::eInterpolationLinear);
			//Set y-axis position
			lKeyIndex = yTranCurve->KeyAdd(lTime);
			yTranCurve->KeySet(lKeyIndex, lTime,
				skeletons[j].joints[i].position.xyz.y,
				FbxAnimCurveDef::eInterpolationLinear);
			//Set z-axis position
			lKeyIndex = zTranCurve->KeyAdd(lTime);
			zTranCurve->KeySet(lKeyIndex, lTime,
				skeletons[j].joints[i].position.xyz.z,
				FbxAnimCurveDef::eInterpolationLinear);
		}
		//End animation curves
		xTranCurve->KeyModifyEnd();
		yTranCurve->KeyModifyEnd();
		zTranCurve->KeyModifyEnd();
	}
}

bool createFBX(std::vector<k4abt_skeleton_t> skeletons, const char* output_path) {
	FbxManager* lSdkManager = NULL;
	FbxScene* lScene = NULL;
	bool lResult;

	//Prepare the FBX SDK.
	InitializeSdkObjects(lSdkManager, lScene);

	//Create the scene.
	CreateScene(lSdkManager, lScene, skeletons);

	//Save scene
	lResult = SaveScene(lSdkManager, lScene, output_path, -1, false);

	//Destroy all objects created by the FBX SDK
	DestroySdkObjects(lSdkManager, lResult);

	return lResult;
}