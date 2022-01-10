#include "lab_work_3.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "utils/random.hpp"
#include "utils/read_file.hpp"
#include "common/camera/trackballCamera.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork3::_shaderFolder = "src/lab_works/lab_work_3/shaders/";

	LabWork3::~LabWork3()
	{
		delete _camera;
		glDeleteProgram( _program );
	}

	bool LabWork3::init()
	{
		std::cout << "Initializing lab work 3..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		glEnable( GL_DEPTH_TEST );

		if ( !_initProgram() )
			return false;

		_initCamera();

		_cube = _createCube();
		_cube._transformation = glm::scale( _cube._transformation, glm::vec3( 0.8f, 0.8f, 0.8f ) );
		_sat  = _createCube();
		_sat._transformation  = glm::translate( _sat._transformation, glm::vec3( 5.f, 0.f, 0.f ) );
		_initBuffers();

		glUseProgram( _program );

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork3::animate( const float p_deltaTime ){
		_cube._transformation = glm::rotate( _cube._transformation, p_deltaTime, glm::vec3( 0.f, 1.f, 1.f ) );
		_sat._transformation  = glm::translate( _sat._transformation, glm::vec3( -5.f, 0.f, 0.f ) );
		_sat._transformation = glm::rotate( _sat._transformation, p_deltaTime, glm::vec3( 0.8f, 0.5f, 1.f) );
		_sat._transformation  = glm::translate( _sat._transformation, glm::vec3( 5.f, 0.f, 0.f ) );
	}

	void LabWork3::render(){
		// Clear the color buffer.
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glProgramUniformMatrix4fv( _program, _uModelMatrixLoc, 1, false, glm::value_ptr( _cube._transformation ) );
		glBindVertexArray( _cube._vao );
		glDrawElements( GL_TRIANGLES, _cube._indices.size(), GL_UNSIGNED_INT, 0 );

		
		glProgramUniformMatrix4fv( _program, _uModelMatrixLoc, 1, false, glm::value_ptr( _sat._transformation ) );
		glBindVertexArray( _sat._vao );
		glDrawElements( GL_TRIANGLES, _sat._indices.size(), GL_UNSIGNED_INT, 0 );

		glBindVertexArray( 0 );
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
	}

	void LabWork3::handleEvents( const SDL_Event & p_event )
	{
		if ( p_event.type == SDL_KEYDOWN )
		{
			switch ( p_event.key.keysym.scancode )
			{
			case SDL_SCANCODE_W: // Front
				_camera->moveFront( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_S: // Back
				_camera->moveFront( -_cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_A: // Left
				_camera->moveRight( -_cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_D: // Right
				_camera->moveRight( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_R: // Up
				_camera->moveUp( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_F: // Bottom
				_camera->moveUp( -_cameraSpeed );
				_updateViewMatrix();
				break;
			default: break;
			}
		}

		// Rotate when left click + motion (if not on Imgui widget).
		if ( p_event.type == SDL_MOUSEMOTION && p_event.motion.state & SDL_BUTTON_LMASK
			 && !ImGui::GetIO().WantCaptureMouse )
		{
			_camera->rotate( p_event.motion.xrel * _cameraSensitivity, p_event.motion.yrel * _cameraSensitivity );
			_updateViewMatrix();
		}
	}

	void LabWork3::displayUI()
	{
		ImGui::Begin( "Settings lab work 3" );

		// Background.
		if ( ImGui::ColorEdit3( "Background", glm::value_ptr( _bgColor ) ) ){
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		}

		// Camera.
		if ( ImGui::SliderFloat( "fovy", &_fovy, 10.f, 160.f, "%01.f" ) ){
			_camera->setFovy( _fovy );
			_updateProjectionMatrix();
		}

		if ( ImGui::SliderFloat( "Speed", &_cameraSpeed, 0.1f, 10.f, "%01.1f" ) ){
			_camera->setFovy( _fovy );
			_updateProjectionMatrix();
		}

		if ( ImGui::RadioButton( "perspective", _camera->getCameraMode() == cameraMode::perspective ) ){
			_camera->setCameraMode( cameraMode::perspective );
			_updateProjectionMatrix();
		}
		ImGui::SameLine();
		if ( ImGui::RadioButton( "ortho", _camera->getCameraMode() == cameraMode::ortho ) ){
			_camera->setCameraMode( cameraMode::ortho );
			_updateProjectionMatrix();
		}

		if ( ImGui::RadioButton( "free fly", _freefly ) )
		{
			_freefly = true;
			delete _camera;
			_camera = new FreeflyCamera();
			_initCamera();
		}
		ImGui::SameLine();
		if ( ImGui::RadioButton( "track ball", !_freefly) )
		{
			_freefly = false;
			delete _camera;
			_camera = new TrackballCamera();
			_initCamera();
		}

		ImGui::End();
	}

	void LabWork3::resize( const int p_width, const int p_height )
	{
		BaseLabWork::resize( p_width, p_height );
		_camera->setScreenSize( p_width, p_height );
	}

	bool LabWork3::_initProgram()
	{
		// ====================================================================
		// Shaders.
		// ====================================================================
		// Create shaders.
		const GLuint vertexShader	= glCreateShader( GL_VERTEX_SHADER );
		const GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

		// Get sources from files.
		const std::string vertexShaderSrc	= readFile( _shaderFolder + "triangle.vert" );
		const std::string fragmentShaderSrc = readFile( _shaderFolder + "triangle.frag" );

		// Convert to GLchar *
		const GLchar * vSrc = vertexShaderSrc.c_str();
		const GLchar * fSrc = fragmentShaderSrc.c_str();

		// Compile vertex shader.
		glShaderSource( vertexShader, 1, &vSrc, NULL );
		glCompileShader( vertexShader );
		// Check if compilation is ok.
		GLint compiled;
		glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( vertexShader, sizeof( log ), NULL, log );
			glDeleteShader( vertexShader );
			glDeleteShader( fragmentShader );
			std::cerr << "Error compiling vertex shader: " << log << std::endl;
			return false;
		}

		// Compile vertex shader.
		glShaderSource( fragmentShader, 1, &fSrc, NULL );
		glCompileShader( fragmentShader );
		// Check if compilation is ok.
		glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &compiled );
		if ( compiled != GL_TRUE )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( fragmentShader, sizeof( log ), NULL, log );
			glDeleteShader( vertexShader );
			glDeleteShader( fragmentShader );
			std::cerr << "Error compiling fragment shader: " << log << std::endl;
			return false;
		}
		// ====================================================================

		// ====================================================================
		// Program.
		// ====================================================================
		// Create program.
		_program = glCreateProgram();

		// Attach shaders.
		glAttachShader( _program, vertexShader );
		glAttachShader( _program, fragmentShader );

		// Link program.
		glLinkProgram( _program );
		// Check if link is ok.
		GLint linked;
		glGetProgramiv( _program, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( _program, sizeof( log ), NULL, log );
			std::cerr << "Error linking program: " << log << std::endl;
			return false;
		}

		// Shaders are now useless.
		glDeleteShader( vertexShader );
		glDeleteShader( fragmentShader );
		// ====================================================================

		// ====================================================================
		// Get uniform locations.
		// ====================================================================
		_uModelMatrixLoc	  = glGetUniformLocation( _program, "uModelMatrix" );
		_uViewMatrixLoc		  = glGetUniformLocation( _program, "uViewMatrix" );
		_uProjectionMatrixLoc = glGetUniformLocation( _program, "uProjectionMatrix" );
		// ====================================================================

		return true;
	}

	void LabWork3::_initCamera(){
		_camera->setScreenSize( _windowWidth, _windowHeight );
		_camera->setPosition( Vec3f( 0.f, 0.f, 10.f ) );
		_camera->setLookAt( Vec3f( 0.f, 0.f, 0.f ) );
		_updateViewMatrix();
		_updateProjectionMatrix();
	}

	void LabWork3::_initBuffers(){
		_initBuffer( &_cube );
		_initBuffer( &_sat );
	}

	void LabWork3::_initBuffer(Mesh* mesh){
		glCreateBuffers( 1, &(*mesh)._ebo );
		glNamedBufferData(
			(*mesh)._ebo, (*mesh)._indices.size() * sizeof( unsigned int ), (*mesh)._indices.data(), GL_STATIC_DRAW );

		glCreateBuffers( 1, &(*mesh)._vboPositions );
		glNamedBufferData(
			(*mesh)._vboPositions, (*mesh)._vertices.size() * sizeof( Vec3f ), (*mesh)._vertices.data(), GL_STATIC_DRAW );

		glCreateBuffers( 1, &(*mesh)._vboColors );
		glNamedBufferData(
			(*mesh)._vboColors, (*mesh)._vertexColors.size() * sizeof( Vec3f ), (*mesh)._vertexColors.data(), GL_STATIC_DRAW );

		glCreateVertexArrays( 1, &(*mesh)._vao );
		glEnableVertexArrayAttrib( (*mesh)._vao, 0 );
		glVertexArrayAttribFormat( (*mesh)._vao, 0, 3, GL_FLOAT, GL_FALSE, 0 );
		glEnableVertexArrayAttrib( (*mesh)._vao, 1 );
		glVertexArrayAttribFormat( (*mesh)._vao, 1, 3, GL_FLOAT, GL_FALSE, 0 );

		glVertexArrayElementBuffer( (*mesh)._vao, (*mesh)._ebo );
		glVertexArrayVertexBuffer( (*mesh)._vao, 0, (*mesh)._vboPositions, 0, sizeof( Vec3f ) );
		glVertexArrayAttribBinding( (*mesh)._vao, 0, 0 );
		glVertexArrayVertexBuffer( (*mesh)._vao, 1, (*mesh)._vboColors, 0, sizeof( Vec3f ) );
		glVertexArrayAttribBinding( (*mesh)._vao, 1, 1 );
	}

	void LabWork3::_updateViewMatrix(){
		glProgramUniformMatrix4fv(_program, _uViewMatrixLoc, 1, false, glm::value_ptr(_camera->getViewMatrix()) );	
	}

	void LabWork3::_updateProjectionMatrix(){
		glProgramUniformMatrix4fv(_program, _uProjectionMatrixLoc, 1, false, glm::value_ptr(_camera->getProjectionMatrix()) );
	}

	LabWork3::Mesh LabWork3::_createCube()
	{		
		Mesh mesh = Mesh();
		mesh._vertices = { Vec3f( -0.5, 0.5, -0.5 ) , Vec3f( -0.5, 0.5, 0.5 ),
						   Vec3f( 0.5, 0.5, 0.5 )   , Vec3f( 0.5, 0.5, -0.5 ),
						   Vec3f( -0.5, -0.5, -0.5 ), Vec3f( -0.5, -0.5, 0.5 ),
						   Vec3f( 0.5, -0.5, 0.5 )  , Vec3f( 0.5, -0.5, -0.5)};
		for ( int i = 0; i < mesh._vertices.size(); i++ ){mesh._vertexColors.push_back( getRandomVec3f() );}
		mesh._indices = {0,1,2  ,  0,2,3  ,  4,5,6  ,  4,6,7  ,  0,1,4  ,  4,1,5  ,  3,2,7  ,  7,6,2  ,  0,4,3  ,  4,3,7  ,  1,2,6  ,  1,5,6};
		return mesh;
	}
} // namespace M3D_ISICG
