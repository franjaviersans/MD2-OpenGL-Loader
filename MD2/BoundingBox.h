// $Id: $
//
// Author: Francisco Sans franjaviersans@gmail.com
//
// Complete history on bottom of file

#ifndef BoundingBox_H
#define BoundingBox_H



//Includes
#include "Definitions.h"
#include <math.h>


/**
* Class Bitmask.
* A class to define a Boundin Box
*
*/
class BoundingBox
{

	//Functions

	public:
		///Default constructor
		BoundingBox();

		///Default destructor
		~BoundingBox();

		///Initializate bounding box
		void Init();

		///Method to set the bounding box
		void SetBoundingBox(GLfloat maxx, GLfloat minx, GLfloat maxy, GLfloat miny, GLfloat maxz, GLfloat minz);

		///Method to append a pint to the bounding box and resize it if necesary
		void AppendPoint(GLfloat x, GLfloat y, GLfloat z);

		///Method to generate the bounding box of two bounding box
		BoundingBox JoingBoundingBox(BoundingBox box);

	private:
		///Method to calculate the size of the bb
		void CalculateSize();


	//Variables

	public:
		GLfloat m_fMaxx, m_fMinx, m_fMaxy, m_fMiny, m_fMaxz, m_fMinz, m_fLenghtX, m_fLenghtY, m_fLenghtZ;
};


#endif //BoundingBox_H