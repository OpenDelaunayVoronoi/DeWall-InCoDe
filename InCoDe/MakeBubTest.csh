date > BubResult
foreach i (1000 2000 3000 4000 5000 6000 7000 8000 9000 10000)
  foreach j (1 2 3 4)
   echo bub/$i.b.$j.tst >>BubResult
   incode ~/Triangulator/tst/dewall/bub/$i.b.$j.tst nul >>BubResult
  end
end
