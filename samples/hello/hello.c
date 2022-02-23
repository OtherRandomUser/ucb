extern int ucb_linux_write(int file, char *buf, int size);

int main()
{
    ucb_linux_write(0, "hello!\n", 7);
    return 0;
}
