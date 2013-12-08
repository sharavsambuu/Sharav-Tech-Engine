#include "MaterialManager.hpp"
#include <iostream>

MaterialManager::MaterialManager() {
}

MaterialManager::~MaterialManager() {
    std::map<int, Material *>::iterator begin = this->materials.begin();
    std::map<int, Material *>::iterator end = this->materials.end();
    while (begin != end) {
        delete begin->second;
        ++begin;
    }
    this->materials.clear();
    std::cout << "MaterialManager destructor function" << std::endl;
}

Material* MaterialManager::createMaterial(const std::string& name, int index) {
    if (this->materials.find(index) == this->materials.end() &&
            this->materialIndexes.find(name) == this->materialIndexes.end()) {
        this->materials[index] = new Material();
        this->materialIndexes[name] = index;
    }
    return this->materials[index];
}

Material* MaterialManager::getMaterial(int index) {
    if (this->materials.find(index) == this->materials.end())
        return NULL;
    return this->materials[index];
}

Material* MaterialManager::getMaterial(const std::string& name) {
    if (this->materialIndexes.find(name) == this->materialIndexes.end())
        return NULL;
    return this->materials[getMaterialIndex(name)];
}

int MaterialManager::getMaterialIndex(const std::string& name) {
    if (this->materialIndexes.find(name) == this->materialIndexes.end())
        return -1;
    else
        return this->materialIndexes[name];
}