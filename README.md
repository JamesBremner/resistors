# Resistors

Find relationships between resistor pairs.

## Definitions
Resistors are series if they share a node that is not connected to any other resitors

Resistors are parallel if they connect the same two nodes

## Input

Input is a space delimited text file.  Resistors are specified by a line of 3 or more tokens: `name node1 node2`.  Queries are specified by a line of two tokens: `name1 name2`

## Example

![image](https://github.com/JamesBremner/resistors/assets/2046227/4268c6ee-1515-4322-b9e5-2d0836cc5a60)

```
R1 Vdd a 1
R2 a b 1
R3 b GND 1
R4 a GND 1
R1 R2
R1 R4
R2 R3
```

## Output

```
C:\Users\James\code\resistors\bin>starter ../dat/test3.txt
Vdd a connected by R1
a b connected by R2
b GND connected by R3
a GND connected by R4
R1 and R2  nothing
R1 and R4  nothing
R2 and R3  series
```

