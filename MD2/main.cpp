// Includes
#include "Definitions.h"
#include <iostream>
#include <fstream>
using namespace std;
#include "DynamicObject.h"
#include "GLSLProgram.h"
#include "TextureManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


using std::cout;
using std::endl;



namespace glfwFunc
{
	GLFWwindow* glfwWindow;
	int WINDOW_WIDTH = 1024;
	int WINDOW_HEIGHT = 768;
	std::string strNameWindow = "Compute Shader Volume Ray Casting";

	const float NCP = 1.0f;
	const float FCP = 10.0f;
	const float fAngle = 45.f * (3.14f / 180.0f); //In radians

	float color[] = { 1, 1, 1 };
	bool pintar = false;

	GLSLProgram m_program;
	glm::mat4x4 mProjMatrix, mMVP, mModel, mView;
	glm::vec3 trans;
	float scaleFactor;

	double lastime;


	int inicio = 40, fin = 60;
	DynamicObject * md2file = NULL;


	///< Callback function used by GLFW to capture some possible error.
	void errorCB(int error, const char* description)
	{
		printf("%s\n", description);
	}



	///
	/// The keyboard function call back
	/// @param window id of the window that received the event
	/// @param iKey the key pressed or released
	/// @param iScancode the system-specific scancode of the key.
	/// @param iAction can be GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT
	/// @param iMods Bit field describing which modifier keys were held down (Shift, Alt, & so on)
	///
	void keyboardCB(GLFWwindow* window, int iKey, int iScancode, int iAction, int iMods)
	{
		/*if (iAction == GLFW_PRESS)
		{
			switch (iKey)
			{
			case GLFW_KEY_ESCAPE:
			case GLFW_KEY_Q:
				glfwSetWindowShouldClose(window, GL_TRUE);
				break;
			case GLFW_KEY_SPACE:
				g_pTransferFunc->isVisible = !g_pTransferFunc->isVisible;
				break;
			}
		}*/
		/* Escape */
		/*if (key == 27)
			exit(0);
		switch (key)
		{
		case '1':
			//Animate (0, md2fileheader.num_frames - 1, &n, &interp);
			xxx++;
			break;
		case '2':
			xxx--;
			break;
		case '3':
			yyy++;
			md2file->m_iEnd++;
			break;
		case '4':
			yyy--;
			md2file->m_iEnd--;
			break;
		case '5':
			zzz++;
			break;
		case '6':
			zzz--;
			break;
		case 't':
			// imdebugTexImagef(GL_TEXTURE_2D, obj->GetTextureAtlas()->GetTextId(),GL_RGB,0,"rgb b=16f");
			break;
		case 'u':
			break;
		}*/
		// printf("inicio:%d fin:%d\n",md2file->m_iStart,md2file->m_iEnd);
	}

	inline int TwEventMousePosGLFW3(GLFWwindow* window, double xpos, double ypos)
	{

		
		return true;
	}

	int TwEventMouseButtonGLFW3(GLFWwindow* window, int button, int action, int mods)
	{
		

		return true;
	}

	///< The resizing function
	void resizeCB(GLFWwindow* window, int iWidth, int iHeight)
	{

		WINDOW_WIDTH = iWidth;
		WINDOW_HEIGHT = iHeight;

		if (iHeight == 0) iHeight = 1;
		float ratio = iWidth / float(iHeight);
		glViewport(0, 0, iWidth, iHeight);

		mProjMatrix = glm::perspective(float(fAngle), ratio, NCP, FCP);
		//mProjMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f);
	}



	///< The main rendering function.
	void draw()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		GLenum err = GL_NO_ERROR;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			std::cout << "INICIO " << err << std::endl;
		}


		glEnable(GL_TEXTURE_2D);

		//Draw a Cube
		m_program.use();
		{
			//center an normalize the object
			mModel = glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor)) * glm::translate(glm::mat4(1.0f), -trans);
			mView = glm::lookAt(glm::vec3(0.0f,0.0f,-1.75f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f));

			mModel = mView * mModel;
			m_program.setUniform("Projection", mProjMatrix);
			m_program.setUniform("modelView", mModel);

			//Do animation
			double now = glfwGetTime();
			md2file->Animate(now - lastime);
			lastime = now;

			md2file->UpdateVAO();

			glFrontFace(GL_CW);
			md2file->Draw();
		}

		glfwSwapBuffers(glfwWindow);
	}


	///
	/// Init all data and variables.
	/// @return true if everything is ok, false otherwise
	///
	bool initialize()
	{
		//Init GLEW
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			printf("- glew Init failed :(\n");
			return false;
		}
		printf("OpenGL version: %s\n", glGetString(GL_VERSION));
		printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
		printf("Vendor: %s\n", glGetString(GL_VENDOR));
		printf("Renderer: %s\n", glGetString(GL_RENDERER));


		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
		/*glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glCullFace(GL_FRONT);*/


		// send window size events to AntTweakBar
		glfwSetWindowSizeCallback(glfwWindow, resizeCB);
		glfwSetMouseButtonCallback(glfwWindow, (GLFWmousebuttonfun)TwEventMouseButtonGLFW3);
		glfwSetCursorPosCallback(glfwWindow, (GLFWcursorposfun)TwEventMousePosGLFW3);
		glfwSetKeyCallback(glfwWindow, (GLFWkeyfun)keyboardCB);


		//load the shaders
		try{
			m_program.compileShader(string("Normal.vert"), GLSLShader::VERTEX);
			m_program.compileShader(string("Normal.frag"), GLSLShader::FRAGMENT);
			m_program.link();
		}
		catch (GLSLProgramException & e) {
			std::cerr << e.what() << std::endl;
			exit(EXIT_FAILURE);
		}


		md2file = new DynamicObject(2.5f, 512, 512, 0, 100);


		//center this model
		md2file->Import("Model/warrior.md2", glm::mat4());


		lastime = glfwGetTime();
		

		trans.x = (md2file->m_bb.m_fMaxx + md2file->m_bb.m_fMinx)/2.0f;
		trans.y = (md2file->m_bb.m_fMaxy + md2file->m_bb.m_fMiny) / 2.0f;
		trans.z = (md2file->m_bb.m_fMaxz + md2file->m_bb.m_fMinz) / 2.0f;

		scaleFactor = 1.0f / fmaxf(md2file->m_bb.m_fMaxx - md2file->m_bb.m_fMinx,
			fmaxf(md2file->m_bb.m_fMaxy - md2file->m_bb.m_fMiny, md2file->m_bb.m_fMaxz - md2file->m_bb.m_fMinz));

		return true;
	}


	/// Here all data must be destroyed + glfwTerminate
	void destroy()
	{
		TextureManager::Inst()->UnloadAllTextures();
		if (md2file) md2file->~DynamicObject();
		glfwTerminate();
		glfwDestroyWindow(glfwWindow);
	}
}

int main(int argc, char** argv)
{

	glfwSetErrorCallback(glfwFunc::errorCB);
	if (!glfwInit())	exit(EXIT_FAILURE);
	glfwFunc::glfwWindow = glfwCreateWindow(glfwFunc::WINDOW_WIDTH, glfwFunc::WINDOW_HEIGHT, glfwFunc::strNameWindow.c_str(), NULL, NULL);
	if (!glfwFunc::glfwWindow)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(glfwFunc::glfwWindow);
	if (!glfwFunc::initialize()) exit(EXIT_FAILURE);
	glfwFunc::resizeCB(glfwFunc::glfwWindow, glfwFunc::WINDOW_WIDTH, glfwFunc::WINDOW_HEIGHT);	//just the 1st time;


	while (!glfwWindowShouldClose(glfwFunc::glfwWindow))
	{

		glfwFunc::draw();
		glfwPollEvents();	//or glfwWaitEvents()
	}


	glfwFunc::destroy();
	return EXIT_SUCCESS;
	}





/*
//
// Animate model from frames 0 to num_frames-1 
md2file->Animate(double(curent_time - last_time));
md2file->UpdateVAO();
;




/* Load MD2 model file 
md2file->Import(filename, glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f))*glm::scale(glm::vec3(0.02, 0.02, 0.02)));
*/