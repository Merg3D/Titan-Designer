#pragma once

#include "FramedButton.h"

class TextButton : public FramedButton
{
	OBJ_DEFINITION(TextButton, FramedButton);

public:
	TextButton();
	TextButton(const String& p_text);

	vec2 get_required_size() const override;

	void notification(int p_notification) override;

private:

	int left_margin, right_margin;

	String text;
	Font *font;
};