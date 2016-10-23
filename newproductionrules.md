## Grammar Productions | First Token Set:

P --> { {S} }                     |    { { }

S --> A | G | O | C | W           |    { let, read, print, if, while }

A --> let ID := E ;               |    { let }

E --> B {( and | or ) B }         |    { not, -, (, ID, FLOATLIT }

B --> R [( < | > | == ) R ]       |    { not, -, (, ID, FLOATLIT }

R --> T {( + | - ) T }            |    { not, -, (, ID, FLOATLIT }

T --> F {( * | / ) F }            |    { not, -, (, ID, FLOATLIT }

F --> [ not | - ] U               |    { not, -, (, ID, FLOATLIT }

U --> ID | FLOATLIT | (E)         |    { (, ID, FLOATLIT }

G --> read [ STRINGLIT ] ID;      |    { read }

O --> print [ STRINGLIT ] [ ID ]; |    { print }

C --> if (E) P [ else P ]         |    { if }

W --> while (E) P                 |    { while }
