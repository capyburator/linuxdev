set pagination off

set $__n = 0
b range_get if ($__n++, 28 <= $__n && $__n <= 35)

command 1
    printf "@@@Range(start=%lld, end=%lld, stride=%lld, value=%lld)\n", this->start, this->end, this->stride, this->value
    c
end

r -100 100 3 > /dev/null
quit
