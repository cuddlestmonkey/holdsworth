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

#include <qglobal.h>
#include "instrumentdefn.h"
#include "types.h"

namespace Holdsworth {

    InstrumentDefn::InstrumentDefn()
        : strings_()
    {
        /*
         * For initial testing, make this base instrument a guitar in
         * standard tuning, though this should really move into a subclass.
         */
        strings_.push_back(InstrumentString(40, 14)); /* E */
        strings_.push_back(InstrumentString(45, 16)); /* A */
        strings_.push_back(InstrumentString(50, 18)); /* D */
        strings_.push_back(InstrumentString(55, 18)); /* G */
        strings_.push_back(InstrumentString(59, 20)); /* B */
        strings_.push_back(InstrumentString(64, 22)); /* E */
    }
    
    FretPosList InstrumentDefn::candidates(const Note& the_note)
    {
        Q_ASSERT(!strings_.empty());
        const NoteNum nn = the_note.noteNum();
        
        FretPosList fpl;
        
        StringNum sn = 1;
        for (InstrumentStringList::const_iterator s = strings_.begin();
                s != strings_.end();
                ++s, ++sn)
        {
            if (((*s).basenote <= nn) && ((*s).basenote + (*s).num_frets >= nn)) {
                FretPos fp;
                fp.strg = sn;
                fp.fret = nn - (*s).basenote;
		
		if (fp.fret == 0) {
		    /*
		     * Open string.
		     */
		}
		else {
		    fpl.push_back(fp);
		}
            }
        }

        return fpl;
    }

    Note InstrumentDefn::noteAt(const FretPos& fp)
    {
        Q_ASSERT((int) fp.strg <= (int) strings_.size());
        Note n(strings_[fp.strg - 1].basenote + fp.fret);
        return n;
    }
    
    InstrumentString::InstrumentString(int the_basenote, int the_num_frets)
        : basenote(the_basenote)
        , num_frets(the_num_frets)
    {
        /*
         * Nothing
         */
    }

    /*
     * Bogus default constructor to keep templates happy
     */
    InstrumentString::InstrumentString()
        : basenote(-1)
        , num_frets(-1)
    {
        /*
         * Nothing
         */
    }
}
