#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=MinGW-Windows
CND_DLIB_EXT=dll
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Material.o \
	${OBJECTDIR}/MaterialManager.o \
	${OBJECTDIR}/SceneObject.o \
	${OBJECTDIR}/Shader.o \
	${OBJECTDIR}/ShaderManager.o \
	${OBJECTDIR}/ShaderProgram.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/C/lib/assimp/lib -L/C/lib/glfw-3.0.3.bin.WIN32/lib-mingw -L/C/lib/glew-1.10.0-src/lib -L/C/lib/Simple\ OpenGL\ Image\ Library/lib

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sharavtechengine.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sharavtechengine.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sharavtechengine ${OBJECTFILES} ${LDLIBSOPTIONS} -lSOIL -lassimp.dll -lglew32 -lglu32 -lglfw3 -lopengl32 -lglu32 -lgdi32 -luser32 -lkernel32

${OBJECTDIR}/Material.o: Material.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/C/lib/assimp/include -I/C/lib/glfw-3.0.3.bin.WIN32/include -I/C/lib/glew-1.10.0-src/include -I/C/lib/glm -I/C/lib/Simple\ OpenGL\ Image\ Library/src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Material.o Material.cpp

${OBJECTDIR}/MaterialManager.o: MaterialManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/C/lib/assimp/include -I/C/lib/glfw-3.0.3.bin.WIN32/include -I/C/lib/glew-1.10.0-src/include -I/C/lib/glm -I/C/lib/Simple\ OpenGL\ Image\ Library/src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MaterialManager.o MaterialManager.cpp

${OBJECTDIR}/SceneObject.o: SceneObject.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/C/lib/assimp/include -I/C/lib/glfw-3.0.3.bin.WIN32/include -I/C/lib/glew-1.10.0-src/include -I/C/lib/glm -I/C/lib/Simple\ OpenGL\ Image\ Library/src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SceneObject.o SceneObject.cpp

${OBJECTDIR}/Shader.o: Shader.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/C/lib/assimp/include -I/C/lib/glfw-3.0.3.bin.WIN32/include -I/C/lib/glew-1.10.0-src/include -I/C/lib/glm -I/C/lib/Simple\ OpenGL\ Image\ Library/src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Shader.o Shader.cpp

${OBJECTDIR}/ShaderManager.o: ShaderManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/C/lib/assimp/include -I/C/lib/glfw-3.0.3.bin.WIN32/include -I/C/lib/glew-1.10.0-src/include -I/C/lib/glm -I/C/lib/Simple\ OpenGL\ Image\ Library/src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ShaderManager.o ShaderManager.cpp

${OBJECTDIR}/ShaderProgram.o: ShaderProgram.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/C/lib/assimp/include -I/C/lib/glfw-3.0.3.bin.WIN32/include -I/C/lib/glew-1.10.0-src/include -I/C/lib/glm -I/C/lib/Simple\ OpenGL\ Image\ Library/src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ShaderProgram.o ShaderProgram.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/C/lib/assimp/include -I/C/lib/glfw-3.0.3.bin.WIN32/include -I/C/lib/glew-1.10.0-src/include -I/C/lib/glm -I/C/lib/Simple\ OpenGL\ Image\ Library/src -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sharavtechengine.exe

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
