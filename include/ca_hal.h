#if HAVE_MIPS_HARDWARE \
 || (HAVE_ARM_HARDWARE \
     && !BOXMODEL_HD60 \
     && !BOXMODEL_MULTIBOX \
     && !BOXMODEL_MULTIBOXSE \
     && !BOXMODEL_OSMIO4K \
     && !BOXMODEL_OSMIO4KPLUS \
     && !BOXMODEL_SF8008 \
     && !BOXMODEL_SF8008M \
     && !BOXMODEL_USTYM4KPRO \
     && !BOXMODEL_H9 \
    )
#include "ca_ci.h"
#else
#include "ca.h"
#endif
