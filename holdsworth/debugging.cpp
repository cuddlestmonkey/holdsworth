/* vim: set ts=8 sts=4 sw=4 expandtab: */
/***************************************************************************
 *   Copyright (C) 2006 by Vince Negri                                     *
 *   vince.negri@gmail.com                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "debugging.h"
#include <QDebug>

#ifndef NUM_ELEMENTS
# define NUM_ELEMENTS(x)        (sizeof(x) / sizeof(x[0]))
#endif /* !NUM_ELEMENTS */


namespace Holdsworth {
    static const char* lilykeys_sharp[] = {
        "\\key c \\major",
        "\\key g \\major",
        "\\key d \\major",
        "\\key a \\major",
        "\\key e \\major",
        "\\key b \\major",
        "\\key fs \\major"
    };


    static const char* lilykeys_flat[] = {
        "\\key c \\major",
        "\\key f \\major",
        "\\key bf \\major",
        "\\key ef \\major",
        "\\key af \\major",
        "\\key df \\major",
        "\\key gf \\major"
    };

    const char* dbgLilypondKeySig(int cycle_of_fifths)
    {
        if (cycle_of_fifths < 0) {
            return lilykeys_flat[-cycle_of_fifths];
        }
        else {
            return lilykeys_sharp[cycle_of_fifths];
        }
    }

    /*!
     * Note names (starting from the Low E on the guitar) for
     * Lilypond export.
     */
    static const char* lilynotenames_sharp[] = {
        "e,",
        "f,",
        "fsharp,",
        "g,",
        "gsharp,",
        "a,",
        "asharp,",
        "b,",
        "c",
        "csharp",
        "d",
        "dsharp",
        "e",
        "f",
        "fsharp",
        "g",
        "gsharp",
        "a",
        "asharp",
        "b",
        "c'",
        "csharp'",
        "d'",
        "dsharp'",
        "e'",
        "f'",
        "fsharp'",
        "g'",
        "gsharp'",
        "a'",
        "asharp'",
        "b'",
        "c''",
        "csharp''",
        "d''",
        "dsharp''",
        "e''",
        "f''",
        "fsharp''",
        "g''",
        "gsharp''",
        "a''",
        "asharp''",
        "b''",
        "c'''",
        "csharp'''",
        "d'''",
        "dsharp'''",
        "e'''",
        "f'''",
        "fsharp'''",
        "g'''",
        "gsharp'''",
        "a'''",
        "asharp'''",
        "b'''"
    };
		
    static const char* lilynotenames_flat[] = {
        "e,",
        "f,",
        "gf,",
        "g,",
        "af,",
        "a,",
        "bf,",
        "b,",
        "c",
        "df",
        "d",
        "ef",
        "e",
        "f",
        "gf",
        "g",
        "af",
        "a",
        "bf",
        "b",
        "c'",
        "df'",
        "d'",
        "ef'",
        "e'",
        "f'",
        "gf'",
        "g'",
        "af'",
        "a'",
        "bf'",
        "b'",
        "c''",
        "df''",
        "d''",
        "ef''",
        "e''",
        "f''",
        "gf''",
        "g''",
        "af''",
        "a''",
        "bf''",
        "b''",
        "c'''",
        "df'''",
        "d'''",
        "ef'''",
        "e'''",
        "f'''",
        "gf'''",
        "g'''",
        "af'''",
        "a'''",
        "bf'''",
        "b'''"
    };

    void dbgDumpNoteList(const NoteList &the_notelist)
    {
        qDebug("DumpNoteList:-");

        for (NoteList::const_iterator n = the_notelist.begin();
                n != the_notelist.end();
                ++n)
        {
            qDebug("%s", (*n).dbgDump().c_str());
        }
    }
    void dbgLilypondDumpNoteList(const NoteList &the_notelist, QTextStream& os, bool with_extras, bool use_flats, bool show_annotations)
    {
#ifdef EXTRA_DEBUG
        qDebug("dbgLilypondDumpNoteList");
#endif
        const char** lilynotenames = lilynotenames_sharp;
        if (use_flats) {
            lilynotenames = lilynotenames_flat;
        }
        os << "\\override Glissando #'gap = #0.01" << endl;
        os << "\\override Glissando #'style = #'zigzag" << endl;

        if (with_extras) {
        //    os << "\\override TupletNumber #'stencil = ##f" << endl;
        }

	int ticks_added = 0;
        uint last_dur = 0;

        uint triplet_counter = 0;

        for (NoteList::const_iterator n = the_notelist.begin();
                n != the_notelist.end();
                ++n)
        {

            uint dur = (*n).duration();

            QString a = QString((*n).annotationAsStr().c_str());


            ticks_added += dur;

            /*! \todo Magic Number of 40 must be removed! */
            int i = (*n).noteNum() - 40;
            bool is_triplet = false;

            if (i == -40) {
                //rest
            }
            else if ((i < 0) || ((unsigned) i > NUM_ELEMENTS(lilynotenames_flat))) {
                qDebug() << "NOTE OUT OF RANGE";
                continue;
            }

            if (dur == 320) { // quaver triplet
                dur = 480;
                is_triplet = true;
                ++triplet_counter;
            }
            if (dur == 160) { // semiquaver triplet
                dur = 240;
                is_triplet = true;
                ++triplet_counter;
            }

            if (is_triplet && (triplet_counter == 1)) {
                os << "\\times 2/3 {";
            }
            
            if (i == -40) {
                os << "r";
            }
            else {
                os << lilynotenames[(*n).noteNum() - 40];
            }

            if (dur != last_dur) {
                last_dur = dur;
                if (dur == (480 * 3)) {
                    os << "4.";
                }
                else if (dur == (960 * 3)) {
                    os << "2.";
                }
                else {
                    os << (960 * 4) / dur;
                }
            }

            if (i != -40) {
                if (!with_extras) {
                    os << "\\" ;
                    os << 7 - (*n).stringNum(); /* Invert stringnum for Lilypond */
                }

                if (with_extras) {
                    a.remove('/');//.remove('=');
                    if (triplet_counter != 0) {
                        os << "-\\tweak #'extra-offset #'(0 . 1.0)";
                    }
                    os << "-";
                    os << (*n).fingerNum();
                }

                NoteList::const_iterator nn = n;
                ++nn;
                if (nn != the_notelist.end()) {
                    if ((*nn).hasAnnotation(HINT_GLISS)) {
                        os << "\\glissando ";
                    }
                }

                if (with_extras) {
                    if (a.isEmpty() && (*n).lilypondDiagram().empty()) {
                    }
                    else {
                        os << "^\\markup { \\center-column {  ";
                        if (!(*n).lilypondDiagram().empty()) {
                            os << (*n).lilypondDiagram().c_str();
                        }

                        if (!a.isEmpty() && show_annotations) {
                            os << "\"" << a << "\"";
                        }
                        os << " } }";
                    }
                }
            }

            if (triplet_counter == 3) {
                os << "}";
                triplet_counter = 0;
            }

            os << endl;

        }
        if (triplet_counter != 0) {
            os << "}";
            triplet_counter = 0;
        }

        /*
         * Convert ticks_added to semiquavers
         */
        uint notes_added = ticks_added / 240;
	if ((notes_added % 16) != 0) {
	    int foo = 16 - (notes_added % 16);

	    if (foo % 2) {
		os << " r16 ";
		--foo;
	    }
	    if (foo % 4) {
		os << " r8 ";
		foo -= 2;
	    }
	    if (foo % 8) {
		os << " r4 ";
		foo -= 4;
	    }
	    if (foo != 0) {
		os << " r2 ";
		foo -= 2;
	    }
	}
    }
}
