#include <node.h>
#include "visa.h"

using namespace v8;

void setAnalyzer(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    double frequency = args[0].As<Number>()->Value();
    double power = args[1].As<Number>()->Value();
    double phase = args[2].As<Number>()->Value();

    //* C++ starts here

    ViSession defaultRM, viMXA;
    ViStatus viStatus = 0;

    viStatus = viOpenDefaultRM(&defaultRM);
    viStatus = viOpen(defaultRM, "USB0::0x03EB::0xAFFF::6C5-0B3D2000A-0436::0::INSTR", VI_NULL, VI_NULL, &viMXA);

    if (viStatus)
        return;
        
    // Initiate a sweep
    if (frequency == 0.0)
    {
        viPrintf(viMXA, "OUTP1:STAT OFF\n");
        viPrintf(viMXA, "OUTP2:STAT OFF\n");
    }
    else
    {
        viPrintf(viMXA, "SOUR1:PHAS:MEM:REST\n");
        viPrintf(viMXA, "SOUR2:PHAS:MEM:REST\n");
        viPrintf(viMXA, "SOUR1:FREQ %fGHz\n", frequency);
        viPrintf(viMXA, "SOUR2:FREQ %fGHz\n", frequency);
        viPrintf(viMXA, "SOUR1:POW %fdbm\n", power < 0 ? 0 : power);
        viPrintf(viMXA, "SOUR2:POW %fdbm\n", power < 0 ? std::abs(power) : 0);
        viPrintf(viMXA, "SOUR%d:PHAS %fdeg\n", phase < 0 ? 1 : 2, 0);
        viPrintf(viMXA, "SOUR%d:PHAS:REF\n", phase < 0 ? 1 : 2);
        viPrintf(viMXA, "SOUR%d:PHAS %fdeg\n", phase < 0 ? 2 : 1, std::abs(phase));
        viPrintf(viMXA, "OUTP1:STAT ON\n");
        viPrintf(viMXA, "OUTP2:STAT ON\n");
    }

    viClose(viMXA);     // closes session
    viClose(defaultRM); // closes default session

    //* C++ ends here
}

void init(Local<Object> exports, Local<Object> method)
{
    NODE_SET_METHOD(method, "exports", setAnalyzer);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, init);