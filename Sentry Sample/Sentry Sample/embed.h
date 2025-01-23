#include <xtl.h>
#include "AtgConsole.h"
#include "AtgUtil.h"

ATG::Console console;

void initConsole(void)
{
	console.Create( "embed:\\font", 0xFF000000, 0xFFFFFFFF );
	console.Clear();
}