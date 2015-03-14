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

#ifndef HOLDSWORTH_INSTRUMENTDEFN_H
#define HOLDSWORTH_INSTRUMENTDEFN_H

#include <vector>
#include <holdsworth/note.h>

namespace Holdsworth {

/*! \brief Defintion of an instrument string.
 *
 */
struct InstrumentString {
    InstrumentString();
    InstrumentString(int, int);

    NoteNum basenote;	/*!< Note of open string */

    /*!
     *  \brief Number of frets available on this string.
     *
     *  A value of zero indicates that this is a non-fretted (drone) string.
     */
    int num_frets;
};

/*! \brief A list of strings.
 *
 * Although not mandated at this level, the strings should be in rising order of pitch.
 */
typedef std::vector<InstrumentString> InstrumentStringList;

/*! \brief Definition of an instrument.
 *
 * An instrument comprises one or more strings.
 */
class InstrumentDefn
{
public:
    InstrumentDefn();
    virtual ~InstrumentDefn() {/*empty*/}

    /*! \brief Return candidate fret positions for a given note.
     */
    FretPosList candidates(const Note&);

    /*! \brief Return the note number for a given fret position.
     */
    Note noteAt(const FretPos& fp);

protected:
    InstrumentStringList strings_;

};

}
#endif /* HOLDSWORTH_INSTRUMENTDEFN_H */
