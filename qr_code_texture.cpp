#include "qr_code_texture.h"
#include "qrcodegen.hpp"

QRCodeTexture::QRCodeTexture()
{
    texture = VS::get_singleton()->texture_create();
    data = Ref<Image>(memnew(Image()));
    error_correction = ERROR_CORRECTION_MEDIUM;
    border = 2;
    enable_colors = false;
    background_color = Color(1.f, 1.f, 1.f);
    foreground_color = Color(0.f, 0.f, 0.f);
    update_queued = false;
    _update_texture();
}

QRCodeTexture::~QRCodeTexture()
{
	VS::get_singleton()->free(texture);
}

void QRCodeTexture::_queue_update()
{
    if (!update_queued)
    {
        update_queued = true;
        call_deferred("_update_texture");
    }
}

void QRCodeTexture::set_text(const String &p_text)
{
    text = p_text;
    _queue_update();
}

String QRCodeTexture::get_text() const
{
    return text;
}

void QRCodeTexture::set_error_correction(ErrorCorrection p_error_correction)
{
    error_correction = p_error_correction;
    _queue_update();
}

QRCodeTexture::ErrorCorrection QRCodeTexture::get_error_correction() const
{
    return error_correction;
}

void QRCodeTexture::set_border(int p_border)
{
    border = p_border < 0 ? 0 : p_border;
    _queue_update();
}

int QRCodeTexture::get_border() const
{
    return border;
}

void QRCodeTexture::set_enable_colors(bool p_enable_colors)
{
    if (enable_colors != p_enable_colors)
    {
        enable_colors = p_enable_colors;
        data_format_changed = true;
        _queue_update();
	    _change_notify(); // triggers _validate_property
    }
}

bool QRCodeTexture::get_enable_colors() const
{
    return enable_colors;
}

void QRCodeTexture::set_background_color(const Color &p_color)
{
    background_color = p_color;
    _queue_update();
}

Color QRCodeTexture::get_background_color() const
{
    return background_color;
}

void QRCodeTexture::set_foreground_color(const Color &p_color)
{
    foreground_color = p_color;
    _queue_update();
}

Color QRCodeTexture::get_foreground_color() const
{
    return foreground_color;
}

void QRCodeTexture::set_flags(uint32_t p_flags) {
    flags = p_flags;
	VS::get_singleton()->texture_set_flags(texture, flags);
}

void QRCodeTexture::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_update_texture"), &QRCodeTexture::_update_texture);
	ClassDB::bind_method(D_METHOD("_queue_update"), &QRCodeTexture::_queue_update);
	
    ClassDB::bind_method(D_METHOD("set_text", "text"), &QRCodeTexture::set_text);
	ClassDB::bind_method(D_METHOD("get_text"), &QRCodeTexture::get_text);
    ClassDB::bind_method(D_METHOD("set_error_correction", "error_correction"), &QRCodeTexture::set_error_correction);
	ClassDB::bind_method(D_METHOD("get_error_correction"), &QRCodeTexture::get_error_correction);
    ClassDB::bind_method(D_METHOD("set_border", "border"), &QRCodeTexture::set_border);
	ClassDB::bind_method(D_METHOD("get_border"), &QRCodeTexture::get_border);
    ClassDB::bind_method(D_METHOD("set_enable_colors", "enable_colors"), &QRCodeTexture::set_enable_colors);
	ClassDB::bind_method(D_METHOD("get_enable_colors"), &QRCodeTexture::get_enable_colors);
    ClassDB::bind_method(D_METHOD("set_background_color", "color"), &QRCodeTexture::set_background_color);
	ClassDB::bind_method(D_METHOD("get_background_color"), &QRCodeTexture::get_background_color);
    ClassDB::bind_method(D_METHOD("set_foreground_color", "color"), &QRCodeTexture::set_foreground_color);
	ClassDB::bind_method(D_METHOD("get_foreground_color"), &QRCodeTexture::get_foreground_color);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "text"), "set_text", "get_text");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "error_correction", PROPERTY_HINT_ENUM, "Low,Medium,Quartile,High"), "set_error_correction", "get_error_correction");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "border", PROPERTY_HINT_RANGE, "0,10,1,or_greater"), "set_border", "get_border");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enable_colors"), "set_enable_colors", "get_enable_colors");
    ADD_PROPERTY(PropertyInfo(Variant::COLOR, "background_color"), "set_background_color", "get_background_color");
    ADD_PROPERTY(PropertyInfo(Variant::COLOR, "foreground_color"), "set_foreground_color", "get_foreground_color");

    BIND_ENUM_CONSTANT(ERROR_CORRECTION_LOW);
    BIND_ENUM_CONSTANT(ERROR_CORRECTION_MEDIUM);
    BIND_ENUM_CONSTANT(ERROR_CORRECTION_QUARTILE);
    BIND_ENUM_CONSTANT(ERROR_CORRECTION_HIGH);
}

void QRCodeTexture::_validate_property(PropertyInfo &property) const 
{
	if (property.name == "background_color" || property.name == "foreground_color") 
    {
		if (!enable_colors) 
        {
			property.usage = PROPERTY_USAGE_NOEDITOR | PROPERTY_USAGE_INTERNAL;
		}
	}
}

void QRCodeTexture::_update_texture()
{
    update_queued = false;

    qrcodegen::QrCode::Ecc qr_ec_level;
    switch(error_correction)
    {
    case ERROR_CORRECTION_LOW: 
        qr_ec_level = qrcodegen::QrCode::Ecc::LOW; 
        break;
    case ERROR_CORRECTION_QUARTILE: 
        qr_ec_level = qrcodegen::QrCode::Ecc::QUARTILE; 
        break;
    case ERROR_CORRECTION_HIGH: 
        qr_ec_level = qrcodegen::QrCode::Ecc::HIGH; 
        break;
    case ERROR_CORRECTION_MEDIUM: // Medium is default setting
    default:
        qr_ec_level = qrcodegen::QrCode::Ecc::MEDIUM; 
        break;
    }

    const qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(text.utf8().get_data(), qr_ec_level);

    const int qr_size = qr.getSize();
    const int double_border = border*2;
    const int qr_size_bordered = qr_size + double_border;

    size = Vector2i(qr_size_bordered, qr_size_bordered);

    if (data->get_width() != qr_size_bordered || data->get_height() != qr_size_bordered || data_format_changed)
    {
        data_format_changed = false;
        data->create(qr_size_bordered, qr_size_bordered, false, enable_colors ? Image::FORMAT_RGB8 : Image::FORMAT_L8);
    }

    Color draw_fg_color;
    Color draw_bg_color;
    if (enable_colors)
    {
        draw_fg_color = foreground_color;
        draw_bg_color = background_color;
    }
    else
    {
        draw_bg_color = Color(1.f, 1.f, 1.f);
    }

    data->lock();

    // Fill inside borders
    for (int y = 0; y < qr_size; y++)
    {
        for (int x = 0; x < qr_size; x++)
        {
            data->set_pixel(x + border, y + border, qr.getModule(x, y) ? draw_fg_color : draw_bg_color);
        }
    }

    // Fill borders
    // top
    for (int y = 0; y < border; y++)
    {
        for (int x = 0; x < qr_size_bordered; x++)
        {
            data->set_pixel(x, y, draw_bg_color);
        }
    }
    // bottom
    for (int y = qr_size_bordered - border; y < qr_size_bordered; y++)
    {
        for (int x = 0; x < qr_size_bordered; x++)
        {
            data->set_pixel(x, y, draw_bg_color);
        }
    }
    // remaining left
    for (int x = 0; x < border; x++)
    {
        for (int y = border; y < qr_size_bordered-border; y++)
        {
            data->set_pixel(x, y, draw_bg_color);
        }
    }
    // remaining right
    for (int x = qr_size_bordered-border; x < qr_size_bordered; x++)
    {
        for (int y = border; y < qr_size_bordered-border; y++)
        {
            data->set_pixel(x, y, draw_bg_color);
        }
    }

    data->unlock();
    
    if (data.is_valid()) {
		VS::get_singleton()->texture_allocate(texture, size.x, size.y, 0, data->get_format(), VS::TEXTURE_TYPE_2D, flags);
		VS::get_singleton()->texture_set_data(texture, data);
	    emit_changed();
	}
}