/**
 *@ingroup dbg
 *@defgroup dbg_test Unit test
 *@{
 *@file
 *****************************************************************************
 * Test the dbg API. The config file should define all 8 debug pin for this 
 *  test to work.
 * A steady square signal should be seen on the scope on all debug pins
 *
 * @author software@arreckx.com
 *****************************************************************************
 */
 
#include "dbg.h"

int main(void)
{
   // Init the debug lib
   dbgInit();

   for (;;)
   {
      dbgSet(0);
      dbgSet(1);
      dbgSet(2);
      dbgSet(3);
      dbgSet(4);
      dbgSet(5);
      dbgSet(6);
      dbgSet(7);

      dbgToggle(0);
      dbgToggle(1);
      dbgToggle(2);
      dbgToggle(3);
      dbgToggle(4);
      dbgToggle(5);
      dbgToggle(6);
      dbgToggle(7);

      dbgSet(0);
      dbgSet(1);
      dbgSet(2);
      dbgSet(3);
      dbgSet(4);
      dbgSet(5);
      dbgSet(6);
      dbgSet(7);

      dbgClear(0);
      dbgClear(1);
      dbgClear(2);
      dbgClear(3);
      dbgClear(4);
      dbgClear(5);
      dbgClear(6);
      dbgClear(7);

      dbgToggle(0);
      dbgToggle(1);
      dbgToggle(2);
      dbgToggle(3);
      dbgToggle(4);
      dbgToggle(5);
      dbgToggle(6);
      dbgToggle(7);

      dbgClear(0);
      dbgClear(1);
      dbgClear(2);
      dbgClear(3);
      dbgClear(4);
      dbgClear(5);
      dbgClear(6);
      dbgClear(7);
   }
}
