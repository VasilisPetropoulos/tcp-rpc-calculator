struct a_Y {
    float a;
    int Y<>;
};

typedef int max_min[2];
typedef float a_mul_Y<>;

program CALC_PROG {
    version CALC_VERSION {
        float AVG(a_Y) = 1;
        max_min MAX_MIN(a_Y) = 2;
        a_mul_Y MUL_a_Y(a_Y) = 3;
    } = 1;
} = 0x23451111;