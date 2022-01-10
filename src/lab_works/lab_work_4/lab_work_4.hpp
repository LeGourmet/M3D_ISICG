#ifndef __LAB_WORK_4_HPP__
#define __LAB_WORK_4_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/camera/baseCamera.hpp"
#include "common/camera/freeflyCamera.hpp"
#include "common/camera/trackballCamera.hpp"
#include "common/models/triangle_mesh_model.hpp"
#include "define.hpp"
#include <map>
#include <vector>

namespace M3D_ISICG
{
	enum class model_LB4{ bunny, conference };

	class LabWork4 : public BaseLabWork
	{
	  public:
		LabWork4() : BaseLabWork()
		{
			_camera	 = new FreeflyCamera();
			_freefly = true;
		}
		~LabWork4();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

		void resize( const int p_width, const int p_height ) override;

	  private:
		bool _initProgram();
		void _initCamera();

	  private:
		// ================ Scene data.
		TriangleMeshModel _scene;
		model_LB4		  _model  = model_LB4::bunny;
		BaseCamera *	  _camera = nullptr;
		bool			  _freefly;

		// ================

		// ================ GL data.
		GLuint _program				  = GL_INVALID_INDEX;
		GLint  _uMVPMatrixLoc		  = GL_INVALID_INDEX;
		GLint  _uNormalMatrix		  = GL_INVALID_INDEX;
		GLint  _uMVMatrix			  = GL_INVALID_INDEX;
		// ================

		// ================ Settings.
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		// Camera
		float _cameraSpeed		 = 0.1f;
		float _cameraSensitivity = 0.1f;
		float _fovy				 = 60.f;
		// ================

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_4_HPP__
