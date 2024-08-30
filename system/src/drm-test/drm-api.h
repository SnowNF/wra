//
// Created by snownf on 24-7-22.
//

#ifndef WRA_DRM_API_H
#define WRA_DRM_API_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include "libdrm/drm.h"
#include "libdrm/drm_mode.h"
#include "libdrm/xf86drm.h"
#include "libdrm/xf86drmMode.h"


#define drm_public  __attribute__((visibility("default")))


#if defined(__cplusplus)
}
#endif

#endif //WRA_DRM_API_H
