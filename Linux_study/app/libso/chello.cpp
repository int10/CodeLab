extern "C"
{
        #include "hello.h"
 
        int cprint()
        {
                CHello * phello;
                phello =  new CHello ;
 
                (void *)phello-> Print();
 
                return 0;
        }
}