#pragma once

namespace ucb
{
    class Value;
    class Instruction;

    enum ValueTy : int
    {
        VT_VIRTUAL_REG,
        VT_GLOBAL,
        VT_INT_LITERAL,
        VT_BASIC_BLOCK
    };

    /*

    enum AtomicTy : char
    {
        AT_SIGNED_INT,
        AT_UNSIGNED_INT,
        AT_FLOAT
    };

    struct DataTy
    {
        AtomicTy base{AT_SIGNED_INT};
        char size{32};
        char ptr_depth{0};
        */





    class Value
    {
    public:
        Value();

        ValueTy vt();
        dt();

        def();
        uses();

    private:

    };
}
