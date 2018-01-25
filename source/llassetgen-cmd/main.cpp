#include <iostream>
#include <chrono>

#include <ft2build.h>
#include <freetype/freetype.h>

#include <llassetgen/llassetgen.h>

using namespace llassetgen;

int main(int argc, char** argv) {
    llassetgen::init();
	
	// creating the face
	FT_Face face;
	std::string font_file = "C:/Windows/Fonts/Arial.ttf"; // path to local font
	char letter = 'J';
	
	FT_Error face_created = FT_New_Face(freetype, font_file.c_str(), 0, &face);
	if (face_created != 0) {
		std::cerr << "Failed to create freetype face";
		abort();
	}
	
	FT_Error pixel_size_set = FT_Set_Pixel_Sizes(face, 64, 64);
	if (pixel_size_set != 0) {
		std::cerr << "Failed to set pixel size";
		abort();
	}
	
	// load letter
	FT_Error glyph_loaded = FT_Load_Glyph(face, FT_Get_Char_Index(face, letter), FT_LOAD_RENDER | FT_LOAD_MONOCHROME); // load default 8-bit //| FT_LOAD_MONOCHROME for 1-bit
	if (glyph_loaded != 0) {
		std::cerr << "Failed to render glyph";
		abort();
	}

	//Image ft_8bit(face->glyph->bitmap.width, face->glyph->bitmap.rows, 1);
	//ft_8bit.load(face->glyph->bitmap);

	Image ft_8bit(face->glyph->bitmap);

	// view on (10,10),(20,20)
	Image view_on_ft_8bit(ft_8bit.view(10, 20, 10, 20));
	// invert view
	for (int i = 0; i < view_on_ft_8bit.get_height(); i++) {
		for (int j = 0; j < view_on_ft_8bit.get_width(); j++) {
			view_on_ft_8bit.put<uint8_t>(j, i, 1 - view_on_ft_8bit.at<uint8_t>(j, i));
		}
	}
	
	// save whole image.
	ft_8bit.exportPng<uint8_t>("D:/Desktop/A_glyph1_out.png");
	
	// create blank image with 40x30 pixels and 16 bit
	Image blank_32bit(8, 8, 32);
	// set pixels
	blank_32bit.put<uint32_t>(3, 4, 2678137889);
	blank_32bit.put<uint32_t>(4, 5, 4294967295);
	
	// get pixels and test
	for (int i = 0; i < blank_32bit.get_height(); i++) {
		for (int j = 0; j < blank_32bit.get_width(); j++) {
			if (j == 3 && i == 4) {
				assert(blank_32bit.at<uint32_t>(j, i) == 2678137889);
			}
			else if (j == 4 && i == 5) {
				assert(blank_32bit.at<uint32_t>(j, i) == 4294967295);
			}
			else {
				assert(blank_32bit.at<uint32_t>(j, i) == 0);
			}
		}
	}
	blank_32bit.exportPng<uint32_t>("D:/Desktop/blank.png");
	
	Image loaded_png("D:/Desktop/A_glyph.png");
	// view on (10,10),(20,20)
	Image view_on_loaded_png(loaded_png.view(10, 20, 10, 20));
	// invert view
	for (int i = 0; i < view_on_loaded_png.get_height(); i++) {
		for (int j = 0; j < view_on_loaded_png.get_width(); j++) {
			view_on_loaded_png.put<uint16_t>(j, i, 0xFF - view_on_loaded_png.at<uint16_t>(j, i));
		}
	}
	loaded_png.exportPng<uint16_t>("D:/Desktop/A_glyph2_out.png");

	Image float_image(256, 256, 32);
	for (int x = 0; x < float_image.get_width(); x++) {
		for (int y = 0; y < float_image.get_height(); y++) {
			float_image.put<float>(x, y, float(x + y) / float(float_image.get_width() + float_image.get_height()));
		}
	}

	float_image.exportPng<float>("D:/Desktop/float.png", 0.0f, 1.0f);

	/*
	std::unique_ptr<DistanceTransform> dt(new DeadReckoning());
    if(argc == 5 && strcmp(argv[1], "-dt-from-glyph") == 0) {
        // Example: llassetgen-cmd -dt-from-glyph G /Library/Fonts/Verdana.ttf glyph.png
        FT_Face face;
        assert(FT_New_Face(freetype, argv[3], 0, &face) == 0);
        assert(FT_Set_Pixel_Sizes(face, 0, 128) == 0);
        assert(FT_Load_Glyph(face, FT_Get_Char_Index(face, argv[2][0]), FT_LOAD_RENDER | FT_LOAD_TARGET_MONO) == 0);
        dt->importFreeTypeBitmap(&face->glyph->bitmap, 20);
        dt->transform();
        dt->exportPng(argv[4], -10, 20, 8);
    } else if(argc == 4 && strcmp(argv[1], "-dt-from-png") == 0) {
        // Example: llassetgen-cmd -dt-from-png input.png output.png
        dt->importPng(argv[2]);
        dt->transform();
        dt->exportPng(argv[3], -20, 50, 8);
    }
	*/
    return 0;
}
