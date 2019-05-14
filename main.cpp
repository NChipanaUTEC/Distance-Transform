#if defined(__APPLE__) || defined(__MACOSX)
    #define IMAGEMAGICK_PATH "/usr/local/bin/convert"
#else
    #define IMAGEMAGICK_PATH "/usr/bin/convert"
#endif

#include "src/Image.hpp"
#include "src/ImageExporter.hpp"

typedef brahand::Image<uint> Image;

#include <iostream>
using namespace std;

int main(){
    Image image("examples/image.png");
    brahand::VTKExport::grayscale(image, "./", "image");

    return 0;
}
