\include "english.ly"
\header{
  title = "unmerry_x_b2b.ly"
  tagline = \markup \center-column {"Tab generated by the Holdsworth library" "Command Line: fing --statistics --back-to-back --extended --output=unmerry_x_b2b"}
}
frag = {
\override Glissando #'gap = #0.01
\override Glissando #'style = #'zigzag
d''16-3^\markup { \center-column {  \fret-diagram #"s:1;f:1;2-15-3;1-12-1;1-15-3;1-13-1;2-14-2;2-12-1;3-14-2;4-16-4;4-14-2;3-12-1;4-12-1;5-14-2;5-12-1;" } }
e''-1^\markup { \center-column {  "s" } }
g''-3
f''-1
e''-1^\markup { \center-column {  "sA" } }
csharp''-2
d''-3
csharp''-2
b'-1^\markup { \center-column {  "s" } }
a'-2
fsharp'-4
e'-2
fsharp'-4
g'-1^\markup { \center-column {  "s" } }
a'-2
g'-1^\markup { \center-column {  "s" } }
fsharp'-4
e'-2
e'-2
d'-1^\markup { \center-column {  "s" } }
e'-2
d'-1^\markup { \center-column {  "s" } }
b-2
a-1\glissando ^\markup { \center-column {  "s" } }
g-1^\markup { \center-column {  \fret-diagram #"s:1;f:1;5-10-1;6-12-3;6-10-1;""*" } }
e-3
d-1\glissando 
b,-1\glissando ^\markup { \center-column {  "s*" } }
csharp-1^\markup { \center-column {  \fret-diagram #"s:1;f:1;6-9-1;6-10-2;3-9-1;3-10-2;3-12-4;1-9-1;1-10-2;1-12-4;5-12-4;5-10-2;5-8-1;4-12-4;""*" } }
d-2
e'-1
f'-2
g'-4
csharp''-1
d''-2
e''-4
d''-2
csharp''-1
g'-4
f'-2
e'-1
a-4
g-2
f-1^\markup { \center-column {  "s" } }
g-2
a-4
d'-4^\markup { \center-column {  "L" } }
d'-4
g,-1^\markup { \center-column {  \fret-diagram #"s:1;f:1;6-3-1;6-7-4;4-3-1;4-7-4;""=s*" } }
b,-4
f-1^\markup { \center-column {  "s" } }
a-4\glissando 
b-4^\markup { \center-column {  \fret-diagram #"s:1;f:1;4-9-4;6-5-1;6-9-4;4-5-1;""*" } }
a,-1^\markup { \center-column {  "s" } }
csharp-4
g-1^\markup { \center-column {  "s" } }
b-4\glissando 
csharp'-4^\markup { \center-column {  \fret-diagram #"s:1;f:1;4-11-4;5-9-2;4-7-1;3-7-1;3-9-2;""*" } }
fsharp-2
a-1^\markup { \center-column {  "s" } }
a-1^\markup { \center-column {  "s" } }
d'-1^\markup { \center-column {  "sL" } }
e'-2
d'-1^\markup { \center-column {  "s" } }
csharp'-4
d'-1^\markup { \center-column {  "s" } }
csharp'-4
d'-1^\markup { \center-column {  "s" } }
e'-2\glissando 
fsharp'-2^\markup { \center-column {  \fret-diagram #"s:1;f:1;3-11-2;3-12-3;2-10-1;""+c" } }
g'-3
a'-1
a'-1
g'-3
fsharp'-2
g'-3\glissando 
a'-3^\markup { \center-column {  \fret-diagram #"s:1;f:1;3-14-3;2-12-1;3-12-1;2-14-3;""+c" } }
b'-1
b'-1
a'-3
g'-1
a'-3
b'-1
csharp''-3
csharp''-3
b'-1
a'-3
b'-1
csharp''-3\glissando 
d''-3^\markup { \center-column {  \fret-diagram #"s:1;f:1;2-15-3;2-17-2;1-15-1;1-17-2;1-19-4;""*c*" } }
e''-2
g''-1^\markup { \center-column {  "s" } }
a''-2
b''-4
a''-2
a''-2
}
fragt = {
\override Glissando #'gap = #0.01
\override Glissando #'style = #'zigzag
d''16\2
e''\1
g''\1
f''\1
e''\1
csharp''\2
d''\2
csharp''\2
b'\2
a'\3
fsharp'\4
e'\4
fsharp'\4
g'\3
a'\3
g'\3
fsharp'\4
e'\4
e'\4
d'\4
e'\4
d'\4
b\5
a\5\glissando 
g\5
e\6
d\6\glissando 
b,\6\glissando 
csharp\6
d\6
e'\3
f'\3
g'\3
csharp''\1
d''\1
e''\1
d''\1
csharp''\1
g'\3
f'\3
e'\3
a\5
g\5
f\5
g\5
a\5
d'\4
d'\4
g,\6
b,\6
f\4
a\4\glissando 
b\4
a,\6
csharp\6
g\4
b\4\glissando 
csharp'\4
fsharp\5
a\4
a\4
d'\3
e'\3
d'\3
csharp'\4
d'\3
csharp'\4
d'\3
e'\3\glissando 
fsharp'\3
g'\3
a'\2
a'\2
g'\3
fsharp'\3
g'\3\glissando 
a'\3
b'\2
b'\2
a'\3
g'\3
a'\3
b'\2
csharp''\2
csharp''\2
b'\2
a'\3
b'\2
csharp''\2\glissando 
d''\2
e''\2
g''\1
a''\1
b''\1
a''\1
a''\1
}
\new StaffGroup << \new Staff { \clef "G_8" \key c \major  \frag } \new TabStaff { \fragt } >> 
