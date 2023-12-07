/**
 *@ingroup nvparam_test
 *@{
 *@file
 *****************************************************************************
 * Defines all non-volatile parameters used by the unit test
 *
 * @author software@arreckx.com
 *****************************************************************************
 */

NV_PARAM_TABLE_BEGIN
   NV_PARAM( paramUnsigned,  "Unsigned parameter",        0, 32767, 9999 )
   NV_PARAM( paramSigned,    "Signed Parameter",     -32768, 32767,    0 )
   NV_PARAM( paramBool0,     "Bool Parameter false",      0,     1,    0 )
   NV_PARAM( paramBool1,     "Bool Parameter true",       0,     1,    1 )
NV_PARAM_TABLE_END


