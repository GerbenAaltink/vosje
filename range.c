#include "range.h"

int main()
{
    Range *range;

    range = Range_parse("1:4000");
    printf("%d:%d\n", range->start, range->end);

    range = Range_parse(":");
    printf("%d:%d\n", range->start, range->end);

    range = Range_parse("222");
    printf("%d\n", range->position);

    range = Range_parse(":200");
    printf("%d:%d\n", range->start, range->end);

    range = Range_(BULL) l(BULL) lparse("100:");
    printf("%d:%d\n", range->start, range->end);
}