#pragma once

#ifndef IMAGEMAGICK_PATH
#error Macro IMAGEMAGICK_PATH is required!
#endif

#include <vector>
#include <iostream>

#include "../libs/FilesManager/FilesManager.hpp"

#define cimg_display 0 // 1 if X11 is installed
#include "../libs/CImg/CImg.h"
using namespace cimg_library;

namespace brahand {
template <class Any = uint>
struct Image : std::vector<Any> {

        struct ImageSize {
                uint width, height, depth;

                ImageSize() = default;
                ImageSize(uint w, uint h, uint d=1) : width(w), height(h), depth(d){
                }

                uint total() const {
                        return width * height * depth;
                }
                uint operator ()(uint x, uint y, uint z) const {
                        return (z * width * height) + (y * width) + x;
                }
        };

        ImageSize image_size;
        Image(ImageSize size){
                this->assign(size.total(), 0);
                this->image_size={size.width,size.height,size.depth};
        };
        Image() = delete;

        Image(std::string image_path){
                cimg::imagemagick_path(IMAGEMAGICK_PATH);
                typedef brahand::FilesManager::FilesArray FilesArray;

                FilesArray files;
                if (image_path.find_last_of('.') == -1) { // its a folder, 3D
                        files = FilesManager::files_in_directory(image_path);
                } else { // its a file, 2D
                        files = FilesArray{image_path};
                }

                std::vector<CImg<> > imagesList(files.size());

                auto firstImage = CImg<>(files[0].c_str());
                this->image_size = {(uint)firstImage.width(), (uint)firstImage.height(), (uint)files.size()};
                this->assign(this->image_size.total(), 0);

                for(auto z = 0; z < files.size(); ++z) {
                        imagesList[z] = CImg<>(files[z].c_str());
                        if(imagesList[z].spectrum() == 3) {
                                imagesList[z] = imagesList[z].get_RGBtoYCbCr().get_channel(0);
                        }
                        cimg_forXY(imagesList[z], x, y){
                                this->at(this->image_size(x, y, z)) = (Any)imagesList[z](x,y);
                        }
                }
        }

};

}
