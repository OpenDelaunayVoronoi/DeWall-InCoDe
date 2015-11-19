foreach i (1000 2000 3000 4000 5000 6000 7000 8000 9000 10000)
  foreach j (1 2 3 4)
   bubbles    -n -s 123$j$i $i 1 5 >~/Triangulator/tst/dewall/bub/$i.b.$j.tst
   bubbles -Q -n -s 123$j$i $i 1 5 >~/Triangulator/tst/qhull/bub/$i.b.$j.tst
   bubbles -D -n -s 124$j$i $i 1 5 >~/Triangulator/tst/detri/bub/$i.b.$j.tst
   bubbles    -u -s 123$j$i $i 1 1 >~/Triangulator/tst/dewall/unif/$i.b.$j.tst
   bubbles -Q -u -s 123$j$i $i 1 1 >~/Triangulator/tst/qhull/unif/$i.b.$j.tst
   bubbles -D -u -s 124$j$i $i 1 1 >~/Triangulator/tst/detri/unif/$i.b.$j.tst
  end
end
