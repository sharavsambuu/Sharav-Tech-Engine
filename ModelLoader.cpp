#include "ModelLoader.hpp"

void processMesh(aiMesh* mesh, SceneObject *object) {
    float *normals, *textureCoordinates, *tangents, *bitangents;
    float *vertices = (float *) malloc(4 * mesh->mNumVertices * sizeof (float));
    if (mesh->HasNormals())
        normals = (float *) malloc(3 * mesh->mNumVertices * sizeof (float));
    if (mesh->HasTextureCoords(0))
        textureCoordinates = (float *) malloc(2 * mesh->mNumVertices * sizeof (float));
    if (mesh->HasTangentsAndBitangents()) {
        tangents = (float *) malloc(3 * mesh->mNumVertices * sizeof (float));
        bitangents = (float *) malloc(3 * mesh->mNumVertices * sizeof (float));
    }

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        vertices[4 * i + 0] = mesh->mVertices[i].x;
        vertices[4 * i + 1] = mesh->mVertices[i].y;
        vertices[4 * i + 2] = mesh->mVertices[i].z;
        vertices[4 * i + 3] = 1.0f;
        if (mesh->HasNormals()) {
            normals[3 * i + 0] = mesh->mNormals[i].x;
            normals[3 * i + 1] = mesh->mNormals[i].y;
            normals[3 * i + 2] = mesh->mNormals[i].z;
        }
        if (mesh->HasTextureCoords(0)) {
            textureCoordinates[2 * i + 0] = mesh->mTextureCoords[0][i].x;
            textureCoordinates[2 * i + 1] = mesh->mTextureCoords[0][i].y;
        }
        if (mesh->HasTangentsAndBitangents()) {
            tangents[3 * i + 0] = mesh->mTangents[i].x;
            tangents[3 * i + 1] = mesh->mTangents[i].y;
            tangents[3 * i + 2] = mesh->mTangents[i].z;
            bitangents[3 * i + 0] = mesh->mBitangents[i].x;
            bitangents[3 * i + 1] = mesh->mBitangents[i].y;
            bitangents[3 * i + 2] = mesh->mBitangents[i].z;
        }
    }
    int *faces = (int *) malloc(3 * mesh->mNumFaces * sizeof (int));
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        faces[3 * i + 0] = mesh->mFaces[i].mIndices[0];
        faces[3 * i + 1] = mesh->mFaces[i].mIndices[1];
        faces[3 * i + 2] = mesh->mFaces[i].mIndices[2];
    }
    object->addMesh(
            vertices, mesh->mNumVertices,
            normals, mesh->mNumVertices,
            textureCoordinates, mesh->mNumVertices,
            tangents, mesh->mNumVertices,
            bitangents, mesh->mNumVertices,
            faces, mesh->mNumFaces,
            mesh->mMaterialIndex);
}

void processMaterial(aiMaterial* material, int index, SceneObject *object) {
    MaterialManager *mm = object->mm;
    std::cout << "================ ID " << index << "====================" << std::endl;

    aiString materialName;

    // Get the name for managing purpose
    if (material->Get(AI_MATKEY_NAME, materialName) != AI_SUCCESS) {
        std::cout << "ERROR: while getting name of material" << std::endl;
    }

    aiString diffuseTexturePath;
    aiString normalMapTexturePath;
    aiString specularTexturePath;
    aiString ambientTexturePath;
    aiString opacityTexturePath;
    aiColor3D diffuseColor, ambientColor, specularColor, emissiveColor;
    float opacity, shininess;

    // Get diffuse color
    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) != AI_SUCCESS) {
        std::cout << "ERROR: Could not get diffuse color for Material " << materialName.data << std::endl;
    }

    // Get ambient color
    if (material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor) != AI_SUCCESS) {
        std::cout << "ERROR: Could not get ambient color for Material " << materialName.data << std::endl;
    }

    // Get diffuse color
    if (material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor) != AI_SUCCESS) {
        std::cout << "ERROR: Could not get specular color for Material " << materialName.data << std::endl;
    }

    // Get opacity
    if (material->Get(AI_MATKEY_OPACITY, opacity) != AI_SUCCESS) {
        std::cout << "ERROR: Could not get opacity for Material " << materialName.data << std::endl;
    }

    // Get emissive color
    if (material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor) != AI_SUCCESS)
        emissiveColor = aiColor3D(0);

    // Get shininess
    if (material->Get(AI_MATKEY_SHININESS, shininess) != AI_SUCCESS) {
        std::cout << "ERROR: Could not get Shininess for Material " << materialName.data << std::endl;
    }


    // material color parameters
    std::cout << "diffuse color [" << diffuseColor.r << "," << diffuseColor.g << "," << diffuseColor.b << "] opacity=" << opacity << std::endl;
    std::cout << "ambient color [" << ambientColor.r << "," << ambientColor.g << "," << ambientColor.b << "]" << std::endl;
    std::cout << "specular color [" << specularColor.r << "," << specularColor.g << "," << specularColor.b << "]" << std::endl;
    std::cout << "emissive color [" << emissiveColor.r << "," << emissiveColor.g << "," << emissiveColor.b << "]" << std::endl;


    Material *m = mm->createMaterial(std::string(materialName.data), index);
    m->setDiffuseColor(glm::vec4(diffuseColor.r, diffuseColor.g, diffuseColor.b, opacity));
    m->setAmbientColor(glm::vec3(ambientColor.r, ambientColor.g, ambientColor.b));
    m->setSpecularColor(glm::vec3(specularColor.r, specularColor.g, specularColor.b));
    m->setEmissiveColor(glm::vec3(emissiveColor.r, emissiveColor.g, emissiveColor.b));

    // diffuse map
    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
        if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), diffuseTexturePath) != AI_SUCCESS) {
            std::cout << "ERROR: Could not get Diffuse Texture for Material " << materialName.data << std::endl;
            return;
        }
        m->hasDiffuseTexture = true;
        m->loadDiffuseTexture(diffuseTexturePath.data);
        std::cout << "diffuse texture path " << diffuseTexturePath.data << std::endl;
    }

    // normal map
    if (material->GetTextureCount(aiTextureType_HEIGHT) > 0) {
        if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_HEIGHT, 0), normalMapTexturePath) != AI_SUCCESS) {
            std::cout << "ERROR: Could not get Height Map Texture for Material " << materialName.data << std::endl;
            return;
        }
        m->hasNormalTexture = true;
        m->loadNormalTexture(normalMapTexturePath.data);
        std::cout << "height map texture path " << normalMapTexturePath.data << std::endl;
    }

    // specular map
    if (material->GetTextureCount(aiTextureType_SPECULAR) > 0) {
        if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), specularTexturePath) != AI_SUCCESS) {
            std::cout << "ERROR: Could not get Mask Texture for Material " << materialName.data << std::endl;
            return;
        }
        m->hasSpecularTexture = true;
        m->loadSpecularTexture(specularTexturePath.data);
        std::cout << "specular texture path " << specularTexturePath.data << std::endl;
    } else {
        std::cout << "there is no specular texture at ID=" << index << std::endl;
    }

    // ambient map
    if (material->GetTextureCount(aiTextureType_AMBIENT) > 0) {
        if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_AMBIENT, 0), ambientTexturePath) != AI_SUCCESS) {
            std::cout << "ERROR: Could not get Ambient Texture for Material " << materialName.data << std::endl;
            return;
        }
        m->hasAmbientTexture = true;
        m->loadAmbientTexture(ambientTexturePath.data);
        std::cout << "ambient texture path " << ambientTexturePath.data << std::endl;
    } else {
        std::cout << ":( THERE IS NO AMBIENT MAP" << std::endl;
    }
    // opacity map
    if (material->GetTextureCount(aiTextureType_OPACITY) > 0) {
        if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_OPACITY, 0), opacityTexturePath) != AI_SUCCESS) {
            std::cout << "ERROR: Couldn't get opacity texture for material" << materialName.data << std::endl;
            return;
        }
        m->hasOpacityTexture = true;
        m->loadOpacityTexture(opacityTexturePath.data);
        std::cout << "opacity texture path" << opacityTexturePath.data << std::endl;
    } else {
        std::cout << ":( THERE IS NO OPACITY MAP" << std::endl;
    }
    std::cout << "==========================================" << std::endl;
}

