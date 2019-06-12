gstats transform
================

Apply statistical functions by group using C for speed.

!!! tip "Important"
    Run `gtools, upgrade` to update `gtools` to the latest stable version.

Syntax
------

<p><span class="codespan"><b>gstats transform</b> <i>clist</i> [if] [in] [weight] [, by(varlist) options] </span></p>

where clist is either

```
[(stat)] varlist [ [(stat)] ... ]
[(stat)] target_var=varname [target_var=varname ...] [ [(stat)] ...]
```

or any combination of the `varlist` or `target_var` forms, and stat is one of

| Stat              | Description
| ----------------- | -----------
| demean            | subtract the mean (default)
| demedian          | subtract the median
| normalize         | (x - mean) / sd
| standardize       | same as normalize
| moving stat [# #] | moving statistic _stat_; # specify the relative bounds (see below)
| range stat ...    | range statistic _stat_ for observations within specified interval (see below)

`gstats moving` and `gstats range` are aliases for `gstats transform`.
In this case all the requested statistics are assumed to be moving or
range statistics, respectively. `moving` and `range` may be combined
with any one of the folloing:

| Stat        | Description
| ----------- | -----------
| mean        | means (default)
| geomean     | geometric means
| count       | number of nonmissing observations
| nmissing    | number of missing observations
| median      | medians
| p#.#        | arbitrary quantiles (#.# must be strictly between 0, 100)
| p1          | 1st percentile
| p2          | 2nd percentile
| ...         | 3rd-49th percentiles
| p50         | 50th percentile (same as median)
| ...         | 51st-97th percentiles
| p98         | 98th percentile
| p99         | 99th percentile
| iqr         | interquartile range
| sum         | sums
| nansum      | sum; returns . instead of 0 if all entries are missing
| sd          | standard deviation
| variance    | variance
| cv          | coefficient of variation (`sd/mean`)
| semean      | standard error of the mean (sd/sqrt(n))
| sebinomial  | standard error of the mean, binomial (sqrt(p(1-p)/n)) (missing if source not 0, 1)
| sepoisson   | standard error of the mean, Poisson (sqrt(mean / n)) (missing if negative; result rounded to nearest integer)
| skewness    | Skewness
| kurtosis    | Kurtosis
| max         | maximums
| min         | minimums
| select#     | `#`th smallest non-missing
| select-#    | `#`th largest non-missing
| range       | range (`max` - `min`)
| first       | first value
| last        | last value
| firstnm     | first nonmissing value
| lastnm      | last nonmissing value

### Interval format

`range stat` must specify an interval or use the `interval(...)`
option. The interval must be of the form

```
#[statlow] #[stathigh] [var]
```

This computes, for each observation `i`, the summary statistic `stat`
among all observations `j` of the source variable such that

```
var[i] + # * statlow(var) <= var[j] <= var[i] + # * stathigh(var)
```

if `var` is not specified, it is taken to be the source variable itself.
`statlow` and `stathigh` are summary statistics computed based on
_every_ value of `var`. If they are not specified, then `#` is used by
itself to construct the bounds, but `#` may be missing (`.`) to mean
no upper or lower bound. For example, given some vector `x_i` with `N`
observations, we have

```
    Input      ->  Meaning
    -------------------------------------------------------
    -2 2 time  ->  j: time[i] - 2 <= time[j] <= time[i] + 2
                   i.e. stat within a 2-period time window

    -sd sd     ->  j: x[i] - sd(x) <= x[j] <= x[i] + sd(x)
                   i.e. stat for obs within a standard dev
```

### Moving window format

Note that `moving` uses a window defined by the _observations_. That
would be equivalent to computing time series rolling window statistics
using the time variable set to `_n`. For example, given some vector `x_i`
with `N` observations, we have

`moving stat` must specify a relative range or use the `window(# #)`
option. The relative range uses a window defined by the observations.
This would be equivalent to computing time series rolling window
statistics using the time variable set to `_n`. For example, given some
variable `x` with `N` observations, we have

```
    Input  ->  Range
    --------------------------------
    -3  3  ->  x[i - 3] to x[i + 3]
    -3  .  ->  x[i - 3] to x[N]
     .  3  ->  x[1]     to x[i + 3]
    -3 -1  ->  x[i - 3] to x[i - 1]
    -3  0  ->  x[i - 3] to x[i]
     5 10  ->  x[i + 5] to x[i + 10]
```

and so on. If the observation is outside of the admisible range (e.g.
`-10 10` but `i = 5`) the output is set to missing. If you don't specify
a range in `(it:moving stat)` then the range in `window(# #)` is used.

Options
-------

### Options

- `by(varlist)` specifies the groups over which the means, etc., are to be
                calculated. It can contain any mix of string or numeric variables.

- `replace` Replace allows replacing existing variables with merge.

- `wildparse` specifies that the function call should be parsed assuming
              targets are named using rename-stle syntax. For example,
              `gcollapse (sum) s_x* = x*, wildparse`

- `labelformat(str)` Specifies the label format of the output. #stat# is
            replaced with the statistic: #Stat# for titlecase, #STAT# for
            uppercase, #stat:pretty# for a custom replacement; #sourcelabel# for
            the source label and #sourcelabel:start:nchars# to extract a
            substring from the source label. The default is (#stat#)
            #sourcelabel#. #stat# palceholders in the source label are also
            replaced.

- `labelprogram(str)` Specifies the program to use with #stat:pretty#.
            This is an rclass that must set prettystat as a return value. The
            program must specify a value for each summary stat or return
            #default# to use the default engine. The programm is passed the
            requested stat by gcollapse.

- `autorename[(str)]` Automatically name targets based on requested stats.
            Default is `#source#_#stat#`.

- `nogreedy` Use slower but memory-efficient (non-greedy) algorithm.

- `types(str)` Override variable types for targets (**use with caution**).

- `window(lower upper)` Relative observation range for moving statistics
            (if not specified in call). E.g. `window(-3 1)` means from 3
            lagged observations to 1 leading observation, inclusive. 0
            means up to or from the current observation; window(. #)`
            and `window(# .)` mean from the start and through the end,
            respectively.

### Gtools

(Note: These are common to every gtools command.)

- `compress` Try to compress strL to str#. The Stata Plugin Interface has
            only limited support for strL variables. In Stata 13 and
            earlier (version 2.0) there is no support, and in Stata 14
            and later (version 3.0) there is read-only support. The user
            can try to compress strL variables using this option.

- `forcestrl` Skip binary variable check and force gtools to read strL variables
            (14 and above only). __Gtools gives incorrect results when there is
            binary data in strL variables__. This option was included because on
            some windows systems Stata detects binary data even when there is none.
            Only use this option if you are sure you do not have binary data in your
            strL variables.

- `verbose` prints some useful debugging info to the console.

- `benchmark` or `bench(level)` prints how long in seconds various parts of the
            program take to execute. Level 1 is the same as `benchmark`. Levels
            2 and 3 additionally prints benchmarks for internal plugin steps.

- `hashmethod(str)` Hash method to use. `default` automagically chooses the
            algorithm. `biject` tries to biject the inputs into the
            natural numbers. `spooky` hashes the data and then uses the
            hash.

- `oncollision(str)` How to handle collisions. A collision should never happen
            but just in case it does `gtools` will try to use native
            commands. The user can specify it throw an error instead by
            passing `oncollision(error)`.

Remarks
-------

`gstats transform` applies various statistical transformations to
input data. It is similar to `gcollapse, merge` or `gegen` but for
individual-level transformations. That is, `gcollapse` takes an input
variable and procudes a single statistic; `gstats transform` applies a
function to each element of the input variable. For example, subtracting
the mean.

Every function available to `gstats transform` can be called via `gegen`.

Examples
--------

You can download the raw code for the examples below
[here  <img src="https://upload.wikimedia.org/wikipedia/commons/6/64/Icon_External_Link.png" width="13px"/>](https://raw.githubusercontent.com/mcaceresb/stata-gtools/master/docs/examples/gstats_transform.do)

### Basic usage

Syntax is largely analogous to `gcollapse`

```stata
sysuse auto, clear

gegen norm_price = normalize(price),   by(foreign)
gegen std_price  = standardize(price), by(foreign)
gegen dm_price   = demean(price),      by(foreign)

local opts by(foreign) replace
gstats transform (normalize) norm_mpg = mpg (demean) dm_mpg = mpg, `opts'
gstats transform (demean) mpg (normalize) price [w = rep78], `opts'
gstats transform (demean) mpg (normalize) xx = price, `opts' auto(#stat#_#source#)
```

### Range statistics

This can be used to compute statistics within a specified range.
It can also do rolling window statistics. This is similar to the
user-written program `rangestat`:

```stata
webuse grunfeld, clear

gstats transform (range mean -3 0 year) x1 = invest
gstats transform (range mean -3 3 year) x2 = invest
gstats transform (range mean  . 3 year) x3 = invest
gstats transform (range mean -3 . year) x4 = invest
```

These compute moving averages using a 3-year lag, a two-sided 3-year
window, a 3-year lead recursive window (i.e. from a 3-year lead back
until the first observation), and a 3-year lag reverse recursive
window (i.e. from a 3-year lag until the last observation).

You can also specify the boudns to be a summary statistic times a
scalar. For example

```stata
gstats transform (range mean -0.5sd 0.5sd) x5 = invest
```

computes the mean within half a standard deviation of invest (if we
don't specify a range variable, then the source variable is used).
We can specify different intervals per variable as well as a global
interval used whenever a variable-specific interval is not used:

```stata
local i6 (range mean -3 0 year) x6 = invest
local i7 (range mean -0.5sd 2cv mvalue) x7 = invest
local i8 (range mean) x8 = mvalue x9 = kstock

local opts labelf(#stat:pretty#: #sourcelabel#)
gstats transform `i6' `i7' `i8', by(company) interval(-3 3 year) `opts'
```

You can also exclude the current observation from the computation

```stata
gstats transform (range mean -3 0 year) x10 = invest, excludeself
gegen x11 = range_mean(invest), by(company) excludeself interval(-3 0 year)
```

### Moving statistics

Note the moving window is defined relative to the current observation.

```stata
clear
set obs 20
gen g = _n > 10
gen x = _n
gen w = mod(_n, 7)

gegen x1 = moving_mean(x), window(-2 2) by(g)
gstats transform (moving mean -1 3) x2 = x, by(g)
gstats transform (moving sd -4 .) x3 = x (moving p75) x4 = x ///
    (moving select3) x5 = x, by(g) window(-3 3)
l

drop x?
gegen x1 = moving_mean(x) [fw = w], window(-2 2) by(g)
gstats transform (moving mean -1 3) x2 = x [aw = w], by(g)
gstats transform (moving sd -4 .) x3 = x (moving p75) x4 = x [pw = w / 7], by(g) window(-3 3)
l
```