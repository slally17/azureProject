#pragma once

#include <GLTFSDK/GLTF.h>
#include <GLTFSDK/GLTFResourceWriter.h>
#include <GLTFSDK/GLBResourceWriter.h>
#include <GLTFSDK/Serialize.h>
#include <GLTFSDK/IStreamWriter.h>
#include <GLTFSDK/BufferBuilder.h>

#include <experimental/filesystem>
#include <fstream>

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

	void CreateTriangleResources(Document& document, BufferBuilder& bufferBuilder, std::string& accessorIdIndices, std::string& accessorIdPositions)
	{

		// Create all the resource data (e.g. triangle indices and
		// vertex positions) that will be written to the binary buffer
		const char* bufferId = nullptr;

		// Specify the 'special' GLB buffer ID. This informs the GLBResourceWriter that it should use
		// the GLB container's binary chunk (usually the desired buffer location when creating GLBs)
		if (dynamic_cast<const GLBResourceWriter*>(&bufferBuilder.GetResourceWriter()))
		{
			bufferId = GLB_BUFFER_ID;
		}

		// Create a Buffer - it will be the 'current' Buffer that all the BufferViews
		// created by this BufferBuilder will automatically reference
		bufferBuilder.AddBuffer(bufferId);

		// Create a BufferView with a target of ELEMENT_ARRAY_BUFFER (as it will reference index
		// data) - it will be the 'current' BufferView that all the Accessors created by this
		// BufferBuilder will automatically reference
		bufferBuilder.AddBufferView(BufferViewTarget::ELEMENT_ARRAY_BUFFER);
		
		// Add an Accessor for the indices
		std::vector<uint16_t> indices = {
			0U, 1U, 2U
		};

		// Copy the Accessor's id - subsequent calls to AddAccessor may invalidate the returned reference
		accessorIdIndices = bufferBuilder.AddAccessor(indices, { TYPE_SCALAR, COMPONENT_UNSIGNED_SHORT }).id; //THIS LINE BROKEN

		// Create a BufferView with target ARRAY_BUFFER (as it will reference vertex attribute data)
		bufferBuilder.AddBufferView(BufferViewTarget::ARRAY_BUFFER);

		// Add an Accessor for the positions
		std::vector<float> positions = {
			0.0f, 0.0f, 0.0f, // Vertex 0
			1.0f, 0.0f, 0.0f, // Vertex 1
			0.0f, 1.0f, 0.0f  // Vertex 2
		};

		std::vector<float> minValues(3U, std::numeric_limits<float>::max());
		std::vector<float> maxValues(3U, std::numeric_limits<float>::lowest());

		const size_t positionCount = positions.size();

		// Accessor min/max properties must be set for vertex position data so calculate them here
		for (size_t i = 0U, j = 0U; i < positionCount; ++i, j = (i % 3U))
		{
			minValues[j] = std::min(positions[i], minValues[j]);
			maxValues[j] = std::max(positions[i], maxValues[j]);
		}

		accessorIdPositions = bufferBuilder.AddAccessor(positions, { TYPE_VEC3, COMPONENT_FLOAT, false, std::move(minValues), std::move(maxValues) }).id;

		// Add all of the Buffers, BufferViews and Accessors that were created using BufferBuilder to
		// the Document. Note that after this point, no further calls should be made to BufferBuilder
		bufferBuilder.Output(document);
	}

	void CreateTriangleEntities(Document& document, const std::string& accessorIdIndices, const std::string& accessorIdPositions)
	{
		// Create a very simple glTF Document with the following hierarchy:
		//  Scene
		//     Node
		//       Mesh (Triangle)
		//         MeshPrimitive
		//           Material (Blue)
		// 
		// A Document can be constructed top-down or bottom up. However, if constructed top-down
		// then the IDs of child entities must be known in advance, which prevents using the glTF
		// SDK's automatic ID generation functionality.

		// Construct a Material
		Material material;
		material.metallicRoughness.baseColorFactor = Color4(0.0f, 0.0f, 1.0f, 1.0f);
		material.metallicRoughness.metallicFactor = 0.2f;
		material.metallicRoughness.roughnessFactor = 0.4f;
		material.doubleSided = true;

		// Add it to the Document and store the generated ID
		auto materialId = document.materials.Append(std::move(material), AppendIdPolicy::GenerateOnEmpty).id;

		// Construct a MeshPrimitive. Unlike most types in glTF, MeshPrimitives are direct children
		// of their parent Mesh entity rather than being children of the Document. This is why they
		// don't have an ID member.
		MeshPrimitive meshPrimitive;
		meshPrimitive.materialId = materialId;
		meshPrimitive.indicesAccessorId = accessorIdIndices;
		meshPrimitive.attributes[ACCESSOR_POSITION] = accessorIdPositions;

		// Construct a Mesh and add the MeshPrimitive as a child
		Mesh mesh;
		mesh.primitives.push_back(std::move(meshPrimitive));
		// Add it to the Document and store the generated ID
		auto meshId = document.meshes.Append(std::move(mesh), AppendIdPolicy::GenerateOnEmpty).id;

		// Construct a Node adding a reference to the Mesh
		Node node;
		node.meshId = meshId;
		// Add it to the Document and store the generated ID
		auto nodeId = document.nodes.Append(std::move(node), AppendIdPolicy::GenerateOnEmpty).id;

		// Construct a Scene
		Scene scene;
		scene.nodes.push_back(nodeId);
		// Add it to the Document, using a utility method that also sets the Scene as the Document's default
		document.SetDefaultScene(std::move(scene), AppendIdPolicy::GenerateOnEmpty);
	}



	bool createGLTF(std::vector<k4abt_skeleton_t> skeletons, const char* output_path) {
		bool result = true;

		//Convert output_path to absolute path
		std::experimental::filesystem::path path = output_path;
		if (path.is_relative())	{
			auto pathCurrent = std::experimental::filesystem::current_path();

			// Convert the relative path into an absolute path by appending the command line argument to the current path
			pathCurrent /= path;
			pathCurrent.swap(path);
		}

		std::cout << path << std::endl;

		//Create file writers
		auto streamWriter = std::make_unique<StreamWriter>(path.parent_path());
		std::experimental::filesystem::path pathFile = path.filename();
		std::unique_ptr<ResourceWriter> resourceWriter = std::make_unique<GLTFResourceWriter>(std::move(streamWriter));

		//Create gltf JSON manifest
		Document document;
		std::string accessorIdIndices;
		std::string accessorIdPositions;
		BufferBuilder bufferBuilder(std::move(resourceWriter));

		//Create gltf assets
		CreateTriangleResources(document, bufferBuilder, accessorIdIndices, accessorIdPositions);
		CreateTriangleEntities(document, accessorIdIndices, accessorIdPositions);

		// Serialize the glTF Document into a JSON manifest
		std::string manifest;
		try	{			
			manifest = Serialize(document, SerializeFlags::Pretty);
		}
		catch (const GLTFException& ex) {
			result = false;
		}


		//Write the JSON manifest to file
		auto& gltfResourceWriter = bufferBuilder.GetResourceWriter();
		gltfResourceWriter.WriteExternal(pathFile.u8string(), manifest.c_str(), manifest.length()); //THIS LINE BROKEN
		
		return result;
	}
}

