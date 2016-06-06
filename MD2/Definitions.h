// $Id: $
//
// Author: Francisco Sans franjaviersans@gmail.com
//
// Complete history on bottom of file

#ifndef Definitions_H
#define Definitions_H



#pragma comment (lib,"lib/assimp.lib")
#pragma comment (lib,"lib/glfw3.lib")
#pragma comment (lib,"./lib/glfw3dll.lib")
#pragma comment (lib,"lib/glew32.lib")
#pragma comment (lib,"lib/FreeImage.lib")
#pragma comment (lib, "opengl32.lib")


#include "include/glm/glm.hpp" // glm::vec3, glm::vec4, glm::ivec4, glm::mat4
#include "include/glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "include/glm/gtc/type_ptr.hpp" // glm::value_ptr
#include "include/glm/gtx/transform2.hpp" // for glm::lookAt
#include "include/GL/glew.h"
#include "include/GLFW/glfw3.h"
#include "include/assimp/Importer.hpp"	//OO version Header!
#include "include/assimp/Scene.h"		// Output data structure
#include "include/assimp/PostProcess.h"	// Post processing flag
#include "include/FreeImage/FreeImage.h"

#define MAX_NUM_LIGHTS 5
#define MAX_RAY 128


#define BUFFER_OFFSET(i) (reinterpret_cast<void*>(i))
#define WORLD_COORD_LOCATION 0
#define NORMAL_COORD_LOCATION 1
#define TEXTURE_COORD_LOCATION 2



#endif //Definitions_H