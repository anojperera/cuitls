#ifndef __status_h__
#define __status_h__

#define CCSVCUBE_STATUS_SUCCESS 0
#define CCSVCUBE_STATUS_FAILED -1

#define CUTILS_CHECK_INIT(obj) \
        if((obj)->init_flg != CCSVCUBE_STATUS_SUCCESS) \
                return CCSVCUBE_STATUS_FAILED

#endif /* __status_h__ */
