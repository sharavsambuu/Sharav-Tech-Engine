/* 
 * File:   MaterialManager.hpp
 * Author: prompto
 *
 * Created on November 30, 2013, 9:03 PM
 */

#ifndef MATERIALMANAGER_HPP
#define	MATERIALMANAGER_HPP

#include "Material.hpp"
#include <map>
#include <string>

class MaterialManager {
public:
    MaterialManager();
    ~MaterialManager();
    
    Material* createMaterial(const std::string &name, int index);
    Material* getMaterial(int index);
    Material* getMaterial(const std::string &name);
    int getMaterialIndex(const std::string &name);
protected:
    std::map<int, Material*> materials;
    std::map<std::string, int> materialIndexes;
};

#endif	/* MATERIALMANAGER_HPP */

