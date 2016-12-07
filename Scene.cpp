#include "Scene.h"


/* Notes - add to coursework

# Multiple cameras - fixed viewpoint etc
# Multiple lights, different colour, type.
# Texture filtering (billener etc)
# Matrix stack for modelling + animation
# Vertex arrays (not including model loading)


*/

Scene::Scene(Input *in)
{
	// Store pointer for input class
	input = in;
		
	//OpenGL settings
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.39f, 0.58f, 93.0f, 1.0f);			// Cornflour Blue Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glClearStencil(0);									// Clear stencil buffer
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // The type of blending to do

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	// Other OpenGL / render setting should be applied here.
	glEnable(GL_LIGHTING);								// Enables lighting
	glEnable(GL_COLOR_MATERIAL);						// Enables colour materials
	
	camera.render();
	camera.update(0);

	// Initialising varibles
	rotation = 0;
	speed = 1.5;
	cameraSpeed = 0.5;

	// Keeps track which shape is in use
	shapeCounter = 0;


	wireframeToggle = false;
	modelToggle = false;

	// Import the teapot model using crate texture
	model.load("Models/sphere.obj", "gfx/Sand.png");

	// Enables Texturing
	glEnable(GL_TEXTURE_2D);

	// Specify texture calculation
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Loads and stores all textures in variables
	setTextures();

}

void Scene::update(float dt)
{
	// Update object and variables (camera, rotation, etc).
	rotation += speed * dt;
	rotation2 += (speed / 2) * dt;


	// Handle user input

	// ------------- Wireframe toggle -----------
	if (input->isKeyDown('x'))
	{
		if (wireframeToggle == false)
		{
			// Turn on wireframe mode
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			wireframeToggle = true;
		}
		else if (wireframeToggle == true)
		{
			// Turn off wireframe mode
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			wireframeToggle = false;
		}

		// Reset key state
		input->SetKeyUp('x');
	}
	// ------------------------------------------


	// ------------- Shape change button -----------
	if (input->isKeyDown('y'))
	{
		shapeCounter++;

		if (shapeCounter > 2)
		{
			shapeCounter = 0;
		}

		// Calls the shape selection function
		shapeSelection();

		// Reset key state
		input->SetKeyUp('y');
	}
	// ---------------------------------------------



	// CAMERA MOVEMENT ----------------------------

	// Horizontal Movement
	if (input->isKeyDown('d'))
	{
		// Move camera right
		newPosX = camera.getPosX();
		camera.setPosX((newPosX + cameraSpeed));
		camera.update(0);

		// Reset key state
		input->SetKeyUp('d');
	}

	if (input->isKeyDown('a'))
	{
		// Move camera left
		newPosX = camera.getPosX();
		camera.setPosX((newPosX - cameraSpeed));
		camera.update(0);

		// Reset key state
		input->SetKeyUp('a');
	}


	// Vertical movement
	if (input->isKeyDown('w'))
	{
		// Move camera right
		newPosY = camera.getPosY();
		camera.setPosY((newPosY + cameraSpeed));
		camera.update(0);

		// Reset key state
		input->SetKeyUp('w');
	}

	if (input->isKeyDown('s'))
	{
		// Move camera right
		newPosY = camera.getPosY();
		camera.setPosY((newPosY - cameraSpeed));
		camera.update(0);

		// Reset key state
		input->SetKeyUp('s');
	}

	// Forward/backward movement
	if (input->isKeyDown('q'))
	{
		// Move camera right
		newPosZ = camera.getPosZ();
		camera.setPosZ((newPosZ + cameraSpeed));
		camera.update(0);

		// Reset key state
		input->SetKeyUp('q');
	}

	if (input->isKeyDown('e'))
	{
		// Move camera right
		newPosZ = camera.getPosZ();
		camera.setPosZ((newPosZ - cameraSpeed));
		camera.update(0);

		// Reset key state
		input->SetKeyUp('e');
	}


	// MOUSE CAMERA ROTATION

	midWindowX = width / 2;
	midWindowY = height / 2;

	// Get the postion of the mouse to figure out the difference in movement

	horizMovement = input->getMouseX() - midWindowX;
	vertMovement = input->getMouseY() - midWindowY;

	// Horizontal movement
	if (horizMovement > 0 && horizMovement > vertMovement)
	{
		// Rotate camera right
		newRotY = camera.getRotY();
		camera.setRotY((newRotY + speed));
		camera.update(0);
	}

	else if (horizMovement < 0 && horizMovement < vertMovement)
	{
		// Rotate camera left
		newRotY = camera.getRotY();
		camera.setRotY((newRotY - speed));
		camera.update(0);
	}

	// Vertical movement

	else if (vertMovement < 0 && vertMovement < horizMovement)
	{
		// Rotate camera up
		newRotX = camera.getRotX();
		camera.setRotX((newRotX + speed));
		camera.update(0);
	}

	else if (vertMovement > 0 && vertMovement > horizMovement)
	{
		// Rotate camera up
		newRotX = camera.getRotX();
		camera.setRotX((newRotX - speed));
		camera.update(0);
	}
	
	// Warps the mouse back to the middle of the screen
	glutWarpPointer(400, 300);

	// ------------------------------------------------------

	// Calculate FPS
	frame++;
	time = glutGet(GLUT_ELAPSED_TIME);

	if (time - timebase > 1000) {
		sprintf_s(fps, "FPS: %4.2f", frame*1000.0 / (time - timebase));
		timebase = time;
		frame = 0;
	}


}


void Scene::render() 
{

	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();

	// ---------------Lighting variables ------------


	// Light 1 - Point light
	GLfloat Light_Ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat Light_Diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat Light_Position[] = { 0.0f, 15.0f, 0.0f, 1.0f };

	// Light 2 - Spot light
	GLfloat Light_Ambient2[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat Light_Diffuse2[] = { 0.3f, 1.0f, 0.3f, 1.0f };
	GLfloat Light_Position2[] = { -5.0f, 3.0f, 0.0f, 1.0f };
	GLfloat spot_Direction2[] = { 0.0f, -1.0f, 0.0f };


	// ----------------------------------------------

	// Set the camera (Modified to use the camera class)
	gluLookAt(camera.getPosX(), camera.getPosY(), camera.getPosZ(), camera.getLookX(), camera.getLookY(), camera.getLookZ(), camera.getUpX(), camera.getUpY(), camera.getUpZ());

	// Render geometry here -------------------------------------

	// Creates light 1
	glLightfv(GL_LIGHT0, GL_AMBIENT, Light_Ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, Light_Diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, Light_Position);
	glEnable(GL_LIGHT0);


	// Create light 2
	glLightfv(GL_LIGHT1, GL_AMBIENT, Light_Ambient2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, Light_Diffuse2);
	glLightfv(GL_LIGHT1, GL_POSITION, Light_Position2);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 25.0f);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_Direction2);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 50.0);
	glEnable(GL_LIGHT1);


	// Calls the skybox rendering function
	renderSkybox();




	// ----------------- Stencil buffer stuff ------------------

	// Turn off writing to the frame buffer
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	// Enables stencil test
	glEnable(GL_STENCIL_TEST);

	// Set the stencil function to always pass
	glStencilFunc(GL_ALWAYS, 1, 1);

	// Set the stencil operation to replace values when the test passes
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// Disable the depth test (we don't want to store depth values when writing to the stencil buffer)
	glDisable(GL_DEPTH_TEST);



	// Draw floor object (the object on the stencil)
	glBegin(GL_QUADS);
		glVertex3f(-2, 0, -4); // TL
		glVertex3f(2, 0, -4); // TR
		glVertex3f(2, 0, 4); // BR
		glVertex3f(-2, 0, 4); // BL
	glEnd();

	// Enables depth test
	glEnable(GL_DEPTH_TEST);

	// Turn on rendering to the frame buffer
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	// Set stencil function to test if the value is 1 
	glStencilFunc(GL_EQUAL, 1, 1);

	// Set the stencil operation to keep all the values (we don't want to change the stencil)
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);


	glPushMatrix();
		glScalef(1.0, -1.0, 1.0);		// Flip the scale vertically
		glTranslatef(0, 1, 0);			// Translate down (Put us under the floor)
		glRotatef(rotation, 0, 1, 0);		// Rotate the shape so it's spinning
		renderSolarSystem();			// Renders the solar system into the program (reflection)
	glPopMatrix();


	// Disable the stencil test
	glDisable(GL_STENCIL_TEST);

	// Enable alpha blending
	glEnable(GL_BLEND);

	// Disable lighting (100% reflective object)
	glDisable(GL_LIGHTING);

	// Set the colour of the floor object
	glColor4f(0.7f, 0.7f, 1.0f, 0.5f);

	// Draw floor object (visual representation of the stencil buffer)
	glBegin(GL_QUADS);
		glVertex3f(-2, 0, -4); // TL
		glVertex3f(2, 0, -4); // TR
		glVertex3f(2, 0, 4); // BR
		glVertex3f(-2, 0, 4); // BL
	glEnd();

	// Enable lighting (rest of the scene is lit correctly)
	glEnable(GL_LIGHTING);

	// Disable blending
	glDisable(GL_BLEND);

	glPushMatrix();
		glTranslatef(0, 1, 0);				// Position the model in the correct place
		glRotatef(rotation, 0, 1, 0);		// Rotates the model
		renderSolarSystem();				// Renders the model that is loaded into the program (actual object)
	glPopMatrix();


	// ----------------- End of Stencil buffer stuff ------------------



	// ----------------- Shadow stuff ------------------


	
	// Floor vertices
	GLfloat floorVerts[] = { -1.0f, -1.0f, -1.0f,//top left
							-1.0f, -1.0f, 1.0f,// bottom left
							1.0f, -1.0f, 1.0f,//bottom right
							1.0f, -1.0f, -1.0f };// top right


	// Calls the shadow matrix generation
	generateShadowMatrix(Light_Position, floorVerts);


	// MODEL RENDERING - SHADOW

	glBindTexture(GL_TEXTURE_2D, floor);

	// Render the floor

	glPushMatrix();
		
		// Shift over to the correct position
		glTranslatef(-11, 2.01, 1);

			// Creates the floor for the shadow
			glBegin(GL_QUADS);
				glNormal3f(0.0f, 1.0f, 0.0f);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(-1.0f, -1.0f, -1.0f); //TL

				glNormal3f(0.0f, 1.0f, 0.0f);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(-1.0f, -1.0f, 1.0f);	//BL

				glNormal3f(0.0f, 1.0f, 0.0f);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(1.0f, -1.0f, 1.0f);	//BR

				glNormal3f(0.0f, 1.0f, 0.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(1.0f, -1.0f, -1.0f); //TR
			glEnd();
	

		// Disables for shadow generating
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);


		// Shadow's colour
		glColor3f(0.1f, 0.1f, 0.1f);

		// Render the shadow
		glPushMatrix();
			glMultMatrixf((GLfloat *)shadowMatrix);
			// Translate to floor and draw shadow, remember to match any transforms on the object
			glScalef(0.3f, 0.3f, 0.3f);
			glTranslatef(0.0f, 1.5f, 0.0f);
			glRotatef(rotation, 0.0f, 1.0f, 0.0f);
			model.render();
		glPopMatrix();

		glColor3f(1.0f, 1.0f, 1.0f);

		// Enables again
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);

		
		glBindTexture(GL_TEXTURE_2D, ice);

		// Render the model
		glPushMatrix();
			glScalef(0.3f, 0.3f, 0.3f);
			glTranslatef(0.0f, 1.5f, 0.0f);
			glRotatef(rotation, 0.0f, 1.0f, 0.0f);
			model.render();
		glPopMatrix();

	glPopMatrix();


	// --------- Shadow generation for the procedural shapes ------------

	glBindTexture(GL_TEXTURE_2D, floor);

	glPushMatrix();
		
		// Shifts the generation over to the correct position
		glTranslatef(-11, 2.01, -7);

			glBegin(GL_QUADS);
				glNormal3f(0.0f, 1.0f, 0.0f);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(-2.5f, -1.0f, -2.5f); //TL

				glNormal3f(0.0f, 1.0f, 0.0f);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(-2.5f, -1.0f, 2.5f);	//BL

				glNormal3f(0.0f, 1.0f, 0.0f);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(2.5f, -1.0f, 2.5f);	//BR

				glNormal3f(0.0f, 1.0f, 0.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(2.5f, -1.0f, -2.5f); //TR
			glEnd();
	

		// Disables for shadow generating
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);

		// Shadow's colour
		glColor3f(0.1f, 0.1f, 0.1f);

		// Render the shadow
		glPushMatrix();
			glMultMatrixf((GLfloat *)shadowMatrix);
			// Translate to floor and draw shadow, remember to match any transforms on the object
			glTranslatef(0.0f, 1.5f, 0.0f);
			glRotatef(rotation, 0.0f, 1.0f, 1.0f);
			// Renders shape based on current selection
			shapeSelection();
		glPopMatrix();

		glColor3f(1.0f, 1.0f, 1.0f);

		// Enables again
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);

		
		glBindTexture(GL_TEXTURE_2D, soil);

		// Render the shape
		glPushMatrix();
			glTranslatef(0.0f, 1.5f, 0.0f);
			glRotatef(rotation, 0.0f, 1.0f, 1.0f);
			// Renders shape based on current selection
			shapeSelection();
		glPopMatrix();

	glPopMatrix();
	
	// ----------------- End of Shadow stuff ------------------

	

	// Generating the floor

	glPushMatrix();
		glTranslatef(-5, 0, -12);
		glBindTexture(GL_TEXTURE_2D, floor);


		for (int j = 0; j < 5; j++)
		{
			// Shift the cube to the left
			glTranslatef(-2, 0, 20);

			for (int i = 0; i < 10; i++)
			{
				// Shift the cube down
				glTranslatef(0, 0, -2);
				// Render a cube
				shape.render2();
			}
		}
	glPopMatrix();















	// Geometry rendering ends here -----------------------------

	// Render text, should be last object rendered.
	renderTextOutput();
	
	// Swap buffers, after all objects are rendered.
	glutSwapBuffers();
}


// ------------ Shape selection functions ------------------

void Scene::shapeSelection()
{
	switch (shapeCounter)
	{
	case 0:
		shape.render1();
		break;
	case 1:
		shape.render2();
		break;
	case 2:
		shape.render3();
		break;
	}
}

// ----------- End of Shape selection functions ------------





// ------------ Renders the Solar system -----------------

void Scene::renderSolarSystem()
{

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	// Create a solar system

	glPushMatrix();// Remember where we are.  THE SUN
		// Render the sun
		glColor3f(1.0f, 0.9f, 0.0f);
		gluSphere(gluNewQuadric(), 0.20, 20, 20);

		glPushMatrix();
			// Render planet 1
			glRotatef(rotation, 0, 1, 0);
			glTranslatef(1, 0.1, 0);
			glScalef(0.7, 0.7, 0.7);
			glColor3f(0.8f, 0.1f, 0.1f);
			gluSphere(gluNewQuadric(), 0.20, 20, 20);
			glPushMatrix(); // REMEMBER WHERE WE ARE
				// Render a moon around planet 1
				glRotatef((rotation2 * 1.8), 0, 1, 0);
				glTranslatef(0.5, 0, 0);
				glScalef(0.3, 0.3, 0.3);
				glColor3f(0.8f, 0.8f, 0.8f);
				gluSphere(gluNewQuadric(), 0.20, 20, 20);
			glPopMatrix();
		glPopMatrix(); // GO BACK TO SUN

		glPushMatrix(); // REMEMBER WHERE WE ARE
			// Render planet 2
			glRotatef(rotation2 * 0.8, 0, 1, 0);
			glTranslatef(1.0, -0.3, 0);
			glScalef(0.4, 0.4, 0.4);
			glColor3f(0.1f, 0.9f, 1.0f);
			gluSphere(gluNewQuadric(), 0.20, 20, 20);
			glPushMatrix(); // REMEMBER WHERE WE ARE
				// Render a moon around planet 2
				glRotatef((rotation * 5.0), 0, 1, 0);
				glTranslatef(0.7, 0, 0);
				glScalef(0.2, 0.2, 0.2);
				glColor3f(0.8f, 0.8f, 0.8f);
				gluSphere(gluNewQuadric(), 0.20, 20, 20);
			glPopMatrix();
		glPopMatrix(); // GO BACK TO SUN

	glPopMatrix();

	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

}


// ------------ End of Solar system -----------------




// ------------ Renders the Skybox ------------------

void Scene::renderSkybox()
{
	// Skybox
	glPushMatrix();
		glTranslatef(camera.getPosX(), camera.getPosY(), camera.getPosZ());
		glDisable(GL_DEPTH_TEST);


		glBindTexture(GL_TEXTURE_2D, skyboxFt);

		glBegin(GL_QUADS);
			// Front face
			glTexCoord2f(0.0f, 0.0f); // Top left
			glVertex3f(-0.5f, 0.5f, 0.5f);

			glTexCoord2f(1.0f, 0.0f); // Top right
			glVertex3f(0.5f, 0.5f, 0.5f);

			glTexCoord2f(1.0f, 1.0f); // Bottom right
			glVertex3f(0.5f, -0.5f, 0.5f);

			glTexCoord2f(0.0f, 1.0f); // Bottom left
			glVertex3f(-0.5f, -0.5f, 0.5f);
		glEnd();


		glBindTexture(GL_TEXTURE_2D, skyboxBk);

		glBegin(GL_QUADS);
			// Back face

			glTexCoord2f(0.0f, 0.0f); // Top left
			glVertex3f(0.5f, 0.5f, -0.5f);

			glTexCoord2f(1.0f, 0.0f); // Top right
			glVertex3f(-0.5f, 0.5f, -0.5f);

			glTexCoord2f(1.0f, 1.0f); // Bottom right
			glVertex3f(-0.5f, -0.5f, -0.5f);

			glTexCoord2f(0.0f, 1.0f); // Bottom left
			glVertex3f(0.5f, -0.5f, -0.5f);
		glEnd();


		glBindTexture(GL_TEXTURE_2D, skyboxLf);

		glBegin(GL_QUADS);
			// Right face

			glTexCoord2f(0.0f, 0.0f); // Top left
			glVertex3f(0.5f, 0.5f, 0.5f);

			glTexCoord2f(1.0f, 0.0f); // Top right
			glVertex3f(0.5f, 0.5f, -0.5f);

			glTexCoord2f(1.0f, 1.0f); // Bottom right
			glVertex3f(0.5f, -0.5f, -0.5f);

			glTexCoord2f(0.0f, 1.0f); // Bottom left
			glVertex3f(0.5f, -0.5f, 0.5f);
		glEnd();



		glBindTexture(GL_TEXTURE_2D, skyboxRt);

		glBegin(GL_QUADS);
			// Left face

			glTexCoord2f(0.0f, 0.0f); // Top left
			glVertex3f(-0.5f, 0.5f, -0.5f);

			glTexCoord2f(1.0f, 0.0f); // Top right
			glVertex3f(-0.5f, 0.5f, 0.5f);

			glTexCoord2f(1.0f, 1.0f); // Bottom right
			glVertex3f(-0.5f, -0.5f, 0.5f);

			glTexCoord2f(0.0f, 1.0f); // Bottom left
			glVertex3f(-0.5f, -0.5f, -0.5f);
		glEnd();


		glBindTexture(GL_TEXTURE_2D, skyboxUp);

		glBegin(GL_QUADS);
			// Top face

			glTexCoord2f(0.0f, 0.0f); // Top left
			glVertex3f(-0.5f, 0.5f, -0.5f);

			glTexCoord2f(0.0f, 1.0f); // Top right
			glVertex3f(0.5f, 0.5f, -0.5f);

			glTexCoord2f(1.0f, 1.0f); // Bottom right
			glVertex3f(0.5f, 0.5f, 0.5f);

			glTexCoord2f(1.0f, 0.0f); // Bottom left
			glVertex3f(-0.5f, 0.5f, 0.5f);
		glEnd();


		glBindTexture(GL_TEXTURE_2D, skyboxDn);

		glBegin(GL_QUADS);
			// Bottom face

			glTexCoord2f(0.0f, 0.0f); // Top left
			glVertex3f(0.5f, -0.5f, 0.5f);

			glTexCoord2f(1.0f, 0.0f); // Top right
			glVertex3f(0.5f, -0.5f, -0.5f);

			glTexCoord2f(1.0f, 1.0f); // Bottom right
			glVertex3f(-0.5f, -0.5f, -0.5f);

			glTexCoord2f(0.0f, 1.0f); // Bottom left
			glVertex3f(-0.5f, -0.5f, 0.5f);

		glEnd();

		glEnable(GL_DEPTH_TEST);
	glPopMatrix();

}

// ------------ End of the Skybox ------------------



// ------------ Loads in the textures ------------------

void Scene::setTextures()
{

	soil = SOIL_load_OGL_texture
	(
		"gfx/Soil.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_INVERT_Y // Depending on texture file type some need inverted others don't.
	);



	ice = SOIL_load_OGL_texture
	(
		"gfx/Ice.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_INVERT_Y // Depending on texture file type some need inverted others don't.
	);



	floor = SOIL_load_OGL_texture
	(
		"gfx/Sand.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_INVERT_Y // Depending on texture file type some need inverted others don't.
	);


	shackWall = SOIL_load_OGL_texture
	(
		"gfx/shackWall.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_INVERT_Y // Depending on texture file type some need inverted others don't.
	);


	// Front face
	skyboxFt = SOIL_load_OGL_texture
	(
		"gfx/thefog_ft.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT  // Depending on texture file type some need inverted others don't.
	);


	// Back face
	skyboxBk = SOIL_load_OGL_texture
	(
		"gfx/thefog_bk.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT  // Depending on texture file type some need inverted others don't.
	);

	// Right face
	skyboxRt = SOIL_load_OGL_texture
	(
		"gfx/thefog_rt.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT  // Depending on texture file type some need inverted others don't.
	);

	// Left face
	skyboxLf = SOIL_load_OGL_texture
	(
		"gfx/thefog_lf.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT  // Depending on texture file type some need inverted others don't.
	);


	// Top face
	skyboxUp = SOIL_load_OGL_texture
	(
		"gfx/thefog_up.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_INVERT_Y // Depending on texture file type some need inverted others don't.
	);


	// Bottom face
	skyboxDn = SOIL_load_OGL_texture
	(
		"gfx/thefog_dn.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_INVERT_Y // Depending on texture file type some need inverted others don't.
	);

}
// ------------ End of loading in the textures ------------------






// Generates a shadow matrix.
// The shadow matrix will transform an object into a 2D shadow of itself, based on the provide light position and floor plane.
// shadowMatrix[16]; must be declared
// Inputs: light_pos is the light position. floor is a vertex array with 4 vertices that draw a quad (defining the floor plane that the shadow will be drawn on)
void Scene::generateShadowMatrix(GLfloat light_pos[4], GLfloat floor[12]) {


	//Defining vertices of plane are PQR with edges PQ and PR
	Vector3 P(floor[0], floor[1], floor[2]);	//top left
	Vector3 Q(floor[3], floor[4], floor[5]);	// bottom left
	Vector3 R(floor[9], floor[10], floor[11]);	// top right

	Vector3 PQ = (Q - P).normalised();
	Vector3 PR = (R - P).normalised();
	Vector3 normal = PR.cross(PQ);

	//Equation of plane is ax + by + cz = d
	//a, b and c are the coefficients of the normal to the plane (i.e. normal = ai + bj + ck)
	//If (x0, y0, z0) is any point on the plane, d = a*x0 + b*y0 + c*z0
	//i.e. d is the dot product of any point on the plane (using P here) and the normal to the plane
	float a, b, c, d;
	a = normal.getX();
	b = normal.getY();
	c = normal.getZ();
	d = normal.dot(P);

	//Origin of projection is at x, y, z. Projection here originating from the light source's position
	float x, y, z;
	x = light_pos[0];
	y = light_pos[1];
	z = light_pos[2];

	//This is the general perspective transformation matrix from a point (x, y, z) onto the plane ax + by + cz = d
	shadowMatrix[0] = d - (b * y + c * z);
	shadowMatrix[1] = a * y;
	shadowMatrix[2] = a * z;
	shadowMatrix[3] = a;

	shadowMatrix[4] = b * x;
	shadowMatrix[5] = d - (a * x + c * z);
	shadowMatrix[6] = b * z;
	shadowMatrix[7] = b;

	shadowMatrix[8] = c * x;
	shadowMatrix[9] = c * y;
	shadowMatrix[10] = d - (a * x + b * y);
	shadowMatrix[11] = c;

	shadowMatrix[12] = -d * x;
	shadowMatrix[13] = -d * y;
	shadowMatrix[14] = -d * z;
	shadowMatrix[15] = -(a * x + b * y + c * z);
}



void Scene::resize(int w, int h) 
{
	width = w;
	height = h;
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	float ratio = (float)w / (float)h;
	fov = 45.0f;
	nearPlane = 0.1f;
	farPlane = 100.0f;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(fov, ratio, nearPlane, farPlane);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

void Scene::renderTextOutput()
{
	// Render current mouse position and frames per second.
	sprintf_s(mouseText, "Mouse: %i, %i", input->getMouseX(), input->getMouseY());
	displayText(-1.f, 0.96f, 1.f, 0.f, 0.f, mouseText);
	displayText(-1.f, 0.90f, 1.f, 0.f, 0.f, fps);
}

void Scene::displayText(float x, float y, float r, float g, float b, char* string) {
	// Get Lenth of string
	int j = strlen(string);

	// Swap to 2D rendering
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, 5, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Orthographic lookAt (along the z-axis).
	gluLookAt(0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Set text colour and position.
	glColor3f(r, g, b);
	glRasterPos2f(x, y);
	// Render text.
	for (int i = 0; i < j; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
	}
	// Reset colour to white.
	glColor3f(1.f, 1.f, 1.f);

	// Swap back to 3D rendering.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, ((float)width/(float)height), nearPlane, farPlane);
	glMatrixMode(GL_MODELVIEW);
}
