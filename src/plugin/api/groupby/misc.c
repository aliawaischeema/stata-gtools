void GtoolsGroupByHDFE (
    struct GtoolsHash *GtoolsHashInfo,
    GT_size   khashes,
    ST_double *sources,
    ST_double *targets,
    GT_size   ktargets,
    ST_double tol)
{
    GT_size i, j, k, l, m, start, nj, *ixptr;
    ST_double mean, diff, *srcptr, *trgptr;
    struct GtoolsHash *ghptr;

    if ( targets == NULL ) {
        targets = sources;
    }

    diff = 1;
    while ( diff > tol ) {
        ghptr = GtoolsHashInfo;
        for (l = 0; l < khashes; l++, ghptr++) {
            diff = 0;
            for (j = 0; j < ghptr->nlevels; j++) {
                start  = ghptr->info[j];
                nj     = ghptr->info[j + 1] - start;
                ixptr  = ghptr->index + start;
                srcptr = sources;
                trgptr = targets;
                for (k = 0; k < ktargets; k++, srcptr += ghptr->nobs, trgptr += ghptr->nobs) {
                    mean = GtoolsStatsMean(srcptr, ixptr, nj);
                    diff = GTOOLS_PWMAX(diff, fabs(mean));
                    for (i = 0; i < nj; i++) {
                        m = ixptr[i];
                        trgptr[m] = srcptr[m] - mean;
                    }
                }
            }
            if ( diff < tol ) break;
        }
    }
}

void GtoolsGroupByHDFEWeights (
    struct GtoolsHash *GtoolsHashInfo,
    GT_size   khashes,
    ST_double *sources,
    ST_double *weights,
    ST_double *targets,
    GT_size   ktargets,
    ST_double tol)
{
    GT_size i, j, k, l, m, start, nj, *ixptr;
    ST_double mean, diff, *srcptr, *trgptr;
    struct GtoolsHash *ghptr;

    if ( targets == NULL ) {
        targets = sources;
    }

    diff = 1;
    while ( diff > tol ) {
        ghptr = GtoolsHashInfo;
        for (l = 0; l < khashes; l++, ghptr++) {
            diff = 0;
            for (j = 0; j < ghptr->nlevels; j++) {
                start  = ghptr->info[j];
                nj     = ghptr->info[j + 1] - start;
                ixptr  = ghptr->index + start;
                srcptr = sources;
                trgptr = targets;
                for (k = 0; k < ktargets; k++, srcptr += ghptr->nobs, trgptr += ghptr->nobs) {
                    mean = GtoolsStatsMeanWeights(srcptr, weights, ixptr, nj);
                    diff = GTOOLS_PWMAX(diff, fabs(mean));
                    for (i = 0; i < nj; i++) {
                        m = ixptr[i];
                        trgptr[m] = srcptr[m] - mean;
                    }
                }
            }
            if ( diff < tol ) break;
        }
    }
}