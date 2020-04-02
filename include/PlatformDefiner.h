#ifndef MULTYTHREADING_PLATFORMDEFINER_H
#define MULTYTHREADING_PLATFORMDEFINER_H

#define windows 0
#define unix 1
#ifdef _WIN32
    #define PLATFORM_CODE windows
#elif defined __unix__
    #define PLATFORM_CODE unix
#elif defined __APPLE__
    #define PLATFORM_CODE unix
#endif



#if defined(PLATFORM_CODE)
    #if PLATFORM_CODE == windows
        #include <iostream>
        int i(){
            std::cout<<"1"<<std::endl;
        }
    #elif PLATFORM_CODE == unix
        #include <iostream>
        int i(){
            std::cout<<"2"<<std::endl;
            return 0;
        }
    #endif
#endif


#endif //MULTYTHREADING_PLATFORMDEFINER_H
