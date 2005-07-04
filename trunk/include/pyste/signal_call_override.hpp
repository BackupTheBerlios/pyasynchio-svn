#pragma once

template<class SIG, int arity = SIG::slot_function_type::arity> struct call_override;

template<class SIG>
struct call_override<SIG, 0>
{
    static void call(SIG *sig)
    {
        (*sig)();
    }
};

template<class SIG>
struct call_override<SIG, 1>
{
    static void call(SIG *sig
        , typename SIG::slot_function_type::arg1_type arg1)
    {
        (*sig)(arg1);
    }
};

template<class SIG>
struct call_override<SIG, 2>
{
    static void call(SIG *sig
        , typename SIG::slot_function_type::arg1_type arg1
        , typename SIG::slot_function_type::arg2_type arg2)
    {
        (*sig)(arg1, arg2);
    }
};

template<class SIG>
struct call_override<SIG, 3>
{
    static void call(SIG *sig
        , typename SIG::slot_function_type::arg1_type arg1
        , typename SIG::slot_function_type::arg2_type arg2
        , typename SIG::slot_function_type::arg3_type arg3)
    {
        (*sig)(arg1, arg2, arg3);
    }
};

template<class SIG>
struct call_override<SIG, 4>
{
    static void call(SIG *sig
        , typename SIG::slot_function_type::arg1_type arg1
        , typename SIG::slot_function_type::arg2_type arg2
        , typename SIG::slot_function_type::arg3_type arg3
        , typename SIG::slot_function_type::arg4_type arg4)
    {
        (*sig)(arg1, arg2, arg3, arg4);
    }
};


template<class SIG>
struct call_override<SIG, 5>
{
    static void call(SIG *sig
        , typename SIG::slot_function_type::arg1_type arg1
        , typename SIG::slot_function_type::arg2_type arg2
        , typename SIG::slot_function_type::arg3_type arg3
        , typename SIG::slot_function_type::arg4_type arg4
        , typename SIG::slot_function_type::arg5_type arg5)
    {
        (*sig)(arg1, arg2, arg3, arg4, arg5);
    }
};
