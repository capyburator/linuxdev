set pagination off

b range_get if this->value % 5 == 0

command 1
    printf "@@@Range(start=%lld, end=%lld, stride=%lld, value=%lld)\n", this->start, this->end, this->stride, this->value
    c
end

r 1 12 > /dev/null
quit
