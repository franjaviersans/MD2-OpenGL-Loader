// $Id: $
//
// Author: Francisco Sans franjaviersans@gmail.com
//
// Complete history on bottom of file

#define FILE_REVISION "$Revision: $"

#include "BoundingBox.h"
#include <iostream>

/**
* Default constructor
*/
BoundingBox::BoundingBox()
{
	m_fMaxx = -99999.0f;
	m_fMinx = 99999.0f;
	m_fMaxy = -99999.0f;
	m_fMiny = 99999.0f;
	m_fMaxz = -99999.0f;
	m_fMinz = 99999.0f;

	CalculateSize();
}

/**
* Default destructor
*/
BoundingBox::~BoundingBox()
{

}


/**
* Initializate bounding box
*/
void BoundingBox::Init()
{
	m_fMaxx = -99999.0f;
	m_fMinx = 99999.0f;
	m_fMaxy = -99999.0f;
	m_fMiny = 99999.0f;
	m_fMaxz = -99999.0f;
	m_fMinz = 99999.0f;

	CalculateSize();
}


/**
* Method to set the bounding box
*
* @params maxx x max coord
* @params minx x min coord
* @params maxy y max coord
* @params miny y min coord
* @params maxz z max coord
* @params minz z min coord
*
*/
void BoundingBox::SetBoundingBox(GLfloat maxx, GLfloat minx, GLfloat maxy, GLfloat miny, GLfloat maxz, GLfloat minz)
{
	m_fMaxx = maxx;
	m_fMinx = minx;
	m_fMaxy = maxy;
	m_fMiny = miny;
	m_fMaxz = maxz;
	m_fMinz = minz;

	CalculateSize();
}


/**
* Method to append a pint to the bounding box and resize it if necesary
*
* @params x x coord
* @params y y coord
* @params z z coord
* 
*/
void BoundingBox::AppendPoint(GLfloat x, GLfloat y, GLfloat z)
{

	if(x > m_fMaxx ) m_fMaxx = x;
	if(x < m_fMinx ) m_fMinx = x;
	if(y > m_fMaxy ) m_fMaxy = y;
	if(y < m_fMiny ) m_fMiny = y;
	if(z > m_fMaxz ) m_fMaxz = z;
	if(z < m_fMinz ) m_fMinz = z;

	CalculateSize();
}


/**
* Method to generate the bounding box of two bounding box
*
* @params box a bounding box
*
* @return the joined bounding box
* 
*/
BoundingBox BoundingBox::JoingBoundingBox(BoundingBox box)
{

	AppendPoint(box.m_fMaxx, box.m_fMaxy, box.m_fMaxz);
	AppendPoint(box.m_fMinx, box.m_fMiny, box.m_fMinz);

	return *this;
}

/**
* Method to calculate the size of the bb in the 3 axes
*/
void BoundingBox::CalculateSize()
{
	m_fLenghtX = abs(m_fMaxx - m_fMinx);
	m_fLenghtY = abs(m_fMaxy - m_fMiny);
	m_fLenghtZ = abs(m_fMaxz - m_fMinz);
}


#undef FILE_REVISION

// Revision History:
// $Log: $
// $Header: $
// $Id: $