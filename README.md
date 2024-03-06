# Progress bar in ANSI C for ascii terminals

This is a really simple progressbar implementation in ANSI C99.
I want it to be beautiful yet simple. General yet not requiring
much configuration.

Super simple API. Only one function:
```c
#include "progress.h"
#define MAX_COUNT 100
int main()
{
    for(int i=0; i <= MAX_COUNT; i++ ){
        /* Do something */
        progress_ascii( i, MAX_COUNT, "Some text: ");
    }
    return 0;
}
```

I have added some flexibility in the leading text by having the function variadic, just like a printf()
You can hench call the function like:

```c
        progress_ascii( i, maxvalue, "(%4d/%4d) ", i, maxvalue);
```

## Estimated time

I have added the feature to estimate the ETA of a long process. This feature
sets a start time when the call to progress_ascii() gets 0 as it first
argument.

Please note that you probably want to call the function with a final 100%
value as well. In that case the total spent time is reported.

So... Make sure that you call the function with both 0 at start and 100%-value at the end.

