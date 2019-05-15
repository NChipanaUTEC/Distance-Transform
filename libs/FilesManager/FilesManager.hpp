#pragma once

#include <dirent.h>
#include <vector>
#include <string>
#include <map>
#include <assert.h>

namespace brahand {

    inline std::string regexNumber( const std::string path){
        std::string stringNumber = "";
        for(char c : path){
            if( (int) c >= 48 && (int) c <=57) stringNumber += c;
        }
        return stringNumber;
    }

    struct FilesManager {
        typedef std::vector<std::string> FilesArray;
        
        static inline FilesArray files_in_directory(const std::string directory){
            FilesArray files;
            DIR * fileManager; struct dirent * dirp;
            
            assert(  (fileManager = opendir(directory.c_str())) != NULL);
            
            std::map<int, std::string> filesMap;
            
            while( (dirp = readdir(fileManager)) != NULL){
                if(dirp->d_name[0] != '.'){
                    const std::string ex = std::string(dirp->d_name).c_str();
                    const std::string tmp = regexNumber(ex);
                    filesMap[std::stoi(tmp.c_str())] = directory + "/" + ex;
                }
            }
            closedir(fileManager);
            
            for(auto it = filesMap.begin() ; it!= filesMap.end() ; ++it){
                files.push_back( (*it).second.c_str() );
            }
            
            return files;
        }
    };
    
}
