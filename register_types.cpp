#include "register_types.h"
#include "core/class_db.h"
#include "qr_code_texture.h"

void register_qrcodetexture_types()
{
    ClassDB::register_class<QRCodeTexture>();
}

void unregister_qrcodetexture_types() {}
