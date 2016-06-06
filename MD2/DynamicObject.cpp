// $Id: $
//
// Author: Francisco Sans franjaviersans@gmail.com
//
// Complete history on bottom of file

#define FILE_REVISION "$Revision: $"


#include "DynamicObject.h"
#include <stdio.h>
#include <iostream>
using namespace std;



/**
* Default constructor
*
* @params velocity the velocity of the animation
* @params Width the width of the texture atlas
* @params Height the height of the texture atlas
* @params start the start frame of the animation
* @params end the end frame of the animation
*
*/
DynamicObject::DynamicObject(GLfloat velocity, GLuint Width, GLuint Height, GLint start, GLint end)
{
	m_fAnimationVelocity = velocity;

	m_iWidth = Width;
	m_iHeight = Height;

	m_iStart = start;
	m_iEnd = end;

	m_bb.Init();
}

/**
* Default destructor
*/
DynamicObject::~DynamicObject()
{
	glDeleteBuffers(1, &m_Vbo);
	glDeleteVertexArrays(1, &m_iVao);
}



/**
 * Load an MD2 model from file.
 *
 * Note: MD2 format stores model's data in little-endian ordering.  On
 * big-endian machines, you'll have to perform proper conversions.
 *
 * @params filename the name of the model to be loaded
 * @params TransformationMatrix a Matrix with all the transformations to be made to the model
 *
 */
void DynamicObject::Import(const char *filename, const glm::mat4 &TransformationMatrix)
{
	FILE *fp;
	int i;

	m_sFile = filename;

	fopen_s (&fp, filename, "rb");
	
	if (!fp)
	{
		fprintf (stderr, "Error: couldn't open \"%s\"!\n", filename);
		system("pause");
		exit(0);
	}

	md2_model_t m_Object;

	/* Read header */
	fread (&m_Object.header, 1, sizeof (md2_header_t), fp);

	if ((m_Object.header.ident != 844121161) ||
		(m_Object.header.version != 8))
	{
		/* Error! */
		fprintf (stderr, "Error: bad version or identifier\n");
		fclose (fp);
		system("pause");
		exit(0);
	}

	/* Memory allocations */
	m_Object.skins.resize(m_Object.header.num_skins);
	m_Object.texcoords.resize(m_Object.header.num_st);
	m_Object.triangles.resize(m_Object.header.num_tris);
	m_Object.frames.resize(m_Object.header.num_frames);
	m_Object.glcmds.resize(m_Object.header.num_glcmds);


	/* Read model data */
	if(m_Object.header.num_skins != 0)
	{
		fseek (fp, m_Object.header.offset_skins, SEEK_SET);
		fread (&m_Object.skins[0], sizeof (md2_skin_t),
			m_Object.header.num_skins, fp);
	}

	if(m_Object.header.num_st != 0)
	{
		fseek (fp, m_Object.header.offset_st, SEEK_SET);
		fread (&m_Object.texcoords[0], sizeof (md2_texCoord_t),
			m_Object.header.num_st, fp);
	}

	if(m_Object.header.num_tris != 0)
	{
		fseek (fp, m_Object.header.offset_tris, SEEK_SET);
		fread (&m_Object.triangles[0], sizeof (md2_triangle_t),
			m_Object.header.num_tris, fp);
	}

	if(m_Object.header.num_glcmds != 0)
	{
		fseek (fp, m_Object.header.offset_glcmds, SEEK_SET);
		fread (&m_Object.glcmds[0], sizeof (int), m_Object.header.num_glcmds, fp);
	}

	/* Read frames */
	fseek (fp, m_Object.header.offset_frames, SEEK_SET);
	for (i = 0; i < m_Object.header.num_frames; ++i)
	{
		/* Memory allocation for vertices of this frame */
		m_Object.frames[i].verts.resize(m_Object.header.num_vertices);

		/* Read frame data */
		fread (m_Object.frames[i].scale, sizeof (vec3_t), 1, fp);
		fread (m_Object.frames[i].translate, sizeof (vec3_t), 1, fp);
		fread (m_Object.frames[i].name, sizeof (char), 16, fp);
		fread (&m_Object.frames[i].verts[0], sizeof (md2_vertex_t),
			m_Object.header.num_vertices, fp);
	}

	fclose (fp);


	///////////////////End of file read/////////////////////////

	LoadTexture(&m_Object);
	Transform(&m_Object, TransformationMatrix);
	
	//Free the memory from the auxiliar data structure
	m_Object.skins.clear();
	m_Object.texcoords.clear();
	m_Object.triangles.clear();
	m_Object.glcmds.clear();
 
    for (int i = 0; i < m_Object.header.num_frames; ++i)
	{
		m_Object.frames[i].verts.clear();
	}

    m_Object.frames.clear();

	//End of freeing memory
	
	
	InitVAO();
}

/**
* Transform the model to a new data estructure
*
* @params m_vObject a pointer to the old data structure
* @params TransformationMatrix a Matrix to transform every vertex in the model
*
*/
void DynamicObject::Transform(md2_model_t * m_Object, const glm::mat4 &TransformationMatrix)
{
	//Precalculated normals
	GLuint cc = 0;
	glm::vec3 * m_pNormals = new glm::vec3[162];

	m_pNormals[cc++] = glm::vec3(-0.525731f,  0.000000f,  0.850651f); 
	m_pNormals[cc++] = glm::vec3(-0.442863f,  0.238856f,  0.864188f ); 
	m_pNormals[cc++] = glm::vec3(-0.295242f,  0.000000f,  0.955423f ); 
	m_pNormals[cc++] = glm::vec3( -0.309017f,  0.500000f,  0.809017f ); 
	m_pNormals[cc++] = glm::vec3( -0.162460f,  0.262866f,  0.951056f ); 
	m_pNormals[cc++] = glm::vec3(  0.000000f,  0.000000f,  1.000000f ); 
	m_pNormals[cc++] = glm::vec3(  0.000000f,  0.850651f,  0.525731f ); 
	m_pNormals[cc++] = glm::vec3( -0.147621f,  0.716567f,  0.681718f ); 
	m_pNormals[cc++] = glm::vec3(  0.147621f,  0.716567f,  0.681718f ); 
	m_pNormals[cc++] = glm::vec3(  0.000000f,  0.525731f,  0.850651f ); 
	m_pNormals[cc++] = glm::vec3(  0.309017f,  0.500000f,  0.809017f ); 
	m_pNormals[cc++] = glm::vec3(  0.525731f,  0.000000f,  0.850651f ); 
	m_pNormals[cc++] = glm::vec3(  0.295242f,  0.000000f,  0.955423f ); 
	m_pNormals[cc++] = glm::vec3(  0.442863f,  0.238856f,  0.864188f ); 
	m_pNormals[cc++] = glm::vec3(  0.162460f,  0.262866f,  0.951056f ); 
	m_pNormals[cc++] = glm::vec3( -0.681718f,  0.147621f,  0.716567f ); 
	m_pNormals[cc++] = glm::vec3( -0.809017f,  0.309017f,  0.500000f ); 
	m_pNormals[cc++] = glm::vec3( -0.587785f,  0.425325f,  0.688191f ); 
	m_pNormals[cc++] = glm::vec3( -0.850651f,  0.525731f,  0.000000f ); 
	m_pNormals[cc++] = glm::vec3( -0.864188f,  0.442863f,  0.238856f ); 
	m_pNormals[cc++] = glm::vec3( -0.716567f,  0.681718f,  0.147621f ); 
	m_pNormals[cc++] = glm::vec3( -0.688191f,  0.587785f,  0.425325f ); 
	m_pNormals[cc++] = glm::vec3( -0.500000f,  0.809017f,  0.309017f ); 
	m_pNormals[cc++] = glm::vec3( -0.238856f,  0.864188f,  0.442863f ); 
	m_pNormals[cc++] = glm::vec3( -0.425325f,  0.688191f,  0.587785f ); 
	m_pNormals[cc++] = glm::vec3( -0.716567f,  0.681718f, -0.147621f ); 
	m_pNormals[cc++] = glm::vec3( -0.500000f,  0.809017f, -0.309017f ); 
	m_pNormals[cc++] = glm::vec3( -0.525731f,  0.850651f,  0.000000f ); 
	m_pNormals[cc++] = glm::vec3(  0.000000f,  0.850651f, -0.525731f ); 
	m_pNormals[cc++] = glm::vec3( -0.238856f,  0.864188f, -0.442863f ); 
	m_pNormals[cc++] = glm::vec3(  0.000000f,  0.955423f, -0.295242f ); 
	m_pNormals[cc++] = glm::vec3( -0.262866f,  0.951056f, -0.162460f ); 
	m_pNormals[cc++] = glm::vec3(  0.000000f,  1.000000f,  0.000000f ); 
	m_pNormals[cc++] = glm::vec3(  0.000000f,  0.955423f,  0.295242f ); 
	m_pNormals[cc++] = glm::vec3( -0.262866f,  0.951056f,  0.162460f ); 
	m_pNormals[cc++] = glm::vec3(  0.238856f,  0.864188f,  0.442863f ); 
	m_pNormals[cc++] = glm::vec3(  0.262866f,  0.951056f,  0.162460f ); 
	m_pNormals[cc++] = glm::vec3(  0.500000f,  0.809017f,  0.309017f ); 
	m_pNormals[cc++] = glm::vec3(  0.238856f,  0.864188f, -0.442863f ); 
	m_pNormals[cc++] = glm::vec3(  0.262866f,  0.951056f, -0.162460f ); 
	m_pNormals[cc++] = glm::vec3(  0.500000f,  0.809017f, -0.309017f ); 
	m_pNormals[cc++] = glm::vec3(  0.850651f,  0.525731f,  0.000000f ); 
	m_pNormals[cc++] = glm::vec3(  0.716567f,  0.681718f,  0.147621f ); 
	m_pNormals[cc++] = glm::vec3(  0.716567f,  0.681718f, -0.147621f ); 
	m_pNormals[cc++] = glm::vec3(  0.525731f,  0.850651f,  0.000000f ); 
	m_pNormals[cc++] = glm::vec3(  0.425325f,  0.688191f,  0.587785f ); 
	m_pNormals[cc++] = glm::vec3(  0.864188f,  0.442863f,  0.238856f ); 
	m_pNormals[cc++] = glm::vec3(  0.688191f,  0.587785f,  0.425325f ); 
	m_pNormals[cc++] = glm::vec3(  0.809017f,  0.309017f,  0.500000f ); 
	m_pNormals[cc++] = glm::vec3(  0.681718f,  0.147621f,  0.716567f ); 
	m_pNormals[cc++] = glm::vec3(  0.587785f,  0.425325f,  0.688191f ); 
	m_pNormals[cc++] = glm::vec3(  0.955423f,  0.295242f,  0.000000f ); 
	m_pNormals[cc++] = glm::vec3(  1.000000f,  0.000000f,  0.000000f ); 
	m_pNormals[cc++] = glm::vec3(  0.951056f,  0.162460f,  0.262866f ); 
	m_pNormals[cc++] = glm::vec3(  0.850651f, -0.525731f,  0.000000f ); 
	m_pNormals[cc++] = glm::vec3(  0.955423f, -0.295242f,  0.000000f ); 
	m_pNormals[cc++] = glm::vec3(  0.864188f, -0.442863f,  0.238856f ); 
	m_pNormals[cc++] = glm::vec3(  0.951056f, -0.162460f,  0.262866f ); 
	m_pNormals[cc++] = glm::vec3(  0.809017f, -0.309017f,  0.500000f ); 
	m_pNormals[cc++] = glm::vec3(  0.681718f, -0.147621f,  0.716567f ); 
	m_pNormals[cc++] = glm::vec3(  0.850651f,  0.000000f,  0.525731f ); 
	m_pNormals[cc++] = glm::vec3(  0.864188f,  0.442863f, -0.238856f ); 
	m_pNormals[cc++] = glm::vec3(  0.809017f,  0.309017f, -0.500000f ); 
	m_pNormals[cc++] = glm::vec3(  0.951056f,  0.162460f, -0.262866f ); 
	m_pNormals[cc++] = glm::vec3(  0.525731f,  0.000000f, -0.850651f ); 
	m_pNormals[cc++] = glm::vec3(  0.681718f,  0.147621f, -0.716567f ); 
	m_pNormals[cc++] = glm::vec3(  0.681718f, -0.147621f, -0.716567f ); 
	m_pNormals[cc++] = glm::vec3(  0.850651f,  0.000000f, -0.525731f ); 
	m_pNormals[cc++] = glm::vec3(  0.809017f, -0.309017f, -0.500000f ); 
	m_pNormals[cc++] = glm::vec3(  0.864188f, -0.442863f, -0.238856f ); 
	m_pNormals[cc++] = glm::vec3(  0.951056f, -0.162460f, -0.262866f ); 
	m_pNormals[cc++] = glm::vec3(  0.147621f,  0.716567f, -0.681718f ); 
	m_pNormals[cc++] = glm::vec3(  0.309017f,  0.500000f, -0.809017f ); 
	m_pNormals[cc++] = glm::vec3(  0.425325f,  0.688191f, -0.587785f ); 
	m_pNormals[cc++] = glm::vec3(  0.442863f,  0.238856f, -0.864188f ); 
	m_pNormals[cc++] = glm::vec3(  0.587785f,  0.425325f, -0.688191f ); 
	m_pNormals[cc++] = glm::vec3(  0.688191f,  0.587785f, -0.425325f ); 
	m_pNormals[cc++] = glm::vec3( -0.147621f,  0.716567f, -0.681718f ); 
	m_pNormals[cc++] = glm::vec3( -0.309017f,  0.500000f, -0.809017f ); 
	m_pNormals[cc++] = glm::vec3(  0.000000f,  0.525731f, -0.850651f ); 
	m_pNormals[cc++] = glm::vec3( -0.525731f,  0.000000f, -0.850651f ); 
	m_pNormals[cc++] = glm::vec3( -0.442863f,  0.238856f, -0.864188f ); 
	m_pNormals[cc++] = glm::vec3( -0.295242f,  0.000000f, -0.955423f ); 
	m_pNormals[cc++] = glm::vec3( -0.162460f,  0.262866f, -0.951056f ); 
	m_pNormals[cc++] = glm::vec3(  0.000000f,  0.000000f, -1.000000f ); 
	m_pNormals[cc++] = glm::vec3(  0.295242f,  0.000000f, -0.955423f ); 
	m_pNormals[cc++] = glm::vec3(  0.162460f,  0.262866f, -0.951056f ); 
	m_pNormals[cc++] = glm::vec3( -0.442863f, -0.238856f, -0.864188f ); 
	m_pNormals[cc++] = glm::vec3( -0.309017f, -0.500000f, -0.809017f ); 
	m_pNormals[cc++] = glm::vec3( -0.162460f, -0.262866f, -0.951056f ); 
	m_pNormals[cc++] = glm::vec3(  0.000000f, -0.850651f, -0.525731f ); 
	m_pNormals[cc++] = glm::vec3( -0.147621f, -0.716567f, -0.681718f ); 
	m_pNormals[cc++] = glm::vec3(  0.147621f, -0.716567f, -0.681718f ); 
	m_pNormals[cc++] = glm::vec3(  0.000000f, -0.525731f, -0.850651f ); 
	m_pNormals[cc++] = glm::vec3(  0.309017f, -0.500000f, -0.809017f ); 
	m_pNormals[cc++] = glm::vec3(  0.442863f, -0.238856f, -0.864188f ); 
	m_pNormals[cc++] = glm::vec3(  0.162460f, -0.262866f, -0.951056f ); 
	m_pNormals[cc++] = glm::vec3(  0.238856f, -0.864188f, -0.442863f ); 
	m_pNormals[cc++] = glm::vec3(  0.500000f, -0.809017f, -0.309017f ); 
	m_pNormals[cc++] = glm::vec3(  0.425325f, -0.688191f, -0.587785f ); 
	m_pNormals[cc++] = glm::vec3(  0.716567f, -0.681718f, -0.147621f ); 
	m_pNormals[cc++] = glm::vec3(  0.688191f, -0.587785f, -0.425325f ); 
	m_pNormals[cc++] = glm::vec3(  0.587785f, -0.425325f, -0.688191f ); 
	m_pNormals[cc++] = glm::vec3(  0.000000f, -0.955423f, -0.295242f ); 
	m_pNormals[cc++] = glm::vec3(  0.000000f, -1.000000f,  0.000000f ); 
	m_pNormals[cc++] = glm::vec3(  0.262866f, -0.951056f, -0.162460f ); 
	m_pNormals[cc++] = glm::vec3(  0.000000f, -0.850651f,  0.525731f ); 
	m_pNormals[cc++] = glm::vec3(  0.000000f, -0.955423f,  0.295242f ); 
	m_pNormals[cc++] = glm::vec3(  0.238856f, -0.864188f,  0.442863f ); 
	m_pNormals[cc++] = glm::vec3(  0.262866f, -0.951056f,  0.162460f ); 
	m_pNormals[cc++] = glm::vec3(  0.500000f, -0.809017f,  0.309017f ); 
	m_pNormals[cc++] = glm::vec3(  0.716567f, -0.681718f,  0.147621f ); 
	m_pNormals[cc++] = glm::vec3(  0.525731f, -0.850651f,  0.000000f ); 
	m_pNormals[cc++] = glm::vec3( -0.238856f, -0.864188f, -0.442863f ); 
	m_pNormals[cc++] = glm::vec3( -0.500000f, -0.809017f, -0.309017f ); 
	m_pNormals[cc++] = glm::vec3( -0.262866f, -0.951056f, -0.162460f ); 
	m_pNormals[cc++] = glm::vec3( -0.850651f, -0.525731f,  0.000000f ); 
	m_pNormals[cc++] = glm::vec3( -0.716567f, -0.681718f, -0.147621f ); 
	m_pNormals[cc++] = glm::vec3( -0.716567f, -0.681718f,  0.147621f ); 
	m_pNormals[cc++] = glm::vec3( -0.525731f, -0.850651f,  0.000000f ); 
	m_pNormals[cc++] = glm::vec3( -0.500000f, -0.809017f,  0.309017f ); 
	m_pNormals[cc++] = glm::vec3( -0.238856f, -0.864188f,  0.442863f ); 
	m_pNormals[cc++] = glm::vec3( -0.262866f, -0.951056f,  0.162460f ); 
	m_pNormals[cc++] = glm::vec3( -0.864188f, -0.442863f,  0.238856f ); 
	m_pNormals[cc++] = glm::vec3( -0.809017f, -0.309017f,  0.500000f ); 
	m_pNormals[cc++] = glm::vec3( -0.688191f, -0.587785f,  0.425325f ); 
	m_pNormals[cc++] = glm::vec3( -0.681718f, -0.147621f,  0.716567f ); 
	m_pNormals[cc++] = glm::vec3( -0.442863f, -0.238856f,  0.864188f ); 
	m_pNormals[cc++] = glm::vec3( -0.587785f, -0.425325f,  0.688191f ); 
	m_pNormals[cc++] = glm::vec3( -0.309017f, -0.500000f,  0.809017f ); 
	m_pNormals[cc++] = glm::vec3( -0.147621f, -0.716567f,  0.681718f ); 
	m_pNormals[cc++] = glm::vec3( -0.425325f, -0.688191f,  0.587785f ); 
	m_pNormals[cc++] = glm::vec3( -0.162460f, -0.262866f,  0.951056f ); 
	m_pNormals[cc++] = glm::vec3(  0.442863f, -0.238856f,  0.864188f ); 
	m_pNormals[cc++] = glm::vec3(  0.162460f, -0.262866f,  0.951056f ); 
	m_pNormals[cc++] = glm::vec3(  0.309017f, -0.500000f,  0.809017f ); 
	m_pNormals[cc++] = glm::vec3(  0.147621f, -0.716567f,  0.681718f ); 
	m_pNormals[cc++] = glm::vec3(  0.000000f, -0.525731f,  0.850651f ); 
	m_pNormals[cc++] = glm::vec3(  0.425325f, -0.688191f,  0.587785f ); 
	m_pNormals[cc++] = glm::vec3(  0.587785f, -0.425325f,  0.688191f ); 
	m_pNormals[cc++] = glm::vec3(  0.688191f, -0.587785f,  0.425325f ); 
	m_pNormals[cc++] = glm::vec3( -0.955423f,  0.295242f,  0.000000f ); 
	m_pNormals[cc++] = glm::vec3( -0.951056f,  0.162460f,  0.262866f ); 
	m_pNormals[cc++] = glm::vec3( -1.000000f,  0.000000f,  0.000000f ); 
	m_pNormals[cc++] = glm::vec3( -0.850651f,  0.000000f,  0.525731f ); 
	m_pNormals[cc++] = glm::vec3( -0.955423f, -0.295242f,  0.000000f ); 
	m_pNormals[cc++] = glm::vec3( -0.951056f, -0.162460f,  0.262866f ); 
	m_pNormals[cc++] = glm::vec3( -0.864188f,  0.442863f, -0.238856f ); 
	m_pNormals[cc++] = glm::vec3( -0.951056f,  0.162460f, -0.262866f ); 
	m_pNormals[cc++] = glm::vec3( -0.809017f,  0.309017f, -0.500000f ); 
	m_pNormals[cc++] = glm::vec3( -0.864188f, -0.442863f, -0.238856f ); 
	m_pNormals[cc++] = glm::vec3( -0.951056f, -0.162460f, -0.262866f ); 
	m_pNormals[cc++] = glm::vec3( -0.809017f, -0.309017f, -0.500000f ); 
	m_pNormals[cc++] = glm::vec3( -0.681718f,  0.147621f, -0.716567f ); 
	m_pNormals[cc++] = glm::vec3( -0.681718f, -0.147621f, -0.716567f ); 
	m_pNormals[cc++] = glm::vec3( -0.850651f,  0.000000f, -0.525731f ); 
	m_pNormals[cc++] = glm::vec3( -0.688191f,  0.587785f, -0.425325f ); 
	m_pNormals[cc++] = glm::vec3( -0.587785f,  0.425325f, -0.688191f ); 
	m_pNormals[cc++] = glm::vec3( -0.425325f,  0.688191f, -0.587785f ); 
	m_pNormals[cc++] = glm::vec3( -0.425325f, -0.688191f, -0.587785f ); 
	m_pNormals[cc++] = glm::vec3( -0.587785f, -0.425325f, -0.688191f ); 
	m_pNormals[cc++] = glm::vec3( -0.688191f, -0.587785f, -0.425325f );


	//Every md2 model have to be rotated in this way
	/*glm::mat4 FinalTransform =	TransformationMatrix *
								glm::rotate(-90.0f,glm::vec3(1.0f,0.0f,0.0f)) * 
								glm::rotate(-90.0f,glm::vec3(0.0f,0.0f,1.0f));*/

	glm::mat4 FinalTransform = TransformationMatrix;


	int i, j;
	md2_frame_t *pframe;
	md2_vertex_t *pvert;
	vec3_t v_curr, norm;
	glm::vec3 n_curr;

	m_finalObject.resize(m_Object->frames.size());
	Vertex aux_vertex;
	vector<Vertex> aux_frame;

	for(GLuint k =0; k < m_Object->frames.size(); ++k)
	{
		aux_frame.resize( m_Object->header.num_tris * 3);

		for (i = 0; i < m_Object->header.num_tris; ++i)
		{
			/* Draw each vertex */
			for (j = 0; j < 3; ++j)
			{
			
				pframe = &m_Object->frames[k];
				pvert = &pframe->verts[m_Object->triangles[i].vertex[j]];

				/* Compute texture coordinates */
				aux_vertex.TextureCoord = glm::vec2(	((GLfloat)m_Object->texcoords[m_Object->triangles[i].st[j]].s / m_Object->header.skinwidth),
														1.0f - ((GLfloat)m_Object->texcoords[m_Object->triangles[i].st[j]].t / m_Object->header.skinheight));


				/* Interpolate normals */
				n_curr = m_pNormals[pvert->normalIndex];

				norm[0] = n_curr[0];
				norm[1] = n_curr[1];
				norm[2] = n_curr[2];

				aux_vertex.NormalCoord = glm::vec3(norm[0], norm[1], norm[2]);

				/* Interpolate vertices */
				v_curr[0] = pframe->scale[0] * pvert->v[0] + pframe->translate[0];
				v_curr[1] = pframe->scale[1] * pvert->v[1] + pframe->translate[1];
				v_curr[2] = pframe->scale[2] * pvert->v[2] + pframe->translate[2];


				aux_vertex.WorldCoord = FinalTransform * glm::vec4(v_curr[0],v_curr[1],v_curr[2],1.0f);


				m_bb.AppendPoint(aux_vertex.WorldCoord.x,aux_vertex.WorldCoord.y,aux_vertex.WorldCoord.z);

				aux_frame[i*3 + j] = aux_vertex;
			}
		}

		m_finalObject[k] = aux_frame;
	}

	delete m_pNormals;
}

/**
 * Calculate the current frame in animation beginning at frame
 * 'start' and ending at frame 'end', given interpolation percent.
 * interp will be reseted to 0.0 if the next frame is reached.
 *
 * @params time is the time elapsed from the previos time
 *
 */
void DynamicObject::Animate (float time)
{
	if(m_fInterpolation <= 0.0f)
		m_fInterpolation  = 0.0f;
	
	m_fInterpolation += time * m_fAnimationVelocity;

	if ((m_iFrame < m_iStart) || (m_iFrame > m_iEnd))
	m_iFrame = m_iStart;

	if (m_fInterpolation >= 1.0f)
	{
		/* Move to next frame */
		m_fInterpolation = 0.0f;
		(m_iFrame)++;

		if (m_iFrame >= m_iEnd)
			m_iFrame = m_iStart;
	}
}


/**
* Render MD2 using Vertex Buffer Object
*/
void DynamicObject::Draw()
{

	//Enable Texture
	glActiveTexture(GL_TEXTURE0);
	TextureManager::Inst()->BindTexture(m_textureId);


	//Bind Buffers
	glBindVertexArray(m_iVao);
	
	glDrawArrays(GL_TRIANGLES,0, m_vVertex.size());


	//Unbid Buffer
	glBindVertexArray(0);

	/*glDisableVertexAttribArray(WORLD_COORD_LOCATION);
	glDisableVertexAttribArray(NORMAL_COORD_LOCATION);
	glDisableVertexAttribArray(TEXTURE_COORD_LOCATION);*/
}

/**
* Initialize Vertex Buffer Object
*/		
void  DynamicObject::InitVAO()
{
	m_vVertex.resize(m_finalObject[0].size());

	glGenBuffers(1, &m_Vbo);

	// bind buffer for positions and copy data into buffer
	// GL_ARRAY_BUFFER is the buffer type we use to feed attributes
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
 
	//Set buffer to null with Stream Draw for dynamic feeding
	glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STREAM_DRAW);

	//Disable Buffers and vertex attributes
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	
	//Generate the VAO
	glGenVertexArrays(1, &m_iVao);
	glBindVertexArray(m_iVao);

	// bind buffer for positions and copy data into buffer
	// GL_ARRAY_BUFFER is the buffer type we use to feed attributes
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

	glEnableVertexAttribArray(WORLD_COORD_LOCATION);
	glEnableVertexAttribArray(NORMAL_COORD_LOCATION);
	glEnableVertexAttribArray(TEXTURE_COORD_LOCATION);


	glVertexAttribPointer(WORLD_COORD_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0)); //Vertexs
	glVertexAttribPointer(NORMAL_COORD_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(GL_FLOAT) * 4)); //Normals
	glVertexAttribPointer(TEXTURE_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(GL_FLOAT) * 7)); //Text Coords

	//Unbind the vertex array	
	glBindVertexArray(0);


	//Disable Buffers and vertex attributes
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/**
* Update Vertex Buffer Object
*/
void  DynamicObject::UpdateVAO()
{
	
	/* Check if n is in a valid range */
	if ((m_iFrame < 0) || (m_iFrame + 1 > m_finalObject.size() - 1))
	return;
	
	Vertex aux_vertex;

	for(GLuint i=0 ;i < m_finalObject[m_iFrame].size();++i)
	{
		
		/* Compute texture coordinates */
		aux_vertex.TextureCoord = m_finalObject[m_iFrame][i].TextureCoord;

			
		/* Interpolate normals */
		aux_vertex.NormalCoord =	m_finalObject[m_iFrame][i].NormalCoord + 
									m_fInterpolation * ( m_finalObject[m_iFrame + 1][i].NormalCoord
														- m_finalObject[m_iFrame][i].NormalCoord);

		/* Interpolate vertices */
		aux_vertex.WorldCoord = m_finalObject[m_iFrame][i].WorldCoord + 
								m_fInterpolation * ( m_finalObject[m_iFrame + 1][i].WorldCoord
														- m_finalObject[m_iFrame][i].WorldCoord);

		m_vVertex[i] = aux_vertex;
	}

	//Bind Buffer to copy new data
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
 
	//Free the buffer
	glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STREAM_DRAW);

	//Fill it with new data
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vVertex.size(), &m_vVertex[0], GL_STREAM_DRAW);

	//Disable Buffers and vertex attributes
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

/**
* Method to Load texture (If any)
*
* @params m_Object a pointer to the md2 structure
* 
*/
void DynamicObject::LoadTexture(md2_model_t * m_Object)
{
	if(m_Object->header.num_skins != 0)
	{
		
		m_textureId = 10;
		int flo = m_sFile.find_last_of("/");
		if (TextureManager::Inst()->LoadTexture2D((m_sFile.substr(0, flo) + "/" + m_Object->skins[0].name).c_str(), m_textureId)){
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}else{
			GLubyte data[1];
			data[0] = 255;
			TextureManager::Inst()->CreateTexture2D(m_textureId, 1, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST, data);
		}
	}
	else
	{
		//Load a white texture in case the model doesn't have one
		GLubyte data[1];
		data[0] = 255;
		TextureManager::Inst()->CreateTexture2D(m_textureId, 1, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST, data);
	}
}

/**
* Get Texture Width
*
* @return width of the Texture
*/
GLuint DynamicObject::GetTextureWidth()
{
	return m_iWidth;
}


/**
* Get Texture Height
*
* @return height of the Texture
*/
GLuint DynamicObject::GetTextureHeight()
{
	return m_iHeight;
}


/**
* Method to get Bounding Box
*
* @return the bounding box of the model
*/
BoundingBox DynamicObject::GetBoundingBox()
{
	return m_bb;
}

/**
* Method to get Bounding Box
*
* @params vel new velocity
*/
//Method to set velocity of the animation
void DynamicObject::SetVelocity(float vel){
	m_fAnimationVelocity = vel;
}


#undef FILE_REVISION

// Revision History:
// $Log: $
// $Header: $
// $Id: $