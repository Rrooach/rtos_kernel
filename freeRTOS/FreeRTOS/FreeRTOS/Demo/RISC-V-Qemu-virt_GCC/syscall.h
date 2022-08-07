

#define chose(a) \
    if (a == 1) \
        SemaphoreHandle_t xSemaphore; \
        xSemaphore=xSemaphoreCreateBinary() \
    else \
        MessageBufferHandle_t xMessageBuffer; \
        const size_t xMessageBufferSizeBytes = 100; \
        xMessageBuffer = xMessageBufferCreate( xMessageBufferSizeBytes ); 
