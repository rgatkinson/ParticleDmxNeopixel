//
// CloudVariables.h
//
#ifndef __CLOUD_VARIABLES_H__
#define __CLOUD_VARIABLES_H__

struct CloudVariableRegistrar
{
    void registerVariable
        (
        int (*pfnGet)(),
        void (*pfnSet)(int);
        );
};

#endif
