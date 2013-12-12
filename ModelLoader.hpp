/* 
 * File:   ModelLoader.hpp
 * Author: prompto
 *
 * Created on December 13, 2013, 3:59 AM
 */

#ifndef MODELLOADER_HPP
#define	MODELLOADER_HPP

#include "SceneObject.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <string>

void processMaterial(aiMaterial* material, int index, SceneObject *object);
void processMesh(aiMesh* mesh, SceneObject *object);
SceneObject* loadSceneModel(const std::string& filePath);

#endif	/* MODELLOADER_HPP */

