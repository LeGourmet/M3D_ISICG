#ifndef __LAB_WORK_2_HPP__
#define __LAB_WORK_2_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWork2 : public BaseLabWork
	{
	  public:
		LabWork2() : BaseLabWork() {}
		~LabWork2();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

		void createCircle(Vec2f center, int nb, float rayon);

	  private:
		// ================ Scene data.
		float  _vLux;
		int	   _nbPts;
		
		std::vector<int>   _indices;
		std::vector<Vec2f> _pts;
		std::vector<Vec3f> _colors;
		// ================

		// ================ GL data.
		GLuint			   _program;
		GLuint			   _VBO_POS;
		GLuint			   _VBO_COL;
		GLuint			   _VAO;
		GLuint			   _EBO;

		GLint _uTranslationX;
		GLint _uLux;
		// ================

		// ================ Settings.
		float _time;
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		static const std::string _shaderFolder;
		// ================
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_2_HPP__
