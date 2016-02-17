\ENCODING=ISO-8859-1
\Problem name: IloCplex

Maximize
 obj: x1 + 2 x2 + 3 x3 + x4
Subject To
 c1: - x1 + x2 + x3 <= 20
 c2: x1 - 3 x2 + x3 <= 30
Bounds
 0 <= x1 <= 40
      x4 = 0
End
