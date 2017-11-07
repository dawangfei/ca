#!/usr/bin/awk -f

BEGIN {
    #FS = "[ \t]*|\(|\)"
    sum = 0
    count = 0
}

/avg/{
    sum+=$6
    count++
}

END {
    printf "%d, %.2f, %.2f\n", count,sum, sum*1.0/count
}

# count.awk
