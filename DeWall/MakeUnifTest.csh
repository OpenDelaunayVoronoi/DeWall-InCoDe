date > UnifResult
foreach i (1000 2000 3000 4000 5000 6000 7000 8000 9000 10000)
  foreach j (1 2 3 4)
   echo unif/$i.b.$j.tst >>UnifResult
   dewall ~/Triangulator/tst/dewall/unif/$i.b.$j.tst nul >>UnifResult
  end
end
