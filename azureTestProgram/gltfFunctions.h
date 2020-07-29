#pragma once

#include <GLTFSDK/GLTF.h>
#include <GLTFSDK/GLTFResourceWriter.h>
#include <GLTFSDK/GLBResourceWriter.h>
#include <GLTFSDK/Serialize.h>
#include <GLTFSDK/IStreamWriter.h>
#include <GLTFSDK/BufferBuilder.h>

#include <experimental/filesystem>
#include <fstream>
#include <sstream>
#include <vector>

using namespace Microsoft::glTF;

namespace {

	class StreamWriter : public IStreamWriter
	{
	public:
		StreamWriter(std::experimental::filesystem::path pathBase) : m_pathBase(std::move(pathBase))
		{
			assert(m_pathBase.has_root_path());
		}

		// Resolves the relative URIs of any external resources declared in the glTF manifest
		std::shared_ptr<std::ostream> GetOutputStream(const std::string& filename) const override
		{
			// In order to construct a valid stream:
			// 1. The filename argument will be encoded as UTF-8 so use filesystem::u8path to
			//    correctly construct a path instance.
			// 2. Generate an absolute path by concatenating m_pathBase with the specified filename
			//    path. The filesystem::operator/ uses the platform's preferred directory separator
			//    if appropriate.
			// 3. Always open the file stream in binary mode. The glTF SDK will handle any text
			//    encoding issues for us.
			auto streamPath = m_pathBase / std::experimental::filesystem::u8path(filename);
			auto stream = std::make_shared<std::ofstream>(streamPath, std::ios_base::binary);

			 //Check if the stream has no errors and is ready for I/O operations
			if (!stream || !(*stream))
			{
				throw std::runtime_error("Unable to create a valid output stream for uri: " + filename);
			}			

			return stream;
		}

	private:
		std::experimental::filesystem::path m_pathBase;
	};

	void addPositionData(std::vector<k4abt_skeleton_t> skeletons, std::vector<float> positions[27]) {
		for (int i = 0; i < 27; i++) {	
			for (int j = 0; j < skeletons.size(); j++) {
				positions[i].push_back(skeletons[j].joints[i].position.xyz.x);
				positions[i].push_back(skeletons[j].joints[i].position.xyz.y);
				positions[i].push_back(skeletons[j].joints[i].position.xyz.z);
			}
		}
	}

	void CreateSkeletonResources(std::vector<k4abt_skeleton_t> skeletons, std::string fileName, Document& document, BufferBuilder& bufferBuilder, std::string& accessorIdTime, std::string accessorIdPositions[27]) {
		//Create buffer to store all resource data
		const char* bufferId = fileName.c_str();
		bufferBuilder.AddBuffer(bufferId);

		//Create buffer view for keyframe times
		bufferBuilder.AddBufferView(BufferViewTarget::ELEMENT_ARRAY_BUFFER);
		std::vector<float> times; //Create times based off input being 30fps
		for (int i = 0; i < skeletons.size(); i++) {
			times.push_back(i / 30.0f);
		}
		accessorIdTime = bufferBuilder.AddAccessor(times, { TYPE_SCALAR, COMPONENT_FLOAT }).id;

		//Create buffer views for animation node data
		bufferBuilder.AddBufferView(BufferViewTarget::ARRAY_BUFFER);
		std::vector<float> positions[27];
		addPositionData(skeletons, positions);
		std::vector<float> minValues[27]; 
		std::vector<float> maxValues[27]; 
		for (int i = 0; i < 27; i++) {
			minValues[i] = std::vector<float>(3U, std::numeric_limits<float>::max());
			maxValues[i] = std::vector<float>(3U, std::numeric_limits<float>::lowest());
		}
		const size_t positionCount = positions[0].size();
		for (int h = 0; h < 27; h++) {
			for (size_t i = 0U, j = 0U; i < positionCount; ++i, j = (i % 3U)) {
				minValues[h][j] = std::min(positions[h][i], minValues[h][j]);
				maxValues[h][j] = std::max(positions[h][i], maxValues[h][j]);
			}
		}		
		for (int i = 0; i < 27; i++) {
			accessorIdPositions[i] = bufferBuilder.AddAccessor(positions[i], { TYPE_VEC3, COMPONENT_FLOAT, false, std::move(minValues[i]), std::move(maxValues[i]) }).id;
		}

		//Add everything created above into the document
		bufferBuilder.Output(document);
	}

	void CreateSkeletonEntities(Document& document, const std::string& accessorIdTime, const std::string accessorIdPositions[27]) {	
		Node skeleton[27];
		std::string skeletonId[27];
		for (int i = 0; i < 27; i++) {
			skeletonId[i] = document.nodes.Append(std::move(skeleton[i]), AppendIdPolicy::GenerateOnEmpty).id;
		}		

		AnimationSampler skeletonAnimationSampler[27];
		AnimationTarget skeletonAnimationTarget[27];
		AnimationChannel skeletonAnimationChannel[27];
		Animation skeletonAnimation[27];
		std::string skeletonAnimationSamplerId[27];
		for (int i = 0; i < 27; i++) {
			skeletonAnimationSampler[i].inputAccessorId = accessorIdTime;
			skeletonAnimationSampler[i].outputAccessorId = accessorIdPositions[i];
			skeletonAnimationSamplerId[i] = skeletonAnimation[i].samplers.Append(std::move(skeletonAnimationSampler[i]), AppendIdPolicy::GenerateOnEmpty).id;
			skeletonAnimationTarget[i].nodeId = skeletonId[i];
			skeletonAnimationTarget[i].path = TARGET_TRANSLATION;
			skeletonAnimationChannel[i].samplerId = skeletonAnimationSamplerId[i];
			skeletonAnimationChannel[i].target = skeletonAnimationTarget[i];			
			skeletonAnimation[i].channels.Append(std::move(skeletonAnimationChannel[i]), AppendIdPolicy::GenerateOnEmpty);
			document.animations.Append(std::move(skeletonAnimation[i]), AppendIdPolicy::GenerateOnEmpty); 
		}

		Scene scene;
		for (int i = 0; i < 27; i++) {
			scene.nodes.push_back(skeletonId[i]);
		}
		document.SetDefaultScene(std::move(scene), AppendIdPolicy::GenerateOnEmpty);
	}	

	bool createGLTF(std::vector<k4abt_skeleton_t> skeletons, const char* output_path) {
		bool result = true;

		//Convert output_path to absolute path
		std::experimental::filesystem::path path = output_path;
		std::string fileName = path.stem().string();
		if (path.is_relative())	{
			auto pathCurrent = std::experimental::filesystem::current_path();

			// Convert the relative path into an absolute path by appending the command line argument to the current path
			pathCurrent /= path;
			pathCurrent.swap(path);
		}

		//Create file writers
		auto streamWriter = std::make_unique<StreamWriter>(path.parent_path());
		std::experimental::filesystem::path pathFile = path.filename();
		std::unique_ptr<ResourceWriter> resourceWriter = std::make_unique<GLTFResourceWriter>(std::move(streamWriter));

		//Create gltf JSON manifest
		Document document;
		std::string accessorIdTime;
		std::string accessorIdPositions[27];
		BufferBuilder bufferBuilder(std::move(resourceWriter));

		//Create gltf assets
		CreateSkeletonResources(skeletons, fileName, document, bufferBuilder, accessorIdTime, accessorIdPositions);
		CreateSkeletonEntities(document, accessorIdTime, accessorIdPositions);

		// Serialize the glTF Document into a JSON manifest
		std::string manifest;
		try	{			
			manifest = Serialize(document, SerializeFlags::Pretty);
		}
		catch (const GLTFException& ex) {
			std::cout << ex.what() << std::endl;
			result = false;
		}

		//Write the JSON manifest to file
		auto& gltfResourceWriter = bufferBuilder.GetResourceWriter();
		gltfResourceWriter.WriteExternal(pathFile.u8string(), manifest.c_str(), manifest.length());
		
		if (result) {
			std::cout << "Program Success!\n";
		}

		return result;
	}
}

