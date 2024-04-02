# ASM_Compiler

## How to run
1. run make clean command to clean any old builds
2. run make
3. start program by typing ./comp [testFile]

## Known tests that are failing

Failing with test2, seems to be calling a semanaticsError but still giving ASM output, just calling READ incorrectly. Everything else seems to be in working order.

## Task

### BNF
```
<program> -> <vars> xopen <stats> xclose
<vars>    -> empty | xdata <varList>
<varList> -> identifier : integer ; | identifier : integer <varList>
<exp>     -> <M> / <exp> | <M> * <exp> | <M>
<M>       -> <N> + <M> | <N>
<N>       -> <R> - <N> | ~ <N> | <R>
<R>       -> ( <exp> ) | identifier | integer
<stats>   -> <stat> <mStat>
<mStat>   -> empty | <stat> <mStat>
<stat>    -> <in> | <out> | <block> | <if> | <loop> | <assign>
<block>   -> {<vars> <stats>}
<in>      -> xin >> identifier;
<out>     -> xout << <exp>;
<if>      -> xcond [<exp> <RO> <exp>] <stat>
<loop>    -> xloop [<exp> <RO> <exp>] <stat>
<assign>  -> xlet identifier <exp>;
<R0>      -> <<(onetoken) | >> (one token) | < | >| = | %
```
