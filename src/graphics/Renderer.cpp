#include "Renderer.h"

#include "core/WindowManager.h"

Renderer Renderer::singleton;

void Renderer::Init()
{
	FBOMANAGER->bind_default_fbo();
	glClearColor(0.3f, 0.3f, 0.3f, 1);

	MeshHandler::get_singleton()->Init();
	Sprite::Init();
	Primitives::Init();

	render_buffer = new FBO2D(WINDOWSIZE);
	render_buffer->add_color_texture();
	render_buffer->add_depth_texture();
	render_buffer->init();

	shadow_buffer = new FBO2D(1024);
	shadow_buffer->add_depth_texture();
	shadow_buffer->init();

	reflection_buffer = new FBO2D(WINDOWSIZE);
	reflection_buffer->add_color_texture();
	reflection_buffer->add_depth_texture();
	reflection_buffer->init();

	//use_depth_test(0.5f, 1000.0f);
	//use_culling();
	use_blending();
}

void Renderer::Resize(int width, int height)
{
	T_INFO("Resizing: (" + StringUtils::IntToString(width) + ", " + StringUtils::IntToString(height) + ")");

	WINDOW->size = vec2i(width, height);

	glViewport(0, 0, width, height);
}

void Renderer::CheckGLError()
{
	GLenum err = GL_NO_ERROR;
	while ((err = glGetError()) != GL_NO_ERROR)
		T_ERROR("OpenGL error: " + std::to_string(err));
}

void Renderer::Prepare()
{
	CheckGLError();

	FBOMANAGER->clear_all();
	//render_buffer->bind();
}

void Renderer::Finish()
{
	//VISUALEFFECT->post_process();
}

void Renderer::Free()
{
	delete viseffect;
}

void Renderer::switch_to_camera(Camera * p_camera)
{
	projection_matrix = mat4(p_camera->get_projection_matrix());
	view_matrix = mat4(p_camera->get_view_matrix());
	final_matrix = mat4(p_camera->get_final_matrix());

	projections.push(projection_matrix);
	views.push(view_matrix);
}

void Renderer::activate_projection_matrix(const mat4& p_projection)
{
	mat4 copy = p_projection;

	projections.push(copy);
	projection_matrix = copy;
	update();
}

void Renderer::activate_view_matrix(const mat4& p_view_matrix)
{
	mat4 copy = p_view_matrix;

	views.push(copy);
	view_matrix = copy;
	update();
}

void Renderer::deactivate_camera()
{
	projections.pop();
	views.pop();

	if (projections.size() > 0)
		projection_matrix = projections.get_top();

	if (views.size() > 0)
		view_matrix = views.get_top();

	update();
}

void Renderer::deactivate_projection_matrix()
{
	projections.pop();

	if (projections.size() > 0)
		projection_matrix = projections.get_top();

	update();
}

void Renderer::deactivate_view_matrix()
{
	views.pop();

	if (views.size() > 0)
		view_matrix = views.get_top();

	update();
}

const mat4 & Renderer::get_projection_matrix() const
{
	return projection_matrix;
}

const mat4 & Renderer::get_view_matrix() const
{
	return view_matrix;
}

const mat4& Renderer::get_final_matrix() const
{
	return final_matrix;
}

void Renderer::set_light_matrix(const mat4& p_light_matrix)
{
	light_matrix = p_light_matrix;
}

const mat4& Renderer::get_light_matrix() const
{
	return light_matrix;
}

void Renderer::use_scissor(const rect2 &area)
{
	glEnable(GL_SCISSOR_TEST);

	glScissor(
		WINDOWSIZE.x / 2 + (int)area.get_bottom_left().x,
		WINDOWSIZE.y / 2 + (int)area.get_bottom_left().y,
		(int)area.size.x * 2,
		(int)area.size.y * 2);
}
void Renderer::stop_scissor()
{
	glDisable(GL_SCISSOR_TEST);
}

void Renderer::use_depth_test(float p_near, float p_far)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthRange(p_near, p_far);
}

void Renderer::stop_depth_test()
{
	glDisable(GL_DEPTH_TEST);
}

void Renderer::use_culling()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void Renderer::stop_culling()
{
	glDisable(GL_CULL_FACE);
}

void Renderer::use_blending()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::stop_blending()
{
	glDisable(GL_BLEND);
}

void Renderer::use_wireframe()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
}

void Renderer::fill()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

FBO2D* Renderer::get_shadow_buffer() const
{
	return shadow_buffer;
}

FBO2D* Renderer::get_render_buffer() const
{
	return render_buffer;
}

FBO2D* Renderer::get_reflection_buffer() const
{
	return reflection_buffer;
}

Renderer* Renderer::get_singleton()
{
	return &singleton;
}

#undef CLASSNAME
#define CLASSNAME Renderer

void Renderer::bind_methods()
{
	REG_SINGLETON(get_singleton());
}

void Renderer::update()
{
	final_matrix = projection_matrix * view_matrix;
}
