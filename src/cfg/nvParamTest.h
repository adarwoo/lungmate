#ifndef __NVPARAMCONFIG_H_HAS_ALREADY_BEEN_INCLUDED__
#define __NVPARAMCONFIG_H_HAS_ALREADY_BEEN_INCLUDED__
/**@ingroup unittest*/
/**@file
 * Configuration for the nvParam unit test.
 */

// Use the default config for most parameters
#include "cfg.h"

// ---------------------------------------------------------------------------
// nvParam configuration relative to src/
// ---------------------------------------------------------------------------
#undef NV_PARAM_DEFS_FILENAME
#define NV_PARAM_DEFS_FILENAME "lib/test/nvParamDefs.h"

#endif // ndef __NVPARAMCONFIG_H_HAS_ALREADY_BEEN_INCLUDED__

