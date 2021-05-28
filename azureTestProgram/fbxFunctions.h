#pragma once

#include <fbxsdk.h>

#include <vector>
#include <iostream>
#include <string>
#include <experimental/filesystem>

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
	//if (pExitStatus) FBXSDK_printf("Program Success!\n");
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
	FbxNode* lRootNode = pScene->GetRootNode();
	FbxNode* pSkeleton = FbxNode::Create(pScene, "Skeleton");
	lRootNode->AddChild(pSkeleton);

	FbxNode* pNodePelvis = FbxNode::Create(pScene, "Pelvis");
	pSkeleton->AddChild(pNodePelvis);
	nodes->push_back(pNodePelvis);
	FbxNode* pNodeSpineNaval = FbxNode::Create(pScene, "Spine_Naval");
	pNodePelvis->AddChild(pNodeSpineNaval);
	nodes->push_back(pNodeSpineNaval);
	FbxNode* pNodeSpineChest = FbxNode::Create(pScene, "Spine_Chest");
	pNodeSpineNaval->AddChild(pNodeSpineChest);
	nodes->push_back(pNodeSpineChest);
	FbxNode* pNodeNeck = FbxNode::Create(pScene, "Neck");
	pNodeSpineChest->AddChild(pNodeNeck);
	nodes->push_back(pNodeNeck);
	FbxNode* pNodeClavicleLeft = FbxNode::Create(pScene, "Clavicle_Left");
	pNodeSpineChest->AddChild(pNodeClavicleLeft);
	nodes->push_back(pNodeClavicleLeft);
	FbxNode* pNodeShoulderLeft = FbxNode::Create(pScene, "Shoulder_Left");
	pNodeClavicleLeft->AddChild(pNodeShoulderLeft);
	nodes->push_back(pNodeShoulderLeft);
	FbxNode* pNodeElbowLeft = FbxNode::Create(pScene, "Elbow_Left");
	pNodeShoulderLeft->AddChild(pNodeElbowLeft);
	nodes->push_back(pNodeElbowLeft);
	FbxNode* pNodeWristLeft = FbxNode::Create(pScene, "Wrist_Left");
	pNodeElbowLeft->AddChild(pNodeWristLeft);
	nodes->push_back(pNodeWristLeft);
	FbxNode* pNodeHandLeft = FbxNode::Create(pScene, "Hand_Left");
	pNodeWristLeft->AddChild(pNodeHandLeft);
	nodes->push_back(pNodeHandLeft);
	FbxNode* pNodeHandtipLeft = FbxNode::Create(pScene, "Handtip_Left");
	pNodeHandLeft->AddChild(pNodeHandtipLeft);
	nodes->push_back(pNodeHandtipLeft);
	FbxNode* pNodeThumbLeft = FbxNode::Create(pScene, "Thumb_Left");
	pNodeHandtipLeft->AddChild(pNodeThumbLeft);
	nodes->push_back(pNodeThumbLeft);
	FbxNode* pNodeClavicleRight = FbxNode::Create(pScene, "Clavical_Right");
	pNodeSpineChest->AddChild(pNodeClavicleRight);
	nodes->push_back(pNodeClavicleRight);
	FbxNode* pNodeShoulderRight = FbxNode::Create(pScene, "Shoulder_Right");
	pNodeClavicleRight->AddChild(pNodeShoulderRight);
	nodes->push_back(pNodeShoulderRight);
	FbxNode* pNodeElbowRight = FbxNode::Create(pScene, "Elbow_Right");
	pNodeShoulderRight->AddChild(pNodeElbowRight);
	nodes->push_back(pNodeElbowRight);
	FbxNode* pNodeWristRight = FbxNode::Create(pScene, "Wrist_Right");
	pNodeElbowRight->AddChild(pNodeWristRight);
	nodes->push_back(pNodeWristRight);
	FbxNode* pNodeHandRight = FbxNode::Create(pScene, "Hand_Right");
	pNodeWristRight->AddChild(pNodeHandRight);
	nodes->push_back(pNodeHandRight);
	FbxNode* pNodeHandtipRight = FbxNode::Create(pScene, "Handtip_Right");
	pNodeHandRight->AddChild(pNodeHandtipRight);
	nodes->push_back(pNodeHandtipRight);
	FbxNode* pNodeThumbRight = FbxNode::Create(pScene, "Thumb_Right");
	pNodeHandtipRight->AddChild(pNodeThumbRight);
	nodes->push_back(pNodeThumbRight);
	FbxNode* pNodeHipLeft = FbxNode::Create(pScene, "Hip_Left");
	pNodePelvis->AddChild(pNodeHipLeft);
	nodes->push_back(pNodeHipLeft);
	FbxNode* pNodeKneeLeft = FbxNode::Create(pScene, "Knee_Left");
	pNodeHipLeft->AddChild(pNodeKneeLeft);
	nodes->push_back(pNodeKneeLeft);
	FbxNode* pNodeAnkleLeft = FbxNode::Create(pScene, "Ankle_Left");
	pNodeKneeLeft->AddChild(pNodeAnkleLeft);
	nodes->push_back(pNodeAnkleLeft);
	FbxNode* pNodeFootLeft = FbxNode::Create(pScene, "Foot_Left");
	pNodeAnkleLeft->AddChild(pNodeFootLeft);
	nodes->push_back(pNodeFootLeft);
	FbxNode* pNodeHipRight = FbxNode::Create(pScene, "Hip_Right");
	pNodePelvis->AddChild(pNodeHipRight);
	nodes->push_back(pNodeHipRight);
	FbxNode* pNodeKneeRight = FbxNode::Create(pScene, "Knee_Right");
	pNodeHipRight->AddChild(pNodeKneeRight);
	nodes->push_back(pNodeKneeRight);
	FbxNode* pNodeAnkleRight = FbxNode::Create(pScene, "Ankle_Right");
	pNodeKneeRight->AddChild(pNodeAnkleRight);
	nodes->push_back(pNodeAnkleRight);
	FbxNode* pNodeFootRight = FbxNode::Create(pScene, "Foot_Right");
	pNodeAnkleRight->AddChild(pNodeFootRight);
	nodes->push_back(pNodeFootRight);
	FbxNode* pNodeHead = FbxNode::Create(pScene, "Head");
	pNodeNeck->AddChild(pNodeHead);
	nodes->push_back(pNodeHead);
}

void createMesh(FbxScene* pScene) {
	// Create a node for our mesh in the scene.
	FbxNode* lMeshNode = FbxNode::Create(pScene, "meshNode");

	// Create a mesh.
	FbxMesh* lMesh = FbxMesh::Create(pScene, "mesh");

	// Set the node attribute of the mesh node.
	lMeshNode->SetNodeAttribute(lMesh);

	// Add the mesh node to the root node in the scene.
	FbxNode *lRootNode = pScene->GetRootNode();
	lRootNode->AddChild(lMeshNode);

	//Define vertices of plane
	FbxVector4 vertex0(-50, 0, 50);
	FbxVector4 vertex1(50, 0, 50);
	FbxVector4 vertex2(50, 0, -50);
	FbxVector4 vertex3(-50, 0, -50);
	FbxVector4 lNormalZPos(0, 0, 1);

	//Initialize the control point array of the mesh.
	lMesh->InitControlPoints(4);
	FbxVector4* lControlPoints = lMesh->GetControlPoints();

	//Define control points of plane.
	lControlPoints[0] = vertex0;
	lControlPoints[1] = vertex1;
	lControlPoints[2] = vertex2;
	lControlPoints[3] = vertex3;

	//Create layer 0
	FbxLayer* lLayer = lMesh->GetLayer(0);
	if (lLayer == NULL) {
		lMesh->CreateLayer();
		lLayer = lMesh->GetLayer(0);
	}

	// Create a normal layer.
	FbxLayerElementNormal* lLayerElementNormal = FbxLayerElementNormal::Create(lMesh, "");

	// Set its mapping mode to map each normal vector to each control point.
	lLayerElementNormal->SetMappingMode(FbxLayerElement::eByControlPoint);

	// Set the reference mode of so that the n'th element of the normal array maps to the n'th
	// element of the control point array.
	lLayerElementNormal->SetReferenceMode(FbxLayerElement::eDirect);

	// Assign the normal vectors in the same order the control points were defined for the mesh.
	lLayerElementNormal->GetDirectArray().Add(lNormalZPos);
	lLayerElementNormal->GetDirectArray().Add(lNormalZPos);
	lLayerElementNormal->GetDirectArray().Add(lNormalZPos);
	lLayerElementNormal->GetDirectArray().Add(lNormalZPos);

	// Finally, we set layer 0 of the mesh to the normal layer element.
	lLayer->SetNormals(lLayerElementNormal);

	//Create material
	FbxSurfaceLambert* lMaterial = FbxSurfaceLambert::Create(pScene, "customMaterial");

	//Add material to mesh node
	lMeshNode->AddMaterial(lMaterial);

	// Set material mapping.
	FbxGeometryElementMaterial* lMaterialElement = lMesh->CreateElementMaterial();
	lMaterialElement->SetMappingMode(FbxGeometryElement::eByPolygon);
	lMaterialElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);

	//Create polygons
	lMesh->BeginPolygon(0); // Material index.
	lMesh->AddPolygon(0);
	lMesh->AddPolygon(3);
	lMesh->AddPolygon(2);
	lMesh->AddPolygon(1);
	lMesh->EndPolygon();
}

void CreateScene(FbxManager *pSdkManager, FbxScene* pScene, std::vector<k4abt_skeleton_t> skeletons, std::string fileName)
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
	FbxAnimStack* myAnimStack = FbxAnimStack::Create(pScene, fileName.c_str());
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
			
			float position, offset;
			//Set x-axis position
			position = skeletons[j].joints[i].position.xyz.x;
			if(nodes[i]->GetParent()) {
				offset = nodes[i]->GetParent()->EvaluateGlobalTransform(lTime).GetT()[0];
				position -= offset;
			}
			lKeyIndex = xTranCurve->KeyAdd(lTime);
			xTranCurve->KeySet(lKeyIndex, lTime,
				position, 
				FbxAnimCurveDef::eInterpolationLinear);

			//Set y-axis position
			position = -1 * skeletons[j].joints[i].position.xyz.y;
			if (nodes[i]->GetParent()) {
				offset = nodes[i]->GetParent()->EvaluateGlobalTransform(lTime).GetT()[1];
				position -= offset;
				//position *= -1; //Flip the skeleton vertically
			}			
			lKeyIndex = yTranCurve->KeyAdd(lTime);
			yTranCurve->KeySet(lKeyIndex, lTime,
				position,
				FbxAnimCurveDef::eInterpolationLinear);

			//Set z-axis position
			position = skeletons[j].joints[i].position.xyz.z;
			if (nodes[i]->GetParent()) {
				offset = nodes[i]->GetParent()->EvaluateGlobalTransform(lTime).GetT()[2];
				position -= offset;
			}
			lKeyIndex = zTranCurve->KeyAdd(lTime);
			zTranCurve->KeySet(lKeyIndex, lTime,
				position,
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

	//Get file name as string
	std::experimental::filesystem::path path = output_path;
	std::string fileName = path.stem().string();

	//Prepare the FBX SDK.
	InitializeSdkObjects(lSdkManager, lScene);

	//Create the scene.
	CreateScene(lSdkManager, lScene, skeletons, fileName);

	//Add a mesh to scene.
	createMesh(lScene);

	//Save scene
	lResult = SaveScene(lSdkManager, lScene, output_path, -1, false);

	//Destroy all objects created by the FBX SDK
	DestroySdkObjects(lSdkManager, lResult);

	return lResult;
}