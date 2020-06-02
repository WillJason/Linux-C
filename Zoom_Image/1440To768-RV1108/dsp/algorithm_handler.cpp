#include <stdio.h>

#include "error.h"
#include "section.h"
#include "typedef.h"

#include "example/copy.h"
#include "loader/loader.h"
#include "status/status.h"
#include "work/work.h"

#include "ceva/csl/csl.h"

__sys__ int algorithm_handler(struct worker *worker, struct work *work)
{
	struct algorithm_params *algorithm = &work->params.algorithm;
	uint64_t clock_start = 0, clock_end = 0;
	int ret = 0;

	switch (algorithm->type) {
	case DSP_ALGORITHM_ZOOM: {
		clock_start = mss_get_clock();
		ret = do_zoom((void *)algorithm->packet_phys);
		clock_end = mss_get_clock();
		break;
	}

	default:
		break;
	}

	return ret;
}
