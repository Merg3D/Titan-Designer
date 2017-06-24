#include "Transform.h"

#include "Quaternion.h"

Transform::Transform(const vec3& p, const vec3& s, const vec3& r)
{
	pos = p;
	size = s;
	rotation = r;

	update();
}

Transform::~Transform()
{

}

bool Transform::operator==(const Transform & r) const
{
	return pos == r.pos && size == r.size && rotation == r.rotation;
}

bool Transform::operator!=(const Transform & r) const
{
	return pos != r.pos || size != r.size || rotation != r.rotation;
}

Transform::operator String() const
{
	return ToString();
}

String Transform::ToString() const
{
	return "{ pos: " + pos.to_string() + ", size: " + size.to_string() + ", rotation: " + rotation.to_string() + " }";
}

void Transform::update(const vec3& pos, const vec3& size, const vec3& p_rot)
{
	this->pos = pos;
	this->size = size;
	this->rotation = p_rot;
	update();
}

void Transform::update(const vec3& pos, const vec3& size)
{
	this->pos = pos;
	this->size = size;
	update();
}

void Transform::update(const vec3 & p_pos, const vec2 & p_size)
{
	update(p_pos, vec3(p_size, 0.0f));
}

void Transform::update(const vec2 & p_pos, const vec3 & p_size)
{
	update(vec3(p_pos, 0.0f), p_size);
}

void Transform::update(const vec2 & p_pos, const vec2 & p_size)
{
	update(vec3(p_pos, 0.0f), vec3(p_size, 0.0f));
}

void Transform::update()
{
	mat4 translate, rotate, scale;

	Quaternion quat = Quaternion(rotation);
	rotate = quat;

	translate.translate(vec3(pos.x, pos.y, pos.z));
	scale.scale(vec3(size.x, size.y, size.z));

	model = translate * rotate * scale;
}

void Transform::pull()
{
	poppos = pos;
	popsize = size;
	poprotation = rotation;
}

void Transform::pop()
{
	pos = poppos;
	size = popsize;
	rotation = poprotation;

	update();
}

void Transform::set_pos2d(const vec2 & p_pos)
{
	pos = vec3(p_pos, 0.0f);
}

void Transform::set_pos(const vec3 &p_pos)
{
	pos = p_pos;
}

const vec3& Transform::get_pos() const
{
	return pos;
}

void Transform::set_size2d(const vec2& p_size)
{
	size = vec3(p_size, 0.0f);
}

void Transform::set_size(const vec3& p_size)
{
	size = p_size;
}

const vec3& Transform::get_size() const
{
	return size;
}

void Transform::set_rotation(const vec3& p_rotation)
{
	rotation = p_rotation;
}

vec3 Transform::get_rotation() const
{
	return rotation;
}

const mat4& Transform::get_model() const
{
	return model;
}
