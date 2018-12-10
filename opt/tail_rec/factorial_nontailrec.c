__attribute__((noinline))
int
factorial(int arg)
{
    if (arg == 0)
        return 1;
    return arg * factorial(arg - 1);
}

int
main(int argc, char* argv[])
{
    return factorial(argc);
}
