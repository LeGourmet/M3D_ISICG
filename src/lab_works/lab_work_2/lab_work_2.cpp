#include "imgui.h"
#include "lab_work_2.hpp"
#include "utils/read_file.hpp"
#include <iostream>
#include "glm/gtc/type_ptr.hpp"
#include "utils/random.hpp"

namespace M3D_ISICG
{
	const std::string LabWork2::_shaderFolder = "src/lab_works/lab_work_2/shaders/";

	LabWork2::~LabWork2()
	{
		glDeleteProgram( _program );
		glDeleteBuffers( 1, &_VBO_POS );
		glDeleteBuffers( 1, &_VBO_COL );
		glDeleteBuffers( 1, &_EBO );
		glDisableVertexArrayAttrib( _VAO, 0 );
		glDeleteVertexArrays( 1, &_VAO );
	}

	bool LabWork2::init()
	{
		std::cout << "Initializing lab work 1..." << std::endl;

		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		/* -------------------------------------- SHADER -------------------------------------- */
		const GLuint vertex	  = glCreateShader( GL_VERTEX_SHADER );
		const GLuint fragment = glCreateShader( GL_FRAGMENT_SHADER );

		const std::string vertexShaderStr	= readFile( _shaderFolder + "lw2.vert" );
		const std::string fragmentShaderStr = readFile( _shaderFolder + "lw2.frag" );

		const GLchar * vSrc = vertexShaderStr.c_str();
		const GLchar * fSrc = fragmentShaderStr.c_str();

		glShaderSource( vertex, 1, &vSrc, NULL );
		glShaderSource( fragment, 1, &fSrc, NULL );

		glCompileShader( vertex );
		glCompileShader( fragment );

		// Check if compilation is ok.
		GLint compiled;
		glGetShaderiv( vertex, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( vertex, sizeof( log ), NULL, log );
			glDeleteShader( vertex );
			glDeleteShader( fragment );
			std ::cerr << " Error compiling vertex shader : " << log << std ::endl;
			return false;
		}

		/* -------------------------------------- PROGRAM -------------------------------------- */
		_program = glCreateProgram();
		glAttachShader( _program, vertex );
		glAttachShader( _program, fragment );
		glLinkProgram( _program );

		glDeleteShader( vertex );
		glDeleteShader( fragment );

		// Check if link is ok.
		GLint linked;
		glGetProgramiv( _program, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( _program, sizeof( log ), NULL, log );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}

		/* -------------------------------------- DATA -------------------------------------- */
		_nbPts = 10;
		createCircle( Vec2f( 0.f, 0.f ), _nbPts, 0.2f );
		_uTranslationX = glGetUniformLocation( _program, "uTranslationX" );
		_time					   = 0.f;
		_uLux = glGetUniformLocation( _program, "uLux" );
		_vLux					   = 1.f;

		glCreateBuffers( 1, &_EBO );
		glNamedBufferData( _EBO, _indices.size()*sizeof( unsigned int ), _indices.data(), GL_STREAM_DRAW );

		glCreateBuffers( 1, &_VBO_POS );
		glNamedBufferData( _VBO_POS, _pts.size()*sizeof( Vec2f ), _pts.data(), GL_STREAM_DRAW );

		glCreateBuffers( 1, &_VBO_COL );
		glNamedBufferData( _VBO_COL, _colors.size()*sizeof( Vec3f ), _colors.data(), GL_STREAM_DRAW );

		glCreateVertexArrays( 1, &_VAO );
		glEnableVertexArrayAttrib( _VAO, 0 );
		glVertexArrayAttribFormat( _VAO, 0, 2, GL_FLOAT, GL_FALSE, 0 );
		glEnableVertexArrayAttrib( _VAO, 1);
		glVertexArrayAttribFormat( _VAO, 1, 3, GL_FLOAT, GL_FALSE, 0 );

		glVertexArrayElementBuffer( _VAO, _EBO );
		glVertexArrayVertexBuffer( _VAO, 0, _VBO_POS, 0, sizeof( Vec2f ) );
		glVertexArrayAttribBinding( _VAO, 0, 0 );
		glVertexArrayVertexBuffer( _VAO, 1, _VBO_COL, 0, sizeof( Vec3f ) );
		glVertexArrayAttribBinding( _VAO, 1, 1 );

		glUseProgram( _program );

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork2::animate( const float p_deltaTime ) { 
		_time += p_deltaTime;
		glProgramUniform1f( _program, _uTranslationX, glm::sin( _time ) );
	}

	void LabWork2::render()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glBindVertexArray( _VAO );
		glDrawElements( GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0 );
		glBindVertexArray( 0 );

		glProgramUniform1f( _program, _uLux, _vLux );
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
	}

	void LabWork2::handleEvents( const SDL_Event & p_event ) {}

	void LabWork2::displayUI()
	{
		ImGui::Begin( "Settings lab work 2" );
		ImGui::SliderFloat( "Luminosite", &_vLux, 0.f, 1.f );
		ImGui::ColorEdit3( "couleur", glm::value_ptr(_bgColor) );
		if ( ImGui::SliderInt( "nombre de points", &_nbPts, 3, 50 ) ) {
			createCircle( Vec2f( 0.f, 0.f ), _nbPts, 0.2f );
			glNamedBufferData( _EBO, _indices.size() * sizeof( unsigned int ), _indices.data(), GL_STATIC_DRAW );
			glNamedBufferData( _VBO_POS, _pts.size() * sizeof( Vec2f ), _pts.data(), GL_STATIC_DRAW );
			glNamedBufferData( _VBO_COL, _colors.size() * sizeof( Vec3f ), _colors.data(), GL_STATIC_DRAW );
		}
		ImGui::End();
	}

	void LabWork2::createCircle(Vec2f center, int nb, float rayon) {
		_pts.clear();
		_colors.clear();
		_indices.clear();

		_pts.push_back( center );
		_colors.push_back( getRandomVec3f() );
		_pts.push_back( Vec2f( rayon+center.x, center.y) );
		_colors.push_back( getRandomVec3f() );
		for( unsigned int i=1; i<nb; i++ ){
			_indices.push_back( 0 );
			_indices.push_back( _pts.size() - 1 );

			_pts.push_back( Vec2f( rayon * glm::cos( ((2*3.14)/nb) * i ) + center.x,
								   rayon * glm::sin( ((2*3.14)/nb) * i ) + center.y) );

			_indices.push_back( _pts.size()-1 );
			_colors.push_back(getRandomVec3f());
		}
		_indices.push_back( 0 );
		_indices.push_back( 1 );
		_indices.push_back( _pts.size() - 1 );
	}

} // namespace M3D_ISICG
