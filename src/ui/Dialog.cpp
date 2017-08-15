#include "Dialog.h"

#include "Canvas.h"
#include "Container.h"
#include "ListView.h"
#include "resources/File.h"
#include "TreeView.h"
#include "NumberField.h"

//=========================================================================
//Dialog
//=========================================================================

Dialog::Dialog()
{
	close_button = new ImageButton("EngineCore/UI/Close.png");
	close_button->set_anchors(Control::ANCHOR_END, Control::ANCHOR_END, Control::ANCHOR_END, Control::ANCHOR_END);
	close_button->set_margins(30, 31, 1, 1);

	close_button->connect("clicked", this, "close");

	add_child(close_button);

	title = get_type_name();

	header_offset = 4.0f;

	is_grabbing = false;
}

Dialog::~Dialog()
{
}

vec2 Dialog::get_required_size() const
{
	return vec2(800);
}

void Dialog::handle_event(UIEvent* p_event)
{
	switch (p_event->type)
	{
	case UIEvent::MOUSE_PRESS:

		if (p_event->button_type != Mouse::LEFT)
			return;

		if (p_event->press_type == UIEvent::DOWN)
		{
			if (bar.is_in_box(p_event->pos))
			{
				grab_pos = p_event->pos;
				orig_pos = get_pos();
				is_grabbing = true;
			}
		}
		else
			is_grabbing = false;

		break;

	case UIEvent::MOUSE_HOVER:

		if (MOUSE->is_pressed(Mouse::LEFT) && is_grabbing)
		{
			vec2 delta = p_event->pos - grab_pos;
			set_pos(orig_pos + delta);
		}	

		break;
	}
}

void Dialog::notification(int p_notification)
{
	switch (p_notification)
	{
	case NOTIFICATION_DRAW:

		draw_bordered_box(area, Color::Black, TO_RGB(80));
		draw_box(bar, TO_RGB(150));

		draw_text(DEFAULT_THEME->get_font(), title, vec2(bar.get_left() + header_offset, bar.pos.y), TO_RGB(30));

		break;

	case NOTIFICATION_RESIZED:
	case NOTIFICATION_TRANSLATED:

		bar = rect2(area.get_left() + 1, area.get_right() - 1, area.get_top() - 1, area.get_top() - 31);


		break;
	}
}

void Dialog::set_title(const String & p_title)
{
	title = p_title;
}

String Dialog::get_title() const
{
	return title;
}

void Dialog::show()
{
	get_canvas()->set_dialog(this);
}

void Dialog::close()
{
	get_canvas()->remove_control(this);
}

#undef CLASSNAME
#define CLASSNAME Dialog

void Dialog::bind_methods()
{
	REG_METHOD(close);
}

//=========================================================================
//MessageDialog
//=========================================================================

ConfirmationDialog::ConfirmationDialog()
{
	//ok_button
	ok_button = new TextButton("OK");
	ok_button->set_anchors(ANCHOR_CENTER, ANCHOR_BEGIN, ANCHOR_CENTER, ANCHOR_BEGIN);
	ok_button->set_margins(-110, 10, -10, 35);

	ok_button->connect("clicked", this, "close");

	add_child(ok_button);

	//cancel_button
	cancel_button = new TextButton("Cancel");
	cancel_button->set_anchors(ANCHOR_CENTER, ANCHOR_BEGIN, ANCHOR_CENTER, ANCHOR_BEGIN);
	cancel_button->set_margins(10, 10, 110, 35);

	cancel_button->connect("clicked", this, "close");

	add_child(cancel_button);
}

#undef CLASSNAME
#define CLASSNAME ConfirmationDialog

void ConfirmationDialog::bind_methods()
{
	REG_SIGNAL("ok");
	REG_SIGNAL("cancel");
}

//=========================================================================
//MessageDialog
//=========================================================================

MessageDialog::MessageDialog() : MessageDialog("")
{
}

MessageDialog::MessageDialog(const String & p_text)
{
	//desciption_label
	desciption_label = new Label(p_text);
	desciption_label->set_anchors(ANCHOR_BEGIN, ANCHOR_END, ANCHOR_END, ANCHOR_END);
	desciption_label->set_margins(4, 60, 4, 30);

	add_child(desciption_label);
}

#undef CLASSNAME
#define CLASSNAME MessageDialog

void MessageDialog::bind_methods()
{
}

//=========================================================================
//FileDialog
//=========================================================================

FileDialog::FileDialog() : FileDialog("")
{
}

FileDialog::FileDialog(const File& p_file)
{
	directory_icon = CONTENT->LoadTexture("EngineCore/UI/Directory.png");
	file_icon = CONTENT->LoadTexture("EngineCore/UI/Picture.png");

	back_button = new ImageButton("EngineCore/UI/Back.png");
	forward_button = new ImageButton("EngineCore/UI/Forward.png");
	buttons = new Container;
	path_label = new TextField;
	list = new ListView;

	//back_button
	back_button->connect("clicked", this, "go_back");
	back_button->set_tip_description("Back");

	//forward_button
	forward_button->connect("clicked", this, "go_forward");
	back_button->set_tip_description("Forward");

	//buttons
	buttons->add_child(back_button);
	buttons->add_child(forward_button);

	vec2 size = buttons->get_required_size();
	vec2 offset = size + 4 + 30;
	buttons->set_anchors(ANCHOR_BEGIN, ANCHOR_END, ANCHOR_END, ANCHOR_END);
	buttons->set_margins(4, offset.y, 4, offset.y - size.y);

	//path_label
	size = path_label->get_required_size();
	offset += size + 4;

	path_label->set_anchors(ANCHOR_BEGIN, ANCHOR_END, ANCHOR_END, ANCHOR_END);
	path_label->set_margins(4, offset.y, 4, offset.y - size.y);
	path_label->connect("entered", this, "go_to");

	//list
	size = list->get_required_size();
	offset += size + 4;

	list->set_anchors(ANCHOR_BEGIN, ANCHOR_BEGIN, ANCHOR_END, ANCHOR_END);
	list->set_margins(4, 45, 4, offset.y - size.y);
	list->connect("chosen", this, "list_chosen");

	add_child(buttons);
	add_child(list);
	add_child(path_label);

	go_to(p_file);

	set_title("Choose a file");
}

void FileDialog::update_list()
{
	list->clear();

	Array<File> files = file.listdir();

	for (int c = 0; c < files.size(); c++)
	{	
		File f = files[c];
		list->push_back_item(f.get_name(), f.is_directory() ? directory_icon : file_icon);
	}

	path_label->set_text(file.get_relative_path());
}

void FileDialog::list_chosen(int p_index)
{
	File f = file + list->get_item(p_index)->get_text();

	if (f.is_directory())
		go_to(f);
	else
	{
		file = f;
		emit_signal("file_chosen", f.get_absolute_path());
		close();
	}
}

void FileDialog::go_to(const File& p_file)
{
	file = p_file;
	update_list();
}

void FileDialog::go_back()
{
	file.go_up();
	update_list();
}

void FileDialog::go_forward()
{

}

#undef CLASSNAME
#define CLASSNAME FileDialog

void FileDialog::bind_methods()
{
	REG_CSTR(0);
	REG_METHOD(go_back);
	REG_METHOD(go_forward);
	REG_METHOD(list_chosen);

	REG_SIGNAL("file_chosen");
}

//=========================================================================
//ColorPickDialog
//=========================================================================

ColorPickDialog::ColorPickDialog() : ColorPickDialog(Color::White)
{

}

ColorPickDialog::ColorPickDialog(const Color& p_color)
{
	color = p_color;

	//labels
	r_label = new Label("R");
	g_label = new Label("G");
	b_label = new Label("B");

	add_child(r_label);
	add_child(g_label);
	add_child(b_label);

	r_label->set_anchors(ANCHOR_BEGIN, ANCHOR_BEGIN, ANCHOR_BEGIN, ANCHOR_BEGIN);
	g_label->set_anchors(ANCHOR_BEGIN, ANCHOR_BEGIN, ANCHOR_BEGIN, ANCHOR_BEGIN);
	b_label->set_anchors(ANCHOR_BEGIN, ANCHOR_BEGIN, ANCHOR_BEGIN, ANCHOR_BEGIN);

	r_label->set_margins(4, 100, 36, 130);
	g_label->set_margins(4, 70, 36, 100);
	b_label->set_margins(4, 40, 36, 70);

	//fields
	r_field = new NumberField(color.r);
	g_field = new NumberField(color.g);
	b_field = new NumberField(color.b);

	add_child(r_field);
	add_child(g_field);
	add_child(b_field);
	
	r_field->set_anchors(ANCHOR_BEGIN, ANCHOR_BEGIN, ANCHOR_END, ANCHOR_BEGIN);
	g_field->set_anchors(ANCHOR_BEGIN, ANCHOR_BEGIN, ANCHOR_END, ANCHOR_BEGIN);
	b_field->set_anchors(ANCHOR_BEGIN, ANCHOR_BEGIN, ANCHOR_END, ANCHOR_BEGIN);

	r_field->set_margins(44, 100, 4, 130);
	g_field->set_margins(44, 70, 4, 100);
	b_field->set_margins(44, 40, 4, 70);
}

#undef CLASSNAME
#define CLASSNAME ColorPickDialog

void ColorPickDialog::bind_methods()
{
	REG_CSTR(0);
	REG_CSTR_OVRLD_1(Color);

	REG_SIGNAL("color_chosen");
}

void ColorPickDialog::notification(int p_notification)
{
	Dialog::notification(p_notification);

	switch (p_notification)
	{
	case NOTIFICATION_DRAW:
		
		rect2 color_area = area.crop(4, 4, 40, 150);

		draw_box(color_area, color);

		break;
	}
}

//=========================================================================
//TypePickDialog
//=========================================================================

TypePickDialog::TypePickDialog()
{
	tree = new TreeView;
	tree->set_anchors(ANCHOR_BEGIN, ANCHOR_BEGIN, ANCHOR_END, ANCHOR_END);
	tree->set_margins(4, 60, 4, 60);

	add_child(tree);

	tree->connect_signal("chosen", this, "type_chosen");

	update_list();
}

void TypePickDialog::update_list()
{
	tree->clear();

	for (std::pair<StringName, ObjectType> pair : TYPEMAN->object_types)
	{
		tree->push_back_path(pair.second.path);
	}
}

void TypePickDialog::tree_chosen(TreeElement* p_element)
{
	emit_signal("type_chosen", p_element->get_text());
}

#undef CLASSNAME
#define CLASSNAME TypePickDialog

void TypePickDialog::bind_methods()
{
	REG_CSTR(0);

	REG_METHOD(tree_chosen);

	REG_SIGNAL("type_chosen");
}