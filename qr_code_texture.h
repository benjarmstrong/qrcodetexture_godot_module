
#ifndef QR_CODE_TEXTURE_H
#define QR_CODE_TEXTURE_H

#include "core/image.h"
#include "scene/resources/texture.h"

class QRCodeTexture: public Texture {
    GDCLASS(QRCodeTexture, Texture);
public:
    enum ErrorCorrection {
        ERROR_CORRECTION_LOW = 0,
        ERROR_CORRECTION_MEDIUM,
        ERROR_CORRECTION_QUARTILE,
        ERROR_CORRECTION_HIGH,
    };

    QRCodeTexture();
    virtual ~QRCodeTexture();
    
    void set_text(const String &str);
    String get_text() const;

    void set_error_correction(ErrorCorrection p_error_correction);
    ErrorCorrection get_error_correction() const;

    void set_border(int p_border);
    int get_border() const;

    void set_enable_colors(bool p_enable_colors);
    bool get_enable_colors() const;

    void set_background_color(const Color &p_color);
    Color get_background_color() const;

    void set_foreground_color(const Color &p_color);
    Color get_foreground_color() const;

    // Overrides
	void set_flags(uint32_t p_flags);
    bool has_alpha() const { return false; };
	uint32_t get_flags() const { return flags; }
    int get_width() const { return size.x; }
	int get_height() const { return size.y; }
	RID get_rid() const { return texture; }
	Ref<Image> get_data() const { return data; }
protected:
    static void _bind_methods();
	void _validate_property(PropertyInfo &property) const;
private:
    String text;
    ErrorCorrection error_correction;
    int border;
    bool enable_colors;
    Color background_color;
    Color foreground_color;

    Ref<Image> data;

    RID texture;
	uint32_t flags;

	Vector2i size;

    bool update_queued;
    bool data_format_changed;

    void _queue_update();
    void _update_texture();
};

VARIANT_ENUM_CAST(QRCodeTexture::ErrorCorrection);

#endif // QR_CODE_TEXTURE_H