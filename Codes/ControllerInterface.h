#ifndef _ControllerInterface_h_
#define _ControllerInterface_h_

class ControllerInterface
{
public:    
    ControllerInterface() {};
    virtual ~ControllerInterface() {};
    
    virtual void Start(const char *xmlPath) = 0;
    virtual void Stop() = 0;

    static ControllerInterface &GetInstance();
};

#endif