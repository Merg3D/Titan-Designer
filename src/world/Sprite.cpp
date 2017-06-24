#include "Sprite.h"

#include "core/ContentManager.h"
#include "graphics\Renderer.h"

#include "resources/Texture.h"
#include "resources/Shader.h"
#include "Mesh.h"
#include "physics/PhysicsGod.h"

SimpleMesh * Sprite::defaultmesh;

Sprite::Sprite(Texture2D *tex, Shader *shader)	: Sprite(tex, vec2(tex->size.x, tex->size.y), shader) { }
Sprite::Sprite(Texture2D *tex, vec2 ts)			: Sprite(tex, ts, CONTENT->Shader2D) { }
Sprite::Sprite(Texture2D *tex)					: Sprite(tex, vec2(tex->size.x, tex->size.y)) { }
Sprite::Sprite(const String &tex_name)			: Sprite(CONTENT->LoadTexture(tex_name)) { }

Sprite::Sprite(Texture2D *tex, vec2 ts, Shader *shader)
{
	Sprite::texture = tex;
	Sprite::shader = shader;

	SetTileSize(ts);

	set_color(Color::White);
	bounds = vec4(0.0, 1.0, 0.0, 1.0);

	get_transform().set_size(vec3(tilesize, 0.0f));
	get_transform().update();
}

Sprite::~Sprite()
{
	
}

void Sprite::SetTileSize(const vec2 &size)
{
	tilesize = size;
	tilecount = { texture->size.x / tilesize.x, texture->size.y / tilesize.y };
	packed = tilesize.x != texture->size.x && tilesize.y != texture->size.y;
}

vec2 Sprite::GetTilePosition(int index) const
{
	return vec2(
		to_float(index % (int)tilecount.x), 
		to_float(index / tilecount.y));
}

vec4 Sprite::GetTileBounds(int index) const
{
	vec2 pos = GetTilePosition(index) * tilesize;
	return vec4(pos.x, pos.x + tilesize.x, pos.y + tilesize.y, pos.y) / vec4(vec2(texture->size.x), vec2(texture->size.y));
}

void Sprite::set_shader(Shader *p_shader)
{
	shader = p_shader;
}

Shader* Sprite::get_shader() const
{
	return shader;
}

void Sprite::set_texture(Texture2D *p_texture)
{
	texture = p_texture;
}

Texture2D* Sprite::get_texture() const
{
	return texture;
}

void Sprite::set_bounds(const vec4 & p_bounds)
{
	bounds = p_bounds;
}

vec4 Sprite::get_bounds() const
{
	return bounds;
}

void Sprite::flip_vertically()
{
	float temp = bounds.z;
	bounds.z = bounds.w;
	bounds.w = temp;
}

void Sprite::flip_horizontally()
{
	float temp = bounds.x;
	bounds.x = bounds.y;
	bounds.y = temp;
}

void Sprite::draw()
{
	Transform t = get_transform();
	t.update();									//TODO

	shader->Bind();
	shader->setUniform("model", RENDERER->get_final_matrix() * t.get_model());
	shader->setUniform("color", get_color());
	shader->setUniform("texture_enabled", true);

	if (packed)
	{
		vec4 b = GetTileBounds(activeindex);
		shader->setUniform("texbounds", b);
	}
	else
		shader->setUniform("texbounds", bounds);

	texture->Bind(0);
	defaultmesh->draw();

	shader->setWhiteColor("color");
}

void Sprite::SimpleDraw()
{
	CONTENT->SimpleShader->Bind();
	CONTENT->SimpleShader->setUniform("model", get_transform().get_model());

	defaultmesh->draw();
}

void Sprite::DrawMap()
{
	Transform t = get_transform();

	vec2 totalsize = t.get_size().get_xy();
	vec2 pos = t.get_pos().get_xy() + (totalsize - tilesize) * vec2(-1, 1);

	vec2 inner = totalsize - vec2(2) * tilesize;
	vec2 halfinner = inner / vec2(2);

	vec3 hor = vec3(0, tilesize.x + inner.x, tilesize.x * 2 + inner.x * 2);
	vec3 vert = vec3(0, -tilesize.y - inner.y, -inner.y * 2 - tilesize.y * 2);
	vec2 size;

	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			activeindex = x + 3 * y;
			if (activeindex == 0 || activeindex == 2 || activeindex == 6 || activeindex == 8) size = tilesize;
			else if (activeindex == 1 || activeindex == 7) size = vec2(inner.x + 0, tilesize.y);
			else if (activeindex == 3 || activeindex == 5) size = vec2(tilesize.x, inner.y + 0);
			else size = inner;

			t.update(pos + vec2(hor[x], vert[y]), size);
			draw();
		}
	}

	t.set_pos2d(pos);
	t.set_size2d(totalsize);
}

void Sprite::Init()
{
	defaultmesh = MeshHandler::get_singleton()->get_plane();
	defaultmesh->SetAttributes(CONTENT->LoadShader("EngineCore/Shader2D"));
}

void Sprite::update()
{
	WorldObject::update();
}

#undef CLASSNAME
#define CLASSNAME Sprite

void Sprite::bind_methods()
{
	REG_CSTR_OVRLD_1(String);
	REG_CSTR_OVRLD_1(Texture2D*);

	REG_METHOD(flip_vertically);
	REG_METHOD(flip_horizontally);

	REG_PROPERTY(shader);
	REG_PROPERTY(texture);
	REG_PROPERTY(bounds);
}