#pragma once

#include "FBO.h"
#include "resources/Shader.h"
#include "FBOManager.h"
#include "core/ContentManager.h"
#include "world/Primitives.h"

#define VISUALEFFECT PostProcess::get_singleton()

class PostProcess : public Object
{
	OBJ_DEFINITION(PostProcess, Object);

public:
	PostProcess();
	PostProcess(Shader* p_shader);
	virtual ~PostProcess();

	void set_area(const rect2 & p_area);
	rect2 get_area() const;

	virtual void post_process();

protected:	
	Shader* shader;

	rect2 area;
};

class WorldPostProcess : public PostProcess
{
	OBJ_DEFINITION(WorldPostProcess, PostProcess);
	
public:
	WorldPostProcess();
	virtual ~WorldPostProcess();

	void post_process() override;
};

class BlurPostProcess : public PostProcess
{
	OBJ_DEFINITION(BlurPostProcess, PostProcess);

public:
	BlurPostProcess();
	virtual ~BlurPostProcess();

	void post_process() override;

private:
	Shader* blurshader;
	FBO2D *blurbuffer;
};