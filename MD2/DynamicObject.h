// $Id: $
//
// Author: Francisco Sans franjaviersans@gmail.com
//
// Complete history on bottom of file

#ifndef DynamicObject_H
#define DynamicObject_H



//Includes

#include "Definitions.h"
#include "Vertex.h"
#include "TextureManager.h"
#include "BoundingBox.h"
#include <vector>

/* Vector */
typedef float vec3_t[3];

/* MD2 header */
class md2_header_t
{
	public:
		int ident;
		int version;

		int skinwidth;
		int skinheight;

		int framesize;

		int num_skins;
		int num_vertices;
		int num_st;
		int num_tris;
		int num_glcmds;
		int num_frames;

		int offset_skins;
		int offset_st;
		int offset_tris;
		int offset_frames;
		int offset_glcmds;
		int offset_end;
};

/* Texture name */
class md2_skin_t
{
	public:
		char name[64];
};

/* Texture coords */
class md2_texCoord_t
{
	public:
		short s;
		short t;
};

/* Triangle info */
class md2_triangle_t
{
	public:
		unsigned short vertex[3];
		unsigned short st[3];
};

/* Compressed vertex */
class md2_vertex_t
{
	public:
		unsigned char v[3];
		unsigned char normalIndex;
};

/* Model frame */
class md2_frame_t
{
	public:
	  vec3_t scale;
	  vec3_t translate;
	  char name[16];
	  std::vector<md2_vertex_t> verts;
};

/* GL command packet */
class md2_glcmd_t
{
	public:
	  float s;
	  float t;
	  int index;
};

/* MD2 model structure */
class md2_model_t
{
	public:
		md2_header_t header;
		std::vector<md2_skin_t> skins;
		std::vector<md2_texCoord_t> texcoords;
		std::vector<md2_triangle_t> triangles;
		std::vector<md2_frame_t> frames;
		std::vector<int> glcmds;
		GLuint tex_id;
};


/**
* Class DynamicObject.
* A Class to track the GPU time from different part of the program
* Also includes a log to print all the times
*
*/
class DynamicObject
{
	//Functions

	public:
		///Default constructor
		DynamicObject(GLfloat velocity, GLuint Width, GLuint Height, GLint start, GLint end);

		///Default destructor
		~DynamicObject();

		///Load an MD2 model from file.
		void Import (const char *filename, const glm::mat4 &TransformationMatrix);

		///Calculate the current frame in animation
		void Animate (float time);

		///Render MD2 using Vertex Buffer Object
		void Draw();

		///Update Vertex Buffer Object
		void UpdateVAO();		

		///Get Texture Width
		GLuint GetTextureWidth();

		///Get Texture Height
		GLuint GetTextureHeight();

		///Method to get Bounding Box
		BoundingBox GetBoundingBox();

		//Method to set velocity of the animation
		void SetVelocity(float vel);

	private:
		///Method to Load texture
		void LoadTexture(md2_model_t * m_Object);
		
		///Initialize Vertex Buffer Object
		void InitVAO();

		///Transform the model
		void Transform(md2_model_t * m_Object, const glm::mat4 &TransformationMatrix);

	//Variables

	public:
		GLfloat m_fInterpolation, m_fAnimationVelocity;
		std::vector<Vertex> m_vVertex;
		std::string m_sFile;
		GLuint m_iWidth, m_iHeight, m_iVao, m_Vbo, m_iFrame, m_iStart, m_iEnd;
		GLuint m_textureId;
		BoundingBox m_bb;
		std::vector<std::vector<Vertex>> m_finalObject;
};


#endif //DynamicObject_H