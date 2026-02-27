void foo(void) __attribute__((__symver__("foo@bar")));
                             int main(void) { return 0; }
