/*
程序编码日期：2016.03.17

*/

#ifndef UTILS_H
#define UTILS_H

#define PI 3.14159



template<typename... Args> struct SELECT {
    template<typename C, typename R>
    static constexpr auto OVERLOAD_OF( R (C::*pmf)(Args...) ) -> decltype(pmf) {
        return pmf;
    }
};
struct stParaInfo{
    int iStartFre;
    int iStopFre;
    int iStepFre;
    int iStepInterivalTime;
    int iScanNumbers;
    int iMutilScanInterivalTime;
    int iOptionalDC;
    int iOptionalAC;
};



#endif // UTILS_H
