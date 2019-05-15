#pragma once

#include "Image.hpp"
#include <string>
#include <fstream>
#include <memory>

namespace brahand {
    class VTKExport {
    public:
        VTKExport() = default;
    
        template <class Any>
        static void grayscale( Image<Any> image, std::string folder, std::string name ){
            printf("Exporting %s \t.... ", name.c_str());
            
            auto size = image.image_size;
            std::string fullPath = folder + name + ".vtk";
            
            std::ofstream output; 
            output.open( fullPath.c_str() );
            output << "# vtk DataFile Version 1.0\nUnstructured Grid Example\nASCII\nDATASET UNSTRUCTURED_GRID\n";
            
            output << "POINTS " << size.total() << " float\n";
            for(uint z = 0 ; z < size.depth ; ++z){
                for(uint y = 0 ; y < size.height ; ++y){
                    for(uint x = 0 ; x < size.width ; ++x){
                        output << x << " " << size.height - y << " " << z << "\n";
                    }
                }
            }
            
            output << "CELLS " << size.total() << " " << size.total() * 2 << "\n";
            for(uint i = 0 ; i < size.total() ; ++i) output << "1 " << i << "\n";
            
            output << "CELL_TYPES " << size.total() << "\n";
            for(uint i = 0 ; i < size.total() ; ++i) output << "1 ";
            
            output << "\nCELL_DATA " << size.total() << "\nCOLOR_SCALARS lut 1\n";
            for(uint i = 0 ; i < size.total() ; ++i){
                float color = (float)(image[i]) / 255.0;
                output << color << "\n";
            }
            
            output.close();
            printf("done!\n");
        }
    };

}
